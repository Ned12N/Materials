/* 
### Compile this code ###
g++ -o verify_rsa_pss verify_sign_pss.cpp -lcrypto -lssl



### Verify the signature ###
openssl dgst -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:32 -verify public_key.pem -signature signature.bin data.txt   


 */
#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/bio.h>


// Function to load an RSA private key from a file
EVP_PKEY* loadPrivateKey(const std::string& keyfile) {
    BIO* bio = BIO_new_file(keyfile.c_str(), "r");
    if (!bio) {
        std::cerr << "Unable to open private key file\n";
        return nullptr;
    }
    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!pkey) {
        std::cerr << "Failed to read private key\n";
        return nullptr;
    }
    return pkey;
}

// Function to load a public key from a file
EVP_PKEY* loadPublicKey(const std::string& keyfile) {
    BIO* bio = BIO_new_file(keyfile.c_str(), "r");
    if (!bio) {
        std::cerr << "Unable to open public key file\n";
        return nullptr;
    }
    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!pkey) {
        std::cerr << "Failed to read public key\n";
        return nullptr;
    }
    return pkey;
}

// Function to sign a message using RSA-PSS with specific salt length
bool signMessage(EVP_PKEY* pkey, const std::vector<unsigned char>& message, std::vector<unsigned char>& signature) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return false;

    EVP_PKEY_CTX* pkey_ctx = nullptr;
    if (EVP_DigestSignInit(ctx, &pkey_ctx, EVP_sha256(), nullptr, pkey) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // Set RSA PSS padding
    if (EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // Set the specific salt length
    if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, 32) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    size_t sig_len;
    if (EVP_DigestSignUpdate(ctx, message.data(), message.size()) <= 0 ||
        EVP_DigestSignFinal(ctx, nullptr, &sig_len) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    signature.resize(sig_len);
    if (EVP_DigestSignFinal(ctx, signature.data(), &sig_len) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    EVP_MD_CTX_free(ctx);
    return true;
}

// Function to read a binary file into a vector
std::vector<unsigned char> readFile(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file) 
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);

    if (!file.read((char*)buffer.data(), size)) 
    {
        throw std::runtime_error("Error reading file: " + filename);
    }

    return buffer;
}


// Function to verify the signature
bool verifySignature(const std::vector<unsigned char>& message, const std::vector<unsigned char>& signature, EVP_PKEY* pkey) 
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return false;

    EVP_PKEY_CTX* pkey_ctx = nullptr;
    if (EVP_DigestVerifyInit(ctx, &pkey_ctx, EVP_sha256(), nullptr, pkey) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // Set RSA PSS padding
    if (EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    // Set the specific salt length
    if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, 32) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    if (EVP_DigestVerifyUpdate(ctx, message.data(), message.size()) <= 0) {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    int verifyResult = EVP_DigestVerifyFinal(ctx, signature.data(), signature.size());
    EVP_MD_CTX_free(ctx);
    return verifyResult == 1;
}
int main() {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    try {
        const std::string publicKeyFile = "public_key.pem";
        const std::string messageFile   = "data.txt";
        const std::string signatureFile = "signature_bat.bin";

        std::vector<unsigned char> message   = readFile(messageFile);
        std::vector<unsigned char> signature = readFile(signatureFile);

        // std::cout << "Message: " << std::string(message.begin(), message.end()) << std::endl;
        // std::cout << "Signature: " << std::string(signature.begin(), signature.end()) << std::endl;

        EVP_PKEY* pubKey = loadPublicKey(publicKeyFile);

        if (verifySignature(message, signature, pubKey)) {
            std::cout << "Verification successful: Signature is valid.\n";
        } else {
            std::cout << "Verification failed: Signature is not valid.\n";
        }

        EVP_PKEY_free(pubKey);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

