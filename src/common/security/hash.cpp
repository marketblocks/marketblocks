#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "hash.h"
#include "encoding.h"

namespace
{
    std::vector<unsigned char> hmac(const std::vector<unsigned char>& source, const std::vector<unsigned char>& key, const EVP_MD* hashAlgorithm)
    {
        unsigned int length = EVP_MAX_MD_SIZE;
        std::vector<unsigned char> hash(length);

        HMAC_CTX* context = HMAC_CTX_new();
        if (context == nullptr) 
        {
            throw std::runtime_error("cannot create HMAC_CTX");
        }

        HMAC_Init_ex(context, key.data(), key.size(), hashAlgorithm, NULL);
        HMAC_Update(context, source.data(), source.size());
        HMAC_Final(context, hash.data(), &length);

        HMAC_CTX_free(context);

        return hash;
    }
}

std::vector<unsigned char> sha256(const std::string& data)
{
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);

    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, data.c_str(), data.length());
    SHA256_Final(hash.data(), &context);

    return hash;
}

std::vector<unsigned char> sha512(const std::string& data)
{
    std::vector<unsigned char> hash(SHA512_DIGEST_LENGTH);

    SHA512_CTX context;
    SHA512_Init(&context);
    SHA512_Update(&context, data.c_str(), data.length());
    SHA512_Final(hash.data(), &context);

    return hash;
}

std::vector<unsigned char> hmac_sha512(const std::vector<unsigned char>& source, const std::vector<unsigned char>& key)
{
    return hmac(source, key, EVP_sha512());
}

std::string convert_to_plain_text(const std::vector<unsigned char>& data)
{
    constexpr std::streamsize width = 2;
    constexpr char fill = '0';
    
    std::stringstream stream;
    for (int i = 0; i < data.size(); i++)
    {
        stream << std::hex << std::setw(width) << std::setfill(fill) << (int)data[i];
    }

    return stream.str();
}