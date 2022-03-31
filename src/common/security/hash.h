#pragma once

#include <string>
#include <vector>

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

namespace mb
{
    namespace internal
    {
        template<typename Source, typename Key>
        std::vector<unsigned char> hmac(const Source& source, const Key& key, const EVP_MD* hashAlgorithm, unsigned int length)
        {
            std::vector<unsigned char> hash(length);

            HMAC_CTX* context = HMAC_CTX_new();
            if (context == nullptr)
            {
                throw std::runtime_error("cannot create HMAC_CTX");
            }

            HMAC_Init_ex(context, key.data(), key.size(), hashAlgorithm, NULL);
            HMAC_Update(context, reinterpret_cast<const unsigned char*>(source.data()), source.size());
            HMAC_Final(context, hash.data(), &length);

            HMAC_CTX_free(context);

            return hash;
        }
    }

    template<typename Data>
    std::vector<unsigned char> sha256(const Data& data)
    {
        std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);

        SHA256_CTX context;
        SHA256_Init(&context);
        SHA256_Update(&context, data.data(), data.size());
        SHA256_Final(hash.data(), &context);

        return hash;
    }

    template<typename Data>
    std::vector<unsigned char> sha512(const Data& data)
    {
        std::vector<unsigned char> hash(SHA512_DIGEST_LENGTH);

        SHA512_CTX context;
        SHA512_Init(&context);
        SHA512_Update(&context, data.data(), data.size());
        SHA512_Final(hash.data(), &context);

        return hash;
    }

    template<typename Source, typename Key>
    std::vector<unsigned char> hmac_sha512(const Source& data, const Key& key)
    {
        constexpr unsigned int length = 512 / CHAR_BIT;
        return internal::hmac(data, key, EVP_sha512(), length);
    }

    template<typename Source, typename Key>
    std::vector<unsigned char> hmac_sha256(const Source& data, const Key& key)
    {
        constexpr unsigned int length = 256 / CHAR_BIT;
        return internal::hmac(data, key, EVP_sha256(), length);
    }
}