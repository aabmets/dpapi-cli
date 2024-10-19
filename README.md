# DPAPI-CLI

DPAPI-CLI is a command-line tool that allows you to encrypt and decrypt data using the Windows Data Protection API (DPAPI). 
It supports both user- and machine-specific encryption keys, making it suitable for various scenarios. 

## Features

- Encrypt and decrypt data using DPAPI.
- Supports `CurrentUser` and `LocalMachine` protection scopes.
- Base64 encoded output or raw binary file output.
- Optional entropy for added security.
- Input and output through files or text.


## Documentation

You can download the latest executable of this tool from the Releases section of this repository.
```text
Usage: dpapi-cli <command> [OPTIONS]

A command-line tool for encrypting and decrypting data using the Windows Data Protection API (DPAPI).

The encryption key is derived either from the current user's credentials (CurrentUser scope) or
the machine's credentials (LocalMachine scope). Encrypted ciphertext can be printed to the console
in base64 format or written to a file in raw binary format. Any relative filepaths are evaluated
from the current working directory of the console. Absolute filepaths are used as-is.

Commands:
  encrypt                    Encrypt the input data using DPAPI.
  decrypt                    Decrypt the input data using DPAPI.

Options:
  -t, --text <STRING>        Specify a plaintext or a base64-encoded ciphertext.
  -i, --input-file <FILE>    Specify a file containing the data to encrypt or decrypt.
  -o, --output-file <FILE>   Specify a file where the result will be written to.
  -e, --entropy <ENTROPY>    Specify optional entropy (additional secret) for encryption/decryption.
  -s, --scope <SCOPE>        Specify protection scope: 'CurrentUser' or 'LocalMachine'.
  -h, --help                 Show this help message and exit.

Examples:
  dpapi-cli encrypt --text "Hello, world!" --output-file encrypted.txt
  dpapi-cli decrypt --input-file encrypted.txt

The default scope is 'CurrentUser'.
```


## License

This project is licensed under the MIT License. See the LICENSE file for details.
