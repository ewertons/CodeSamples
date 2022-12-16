# Description
This application is testing mbedtls_rsa_pkcs1_* functions of Mbed-TLS between versions v2.28.1 and v3.2.1.

The need arised after ESPRESSIF ESP-IDF v5 was released.
We use ESP-IDF to build our [ESP32 ADU sample](https://github.com/Azure-Samples/iot-middleware-freertos-samples/tree/main/demos/projects/ESPRESSIF/adu) based on [Azure IoT Middleware for FreeRTOS](https://github.com/Azure/azure-iot-middleware-freertos).

There were quite some changes between versions 2x and 3x, and a guide was published showing how to migrate from one version to the other: [Version 3.0 Migration Guide](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/3.0-migration-guide.md)

For the [ADU JWS code](https://github.com/Azure/azure-iot-middleware-freertos/blob/54fa7819c5e3ca76dc4c6b0f9ef0d233ed5b273a/ports/mbedTLS/azure_iot_jws_mbedtls.c#L187), the most impactful changes was this:
[Remove the mode parameter from RSA functions](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/3.0-migration-guide.md#remove-the-mode-parameter-from-rsa-functions)

"Remove the mode parameter from RSA functions
This affects all users who use the RSA encrypt, decrypt, sign and verify APIs.

The RSA module no longer supports private-key operations with the public key or vice versa. As a consequence, RSA operation functions no longer have a mode parameter. If you were calling RSA operations with the normal mode (public key for verification or encryption, private key for signature or decryption), remove the MBEDTLS_RSA_PUBLIC or MBEDTLS_RSA_PRIVATE argument. If you were calling RSA operations with the wrong mode, which rarely makes sense from a security perspective, this is no longer supported."

# Links
[MbedTLS online documentation](https://os.mbed.com/teams/sandbox/code/mbedtls/docs/tip/rsa_8h.html#a047b85b9483a9abecaf31af0caa2e7ff)

