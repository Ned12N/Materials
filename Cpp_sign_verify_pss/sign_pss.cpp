/* 
### Compile this code ###
g++ -o sign_rsa_pss sign_pss.cpp -lcrypto -lssl

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

// Function to read a file into a vector of unsigned char
std::vector<unsigned char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::vector<unsigned char> contents((std::istreambuf_iterator<char>(file)),
                                        std::istreambuf_iterator<char>());
    return contents;
}

int main() {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    const std::string keyfile = "keys/private_key.pem";
    const std::string messagefile = "data.txt";

    std::vector<unsigned char> message = readFile(messagefile);
    EVP_PKEY* pkey = loadPrivateKey(keyfile);
    
    std::vector<unsigned char> signature;
    if (signMessage(pkey, message, signature)) {
        std::ofstream sigFile("signature.bin", std::ios::binary);
        sigFile.write(reinterpret_cast<const char*>(signature.data()), signature.size());
        sigFile.close();
        std::cout << "Signature created successfully.\n";
    } else {
        std::cerr << "Failed to create signature.\n";
    }

    EVP_PKEY_free(pkey);
    return 0;
}
