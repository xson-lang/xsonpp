#pragma once
#include "xsonpp/io/parser.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <type_traits>
#include <zsimd/arch/scalar.hpp>

#include "xsonpp/error/code.hpp"
#include "xsonpp/xson/segment.hpp"
#include "xsonpp/xson/value/object.hpp"

#define VERIFY_RESULT(fn) \
if (auto r = fn; r.has_error()) \
	return r.assume_error();

#define VERIFY_CMP(cmp) \
if(simd::is_all_zeros(cmp)) goto next_block


namespace xson {
	template<template<typename...> class KVMapTy> 
	result<KVMapTy<string, string>> parser::parse(const parser::char_type* const char_ptr, std::size_t length) noexcept {
		using simd = zsimd::scalar;
		using vector = simd::vector<char_type>;
		using mask = simd::mask<char_type>;
		using map_type = KVMapTy<string, string>;

		if(!char_ptr || length == 0) return map_type{};
		//if(length % vector::data_size != 0)
		map_type ret;


		enum delim : char_type {
			zero = '\0', separator = ':', newline = '\n', 
			obj_open = '{', obj_close = '}', arr_open = '[', arr_close = ']', 
			comment = '/', comment_ml = '*', directive = '#',
			tab = '\t', space = ' ', cr = '\r'
		};

		constexpr static auto eq_whitespace = [](vector v) noexcept -> vector {
			return 
				simd::eq(v, simd::broadcast<char_type>(space)) | 
				simd::eq(v, simd::broadcast<char_type>(tab)) | 
				simd::eq(v, simd::broadcast<char_type>(cr)) |  
				simd::eq(v, simd::broadcast<char_type>(newline)) |  
				simd::eq(v, simd::broadcast<char_type>(zero));
		};

		//constexpr static vector oog = eq_whitespace(simd::loadu("01\t\r\n 12"));
		//constexpr static char yog = simd::to_mask(oog);

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
			const bool exists = !ret.emplace(
				string{ key_str },
				string(&char_ptr[begin_idx], end_idx - begin_idx + 1)
			).second;
			key_str = {};
			return exists;
		};

		
		vector block;
		for(std::size_t block_idx = 0; i < length; i += vector::slots, block_idx = 0) {
			block = (i + vector::slots) <= length ? 
				simd::loadu(&char_ptr[i]) : 
				simd::ploadu(&char_ptr[i], length - i); 
			mask whitespace_mask = simd::to_mask(eq_whitespace(block));

		parse_nonws:
			switch(mode) {
			find_next_nonws:
			default: {
				mask ignore_mask = whitespace_mask | simd::left_mask<false, char_type>(block_idx); //!!!!!!!
				if(simd::is_ones(ignore_mask)) continue;
				block_idx = simd::countl_one(ignore_mask);
				begin_idx = i + block_idx;


				switch(char_ptr[begin_idx]) {
				case directive:
					return make_parse_error(error::content_before_directive);

				case obj_open:
					obj_depth = 1;
					++block_idx;
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
				mask delim_mask = simd::to_mask(simd::eq(block, simd::broadcast<char_type>(in_value ? newline : separator))) & simd::right_mask<true, char_type>(block_idx);

				//if such a delimier exists, then mask it out of the block (i.e. consider it whitespace) as it should not be included in the current key/value
				mask ignore_mask = whitespace_mask;
				if(!simd::is_zeros(delim_mask)) ignore_mask |= simd::right_mask<true, char_type>(simd::countl_zero(delim_mask));

				//get the index of the last non-whitespace character
				//if such a character exists in the current block, then set the length of the key/value string based on it's index
				if(!simd::is_ones(ignore_mask)) end_idx = i + vector::data_size - 1 - simd::countr_one(ignore_mask);

				//move just past the first ending delimiter. If there is none (tblock_maskhere are 8 zeros, so i is already set to the next block), then continue the next block
				if(simd::is_zeros(delim_mask)) continue;

				block_idx = simd::countl_zero(delim_mask) + 1; //!

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
				mask arr_close_mask = simd::to_mask(simd::eq(block, simd::broadcast<char_type>(arr_close)));
				//If there is none (i.e. the compare vector is all zeros), then continue the next block
				if(simd::is_zeros(arr_close_mask)) continue;
				
				//move to the first array closing character
				block_idx = simd::countl_zero(arr_close_mask); //!
				end_idx = i + block_idx;
				//move just past the first array closing character
				++block_idx;
				
				add_kv();

				//set the mode back to default
				mode = none;
				goto find_next_nonws;
				}
			
			case object: {
				//find the index of the first closing and opening object character
				mask open_mask  = simd::to_mask(simd::eq(block, simd::broadcast<char_type>(obj_open ))) & simd::right_mask<true, char_type>(block_idx); //??!!
				mask close_mask = simd::to_mask(simd::eq(block, simd::broadcast<char_type>(obj_close))) & simd::right_mask<true, char_type>(block_idx); //??!!

				for(mask block_mask = open_mask | close_mask; !simd::is_zeros(block_mask); block_mask &= simd::right_mask<true, char_type>(block_idx + 1)) {
					block_idx = simd::countl_zero(block_mask); //!
					obj_depth += (close_mask & (1 << (vector::data_size - 1 - block_idx))) ? -1 : 1;
					if(obj_depth == 0) {
						end_idx = i + block_idx;
						++block_idx;
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



