from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives.serialization import load_pem_public_key
from cryptography.hazmat.backends import default_backend

# Load the public key from a file
with open('public_key.pem', 'rb') as key_file:
    public_key = load_pem_public_key(key_file.read(), backend=default_backend())

# Load the data that was signed
with open('data.txt', 'rb') as data_file:
    data = data_file.read()

# Load the signature
with open('signature_CPP.bin', 'rb') as sig_file:
    signature = sig_file.read()

# Verify the signature
try:
    public_key.verify(
        signature,
        data,
        padding.PSS(
            mgf=padding.MGF1(hashes.SHA256()),
            salt_length=32
        ),
        hashes.SHA256()
    )
    print("Verification successful.")
except Exception as e:
    print("Verification failed:", str(e))


# from cryptography.hazmat.primitives import hashes
# from cryptography.hazmat.primitives.asymmetric import padding
# from cryptography.hazmat.primitives.serialization import load_pem_private_key
# from cryptography.hazmat.backends import default_backend

# # Load the private key from a file
# with open('private_key.pem', 'rb') as key_file:
#     private_key = load_pem_private_key(key_file.read(), password=None, backend=default_backend())

# # Load the data to be signed
# with open('data.txt', 'rb') as data_file:
#     data = data_file.read()

# # Sign the data
# signature = private_key.sign(
#     data,
#     padding.PSS(
#         mgf=padding.MGF1(hashes.SHA256()),
#         salt_length=32
#     ),
#     hashes.SHA256()
# )

# # Save the signature to a file
# with open('signature.bin', 'wb') as sig_file:
#     sig_file.write(signature)
