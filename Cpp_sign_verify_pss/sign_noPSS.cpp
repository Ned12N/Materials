#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/bio.h>

// Function to read a file into a vector
std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    return std::vector<char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Function to load an RSA private key from a file using BIO
EVP_PKEY* loadPrivateKey(const std::string& keyfile) {
    BIO* bio = BIO_new_file(keyfile.c_str(), "r");
    if (!bio) {
        throw std::runtime_error("Cannot open key file: " + keyfile);
    }
    EVP_PKEY* key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (!key) {
        throw std::runtime_error("Failed to load private key from: " + keyfile);
    }
    return key;
}


// Function to sign a message using RSA and SHA-256
std::vector<unsigned char> signMessage(const std::vector<char>& message, EVP_PKEY* pkey) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    if (EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize DigestSign");
    }

    if (EVP_DigestSignUpdate(ctx, message.data(), message.size()) <= 0) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update DigestSign");
    }

    size_t siglen;
    if (EVP_DigestSignFinal(ctx, NULL, &siglen) <= 0) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize DigestSign");
    }

    std::vector<unsigned char> signature(siglen);
    if (EVP_DigestSignFinal(ctx, signature.data(), &siglen) <= 0) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to get signature");
    }

    signature.resize(siglen);
    EVP_MD_CTX_free(ctx);
    return signature;
}


int main() {
    try {
        // Initialize OpenSSL algorithms and error strings
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        // Load the private key
        EVP_PKEY* privateKey = loadPrivateKey("private_key.pem");

        // Read the message from a file
        std::vector<char> message = readFile("data.txt");

        // Sign the message
        std::vector<unsigned char> signature = signMessage(message, privateKey);

        // Write the signature to a file
        std::ofstream sigFile("signature.bin", std::ios::out | std::ios::binary);
        if (!sigFile) {
            throw std::runtime_error("Cannot open signature file for writing");
        }
        sigFile.write(reinterpret_cast<const char*>(signature.data()), signature.size());

        // Cleanup
        EVP_PKEY_free(privateKey);

        std::cout << "Signature has been successfully generated and saved." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}