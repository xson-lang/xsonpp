#pragma once
#include "xsonpp/io/parser.hpp"
#include <cstddef>
#include <cstring>
#include <string_view>

#include "xsonpp/result/error_code.hpp"
#include "xsonpp/xson/segment.hpp"
#include "xsonpp/xson/value/object.hpp"

#define VERIFY_RESULT(fn) \
if (auto r = fn; r.has_error()) \
	return r.assume_error();

#define VERIFY_CMP(cmp) \
if(all_zeros(cmp)) goto next_block


namespace xson {
    template<> 
	result<object> parser::parse<segment::object>(const char_type* const char_ptr, std::size_t length) noexcept {
		if(!char_ptr || length == 0) return object{};
		object ret;
		

		/******* using scalar vectorization as a placeholder until actual SIMD is implemented ******/
		using vector = std::uint64_t;
		using mask = std::uint8_t;
		constexpr static std::size_t N = sizeof(vector); //size of vector in bytes (i.e. bytes per vector)


		//create broadcasted vectors for each delimiter
		constexpr static auto bc = [](char c) noexcept -> vector { return c * 0x01'01'01'01'01'01'01'01; };
		enum delim {
			zero = '\0', separator = ':', newline = '\n', 
			obj_open = '{', obj_close = '}', arr_open = '[', arr_close = ']', 
			comment = '/', comment_ml = '*', directive = '#',
			tab = '\t', space = ' ', cr = '\r'
		};


		/** placeholder simd functions **/
		constexpr static auto loadu = [](const char_type* const data) noexcept -> vector {
			vector ret = 0;
			//can't use memcpy because of endianness
			for(std::size_t i = 0; i < N; ++i)
				ret |= static_cast<vector>(data[i]) << (8 * (N - i - 1));
			return ret;
		};
		constexpr static auto padded_load = [](const char_type* const data, std::size_t len, std::size_t i) noexcept -> vector {
			bool near_end = len - N < i; 
			vector ret = loadu(&data[near_end ? len - N : i]);
			if(near_end) ret <<= ((i - (len - N)) * sizeof(char_type));
			return ret;
		};

		constexpr static auto cmpeq_8 = [](vector a, delim bc_idx) noexcept -> vector { 
			vector result = a ^ bc(bc_idx);
			vector ret = 0;
			for (std::size_t i = 0; i < N; ++i) {
				const std::size_t shift_amt = 8 * (N - i - 1);
				char_type byte = (result >> shift_amt) & 0xFF;
				ret |= static_cast<vector>(!byte * 0xFF) << shift_amt;
			}
			return ret; 
		};
		constexpr static auto move_mask = [](vector v) noexcept -> mask {
			mask result = 0;
			for (std::size_t i = 0; i < N; ++i)
				result |= (((v >> (8 * (N - i - 1))) & 0b1000'0000) >> 7) << (N - i - 1);
			return result;
		};

		constexpr static auto rmask = [](std::size_t idx) noexcept -> mask {
			return static_cast<mask>(-1) >> idx;
		};
		constexpr static auto lmask = [](std::size_t idx) noexcept -> mask {
			return static_cast<mask>(-1) << ((sizeof(mask) * 8) - (idx + 1));
		};

		constexpr static auto all_zeros = [](mask m) noexcept -> bool {
			return m == 0;
		};
		constexpr static auto all_ones = [](mask m) noexcept -> bool {
			return m == static_cast<mask>(-1);
		};

		constexpr static auto countl_zeros = [](mask m) noexcept -> std::size_t {
			for (std::size_t i = 0; i < N; ++i) 
				if(m & (1 << (N - i - 1))) 
					return i;
			return N; 
		};
		constexpr static auto countl_ones = [](mask m) noexcept -> std::size_t {
			//std::cout << v << std::endl;
			for (std::size_t i = 0; i < N; ++i) 
				if(~m & (1 << (N - i - 1))) 
					return i;
			return N; 
		};
		constexpr static auto countr_ones = [](mask m) noexcept -> std::size_t {
			for (std::size_t i = 0; i < N; ++i) 
				if(~m & (1 << i)) 
					return i;
			return N; 
		};

		constexpr static auto cmpeq_whitespace = [](vector v) noexcept -> vector {
			return cmpeq_8(v, space) | cmpeq_8(v, tab) | cmpeq_8(v, cr) | cmpeq_8(v, newline) | cmpeq_8(v, zero);
		};

		///*first phase: strip comments (EXT 1)
		/*** WIP - see comment.ipp ***/
		
		std::size_t i = 0;
		//second phase: parse directives (must be at top of file)
		/*{

		while(i < length) {
			//load N bytes (or whatever remains if less than N bytes are left) into the block;
			vector block = padded_load(char_ptr, length, i);

			//move to the first non-whitespace character. If there is none (the function returned N, so i is already on the next block), then continue the next block
			if((i += first_not_ws(block)) == N) continue; 

			//if the first non whitespace character is a '#', then try to parse the line as a directive (if that fails then return an error).
			//otherwise, we've run into actual content, so goto the actual k/v pair construction phase
			switch(char_ptr[i]){
			case '#': {
				result<std::pair<ext_list, std::size_t>> r = parse<segment::directive>(&char_ptr[++i], length);
				if(r.has_error()) return r.assume_error();
				exts |= r->first;
				i += r->second;
				continue;
				}
			default:
				goto phase3;
			}
		}
		}*/

	//phase3:
		//third phase: construct the strings
		{
		std::string_view key_str; //string view for the key
		std::size_t begin_idx = 0, end_idx = 0; //beginning index and length for the value

		std::size_t obj_depth = 0; //how deep we are into an object

		enum parse_mode { none, str, object = obj_open, array = arr_open } mode = none;

		auto add_kv = [&key_str, &begin_idx, &end_idx, &ret, char_ptr]() noexcept {
			//try to add the key/value pair. Return whether the key already exists or not.
			const bool exists = !ret.kv_pairs.emplace(
				string{ key_str },
				string(&char_ptr[begin_idx], end_idx - begin_idx + 1)
			).second;
			key_str = {};
			return exists;
		};

		

		for(std::size_t block_idx = 0; i < length; i += N, block_idx = 0) {
			//load N bytes (or whatever remains if less than N bytes are left) into the block;
			vector block = padded_load(char_ptr, length, i);
			mask whitespace_mask = move_mask(cmpeq_whitespace(block));

		parse_nonws:
			switch(mode) {
			find_next_nonws:
			default: {
				mask ignore_mask = whitespace_mask | lmask(block_idx);
				if(all_ones(ignore_mask)) continue;
				block_idx = countl_ones(ignore_mask);
				begin_idx = i + block_idx;


				switch(char_ptr[begin_idx]) {
				case directive:
					return make_parse_error(error::content_before_directive);

				case obj_open:
					obj_depth = 1;
					[[fallthrough]];
				case arr_open:
					mode = static_cast<parse_mode>(char_ptr[begin_idx]);
					break;

				default:
					mode = str;
					break;
				}

				goto parse_nonws;
				}

			case str: {
				//get the index of the first delimiter (a newline if we're in a value; a colon if not) in the current block
				bool in_value = !key_str.empty();
				mask delim_mask = move_mask(cmpeq_8(block, in_value ? newline : separator)) & rmask(block_idx);

				//if such a delimier exists, then mask it out of the block (i.e. consider it whitespace) as it should not be included in the current key/value
				mask ignore_mask = whitespace_mask;
				if(!all_zeros(delim_mask)) ignore_mask |= rmask(countl_zeros(delim_mask));

				//get the index of the last non-whitespace character
				//if such a character exists in the current block, then set the length of the key/value string based on it's index
				if(!all_ones(ignore_mask)) end_idx = i + N - 1 - countr_ones(ignore_mask);

				//move to the first ending delimiter. If there is none (tblock_maskhere are 8 zeros, so i is already set to the next block), then continue the next block
				if(all_zeros(delim_mask)) continue;

				block_idx = countl_zeros(delim_mask);

				//if we're on a key, then set the key string to our current beginning and length
				if(!in_value) 
					key_str = {&char_ptr[begin_idx], end_idx - begin_idx + 1};
				//otherwise, add the key/value pair (the value is made using the current beginning and length) and reset the key string
				else add_kv();
				
				//set the parse mode to default
				mode = none;
				goto find_next_nonws;
				}
				

			case array: {  
				//find the first array closing character. 
				mask arr_close_mask = move_mask(cmpeq_8(block, arr_close));
				//If there is none (i.e. the compare vector is all zeros), then continue the next block
				if(all_zeros(arr_close_mask)) continue;
				
				//move to the first array closing character

				block_idx = countl_zeros(arr_close_mask);
				end_idx = i + block_idx;
				
				add_kv();

				//set the mode back to default
				mode = none;
				goto find_next_nonws;
				}
			
			case object: {
				//find the index of the first closing and opening object character
				mask open_mask  = move_mask(cmpeq_8(block, obj_open )) & rmask(block_idx + 1);
				mask close_mask = move_mask(cmpeq_8(block, obj_close)) & rmask(block_idx + 1);

				for(mask block_mask = open_mask | close_mask; !all_zeros(block_mask); block_mask &= rmask(block_idx + 1)) {
					block_idx = countl_zeros(block_mask);
					obj_depth += (close_mask & (1 << (N - 1 - block_idx))) ? -1 : 1;
					if(obj_depth == 0) {
						end_idx = i + block_idx;
						add_kv();
						mode = none;
						goto find_next_nonws;
					}
				}

				continue;
				}
			}

		//next_block:
		//	i += N;
		//	continue;
		}

		//if we reach EOF but still have a key that needs to be added, then add it (using the lingering value for the begin and length)
		if(!key_str.empty()) add_kv();
		}


		return ret;
	}
}

#undef VERIFY_RESULT
#undef VERIFY_CMP



