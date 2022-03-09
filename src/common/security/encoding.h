#include <vector>
#include <string>
#include <stdexcept>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

namespace cb
{
    template<typename Data>
    std::string b64_encode(const Data& data)
    {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO* bmem = BIO_new(BIO_s_mem());
        b64 = BIO_push(b64, bmem);

        BIO_write(b64, data.data(), data.size());
        BIO_flush(b64);

        BUF_MEM* bptr;
        BIO_get_mem_ptr(b64, &bptr);

        std::string output(bptr->data, bptr->length);
        BIO_free_all(b64);

        return output;
    }

    template<typename Data>
    std::vector<unsigned char> b64_decode(const Data& data)
    {
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO* bmem = BIO_new_mem_buf((void*)data.data(), data.size());
        bmem = BIO_push(b64, bmem);

        std::vector<unsigned char> output(data.size());
        int decoded_size = BIO_read(bmem, output.data(), output.size());
        BIO_free_all(bmem);

        if (decoded_size < 0)
            throw std::runtime_error("failed while decoding base64.");

        return output;
    }
}
