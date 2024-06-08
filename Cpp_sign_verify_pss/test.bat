@REM Generate Private Key
openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -out private_key.pem

@REM Generate Public Key
openssl rsa -pubout -in private_key.pem -out public_key.pem

@REM Apply Signature
openssl dgst -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:32 -sign private_key.pem -out signature.bin data.txt

@REM Verify Signature
openssl dgst -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:32 -verify public_key.pem -signature signature.bin data.txt

