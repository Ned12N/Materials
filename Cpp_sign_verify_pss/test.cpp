#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bio.h>

void print_hex(const unsigned char* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        printf("%02x", data[i]);
    }
    std::cout << std::endl;
}

std::vector<unsigned char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file.");
    }

    std::vector<unsigned char> contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return contents;
}

int main() {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    // Load private key
    BIO *bio = BIO_new_file("private_key.pem", "r");
    if (!bio) {
        std::cerr << "Unable to open file for reading private key\n";
        return 1;
    }
    EVP_PKEY* privateKey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (!privateKey) {
        std::cerr << "Unable to read private key\n";
        return 1;
    }

    // Message to be signed
    // unsigned char message[] = "hello bitch\n";

    // Try to read the message from "data.txt"
    std::vector<unsigned char> message;
    try {
        message = read_file("data.txt");
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    // Hash the message using SHA-256
        // Compute SHA256 hash of the message
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // Correctly use message.data() and message.size()
    SHA256(message.data(), message.size(), hash);

    // Prepare for signing
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_PKEY_CTX* pkey_ctx = NULL;
    size_t sigLen;
    unsigned char* sig;

    if (EVP_DigestSignInit(ctx, &pkey_ctx, EVP_sha256(), NULL, privateKey) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // Set PSS padding options
    EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING);
    EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, 0);

    // Determine buffer length for signature
    if (EVP_DigestSign(ctx, NULL, &sigLen, hash, sizeof(hash)) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // Allocate memory for signature
    sig = (unsigned char*)malloc(sigLen);
    if (!sig) {
        std::cerr << "Memory allocation failed for signature\n";
        return 1;
    }

    // Perform signing
    if (EVP_DigestSign(ctx, sig, &sigLen, hash, sizeof(hash)) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // Write the signature to a binary file
    std::ofstream sigFile("signature_CPP.bin", std::ios::out | std::ios::binary);
    if (!sigFile) {
        std::cerr << "Failed to open signature file for writing\n";
        return 1;
    }
    sigFile.write(reinterpret_cast<char*>(sig), sigLen);
    sigFile.close();

    // Output the signature in hexadecimal
    std::cout << "Signature (hex): ";
    print_hex(sig, sigLen);

    // Clean up
    EVP_MD_CTX_free(ctx);
    free(sig);
    EVP_PKEY_free(privateKey);

    return 0;
}
