#pragma once

namespace kumori
{

	inline std::string encode_base64(const std::string& str)
	{
		static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		const unsigned char* const s = reinterpret_cast<const unsigned char*>(str.c_str());
		std::string result((str.size() + 2) / 3 * 4, '=');
		std::size_t src_pos = 0;
		std::size_t dst_pos = 0;

		while (true)
		{
			if (src_pos == str.size())
				break;

			result[dst_pos] = table[(s[src_pos] & 0xfc) >> 2];

			if (src_pos == str.size() - 1)
			{
				result[dst_pos + 1] = table[(s[src_pos] & 0x03) << 4];
				break;
			}

			result[dst_pos + 1] = table[((s[src_pos] & 0x03) << 4) | ((s[src_pos + 1] & 0xf0) >> 4)];

			if (src_pos == str.size() - 2)
			{
				result[dst_pos + 2] = table[(s[src_pos + 1] & 0x0f) << 2];
				break;
			}

			result[dst_pos + 2] = table[((s[src_pos + 1] & 0x0f) << 2) | ((s[src_pos + 2] & 0xc0) >> 6)];
			result[dst_pos + 3] = table[s[src_pos + 2] & 0x3f];

			dst_pos += 4;
			src_pos += 3;
		}

		return result;
	}

}
