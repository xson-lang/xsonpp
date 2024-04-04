/*** WIP for ext 1 (comments) ***
std::vector<std::string_view> key;
std::size_t count = 0;

struct content_section {
	std::size_t idx, len;
} section;
// this vector will ideally have an aligned allocator once actual SIMD is implemented
std::vector<content_section> sections;

//first phase: strip comments
{
std::size_t comment_start_idx = 0;
std::size_t last_comment_end_idx = 0;
enum class comment_type {
	none, comment, comment_ml
} comment_mode = comment_type::none;

for(std::size_t i = 0; i + N + 1 <= length;) {
	vector lblock = loadu(&char_ptr[i]);
	vector rblock = loadu(&char_ptr[i + 1]);
	switch(comment_mode) {
	case comment_type::comment:
		last_comment_end_idx = countl_zeros_8(cmpeq_8(rblock, newline));
		if(last_comment_end_idx == N) goto next_block;
		goto comment_end;

	case comment_type::comment_ml: {
		vector comment_ml_cmp  = cmpeq_8(lblock, comment_ml);
		vector comment_end_cmp = cmpeq_8(rblock, comment);
		last_comment_end_idx = countl_zeros_8(comment_ml_cmp & (comment_end_cmp << (sizeof(char_type) * 8))) + 1;
		if(last_comment_end_idx == N + 1) goto next_block;
		goto comment_end;
	}


	comment_end:
		//if(last_comment_end_idx == N + (static_cast<std::uint32_t>(comment_mode) - 1)) goto next_block;
		i += ++last_comment_end_idx;
		comment_mode = comment_type::none; 
		continue;


	default: 
		{
		vector comment_start_cmp = cmpeq_8(lblock, comment);
		vector comment_ml_cmp    = cmpeq_8(rblock, comment_ml);
		vector comment_sl_cmp    = cmpeq_8(rblock, comment);

		std::size_t comment_sl_idx = countl_zeros_8(comment_start_cmp & (comment_sl_cmp << (sizeof(char_type) * 8)));
		std::size_t comment_ml_idx = countl_zeros_8(comment_start_cmp & (comment_ml_cmp << (sizeof(char_type) * 8)));
		if(comment_sl_idx != comment_ml_idx) {
			bool ml = comment_ml_idx < comment_sl_idx;

			comment_start_idx = ml ? comment_ml_idx : comment_sl_idx;
			i += comment_start_idx + 2;
			if(comment_start_idx > 0) sections.emplace_back(last_comment_end_idx, comment_start_idx - 1);
			
			comment_mode = ml ? comment_type::comment_ml : comment_type::comment;
			//comment_mode = static_cast<comment_type>(static_cast<std::uint32_t>(comment_type::comment) + ml);
			continue;
		}
		}

	next_block:
		i += N;
		continue;
	}
}
}
*********************************/