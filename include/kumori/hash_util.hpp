#pragma once

namespace kumori
{

	inline std::string hash_md5(const std::string& str)
	{
		char hash[MD5_DIGEST_LENGTH];

		const unsigned char* p_str = reinterpret_cast<const unsigned char*>(str.c_str());
		unsigned char* p_hash = reinterpret_cast<unsigned char*>(hash);

		MD5(p_str, str.size(), p_hash);

		return std::string(hash, sizeof(hash));
	}

	inline std::string hash_sha1hmac(const std::string& str, const std::string& key)
	{
		char hash[SHA_DIGEST_LENGTH];

		const unsigned char* p_str = reinterpret_cast<const unsigned char*>(str.c_str());
		const void* p_key = reinterpret_cast<const void*>(key.c_str());
		int key_length = boost::numeric_cast<int>(key.size());
		unsigned char* p_hash = reinterpret_cast<unsigned char*>(hash);

		unsigned int resultLength;
		BOOST_VERIFY(HMAC(EVP_sha1(), p_key, key_length, p_str, str.size(), p_hash, &resultLength));

		return std::string(hash, resultLength);
	}

}
