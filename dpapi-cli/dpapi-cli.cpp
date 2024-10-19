#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <filesystem>

enum Scope {
    CurrentUser,
    LocalMachine
};

void printUsage() {
    std::cout
        << "\nUsage: dpapi-cli <command> [OPTIONS]\n"
        << "\n"
        << "A command-line tool for encrypting and decrypting data using the Windows Data Protection API (DPAPI). \n\n"
        << "The encryption key is derived either from the current user's credentials (CurrentUser scope) or \n"
        << "the machine's credentials (LocalMachine scope). Encrypted ciphertext can be printed to the console \n"
        << "in base64 format or written to a file in raw binary format. Any relative filepaths are evaluated \n"
        << "from the current working directory of the console. Absolute filepaths are used as-is.\n"
        << "\n"
        << "Commands:\n"
        << "  encrypt                    Encrypt the input data using DPAPI.\n"
        << "  decrypt                    Decrypt the input data using DPAPI.\n"
        << "\n"
        << "Options:\n"
        << "  -t, --text <STRING>        Specify a plaintext or a base64-encoded ciphertext.\n"
        << "  -i, --input-file <FILE>    Specify a file containing the data to encrypt or decrypt.\n"
        << "  -o, --output-file <FILE>   Specify a file where the result will be written to.\n"
        << "  -e, --entropy <ENTROPY>    Specify optional entropy (additional secret) for encryption/decryption.\n"
        << "  -s, --scope <SCOPE>        Specify protection scope: 'CurrentUser' or 'LocalMachine'.\n"
        << "  -h, --help                 Show this help message and exit.\n"
        << "\n"
        << "Examples:\n"
        << "  dpapi-cli encrypt --text \"Hello, world!\" --output-file encrypted.txt\n"
        << "  dpapi-cli decrypt --input-file encrypted.txt\n"
        << "\n"
        << "The default scope is 'CurrentUser'.\n\n";
}

// Resolve a path (absolute or relative) using the current working directory for relative paths
std::string resolvePath(const std::string& pathStr) {
    std::filesystem::path path(pathStr);
    if (path.is_absolute()) {
        return path.string(); // Absolute paths are used as-is
    }
    else {
        // Resolve relative path from the current working directory
        return (std::filesystem::current_path() / path).string();
    }
}

std::string base64Encode(const BYTE* buffer, DWORD length) {
    DWORD outLen = 0;
    if (!CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &outLen)) {
        return "";
    }

    std::string result(outLen, '\0');
    if (!CryptBinaryToStringA(buffer, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, &result[0], &outLen)) {
        return "";
    }
    return result;
}

std::vector<BYTE> base64Decode(const std::string& encoded) {
    DWORD outLen = 0;
    if (!CryptStringToBinaryA(encoded.c_str(), static_cast<DWORD>(encoded.size()), CRYPT_STRING_BASE64, NULL, &outLen, NULL, NULL)) {
        return {};
    }

    std::vector<BYTE> result(outLen);
    if (!CryptStringToBinaryA(encoded.c_str(), static_cast<DWORD>(encoded.size()), CRYPT_STRING_BASE64, result.data(), &outLen, NULL, NULL)) {
        return {};
    }
    return result;
}

Scope getScope(const std::string& scopeStr) {
    if (scopeStr == "CurrentUser") {
        return CurrentUser;
    }
    else if (scopeStr == "LocalMachine") {
        return LocalMachine;
    }
    else {
        std::cerr << "Invalid scope. Using 'CurrentUser'.\n";
        return CurrentUser;
    }
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Unable to open input file: '" << filename << "'.\n";
        return "";
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string buffer(size, '\0');
    if (!file.read(&buffer[0], size)) {
        std::cerr << "Error: Unable to read input file: '" << filename << "'.\n";
        return "";
    }
    return buffer;
}

int handleEncrypt(const std::string& input, const std::string& outputFile, const std::string& entropy, Scope scope) {
    DATA_BLOB inputData;
    inputData.pbData = (BYTE*)input.data();
    inputData.cbData = static_cast<DWORD>(input.size());

    DATA_BLOB entropyData = { 0, NULL };
    if (!entropy.empty()) {
        entropyData.pbData = (BYTE*)entropy.data();
        entropyData.cbData = static_cast<DWORD>(entropy.size());
    }

    DATA_BLOB outputData;

    if (CryptProtectData(&inputData, NULL, &entropyData, NULL, NULL, scope == CurrentUser ? CRYPTPROTECT_UI_FORBIDDEN : 0, &outputData)) {
        if (!outputFile.empty()) {
            std::ofstream out(outputFile, std::ios::binary);
            out.write((char*)outputData.pbData, outputData.cbData);
            std::cout << "Encrypted data written to: '" << outputFile << "'.\n";
        }
        else {
            std::string encryptedData = base64Encode(outputData.pbData, outputData.cbData);
            std::cout << encryptedData << std::endl;
        }
        LocalFree(outputData.pbData);
        return 0;
    }
    else {
        std::cerr << "Encryption failed.\n";
        return 1;
    }
}

int handleDecrypt(const std::string& input, const std::string& outputFile, const std::string& entropy, Scope scope, bool fromFile) {
    std::vector<BYTE> decodedData;

    if (fromFile) {
        decodedData.assign(input.begin(), input.end());
    }
    else {
        decodedData = base64Decode(input);
    }

    DATA_BLOB inputData;
    inputData.pbData = decodedData.data();
    inputData.cbData = static_cast<DWORD>(decodedData.size());
    

    DATA_BLOB entropyData = { 0, NULL };
    if (!entropy.empty()) {
        entropyData.pbData = (BYTE*)entropy.data();
        entropyData.cbData = static_cast<DWORD>(entropy.size());
    }

    DATA_BLOB outputData;

    if (CryptUnprotectData(&inputData, NULL, &entropyData, NULL, NULL, scope == CurrentUser ? CRYPTPROTECT_UI_FORBIDDEN : 0, &outputData)) {
        std::string decryptedData((char*)outputData.pbData, outputData.cbData);

        if (!outputFile.empty()) {
            std::ofstream out(outputFile, std::ios::binary);
            out.write(decryptedData.data(), decryptedData.size());
            std::cout << "Decrypted data written to: '" << outputFile << "'.\n";
        }
        else {
            std::cout << decryptedData << std::endl;
        }
        LocalFree(outputData.pbData);
        return 0;
    }
    else {
        std::cerr << "Decryption failed.\n";
        return 1;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];
    std::string input;
    std::string inputFile;
    std::string outputFile;
    std::string entropy;
    std::string scopeStr = "CurrentUser";
    Scope scope = CurrentUser;
    bool fromFile = false;

    for (int i = 2; i < argc; ++i) {
        if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--text") == 0) && i + 1 < argc) {
            input = argv[++i];
        }
        else if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input-file") == 0 || strcmp(argv[i], "-f") == 0) && i + 1 < argc) {
            inputFile = resolvePath(argv[++i]);
            fromFile = true;
        }
        else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output-file") == 0) && i + 1 < argc) {
            outputFile = resolvePath(argv[++i]);
        }
        else if ((strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--entropy") == 0) && i + 1 < argc) {
            entropy = argv[++i];
        }
        else if ((strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scope") == 0) && i + 1 < argc) {
            scopeStr = argv[++i];
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage();
            return 0;
        }
    }

    scope = getScope(scopeStr);

    if (!inputFile.empty()) {
        input = readFile(inputFile);
        if (input.empty()) return 1;
    }

    if (command == "encrypt") {
        if (input.empty()) {
            std::cerr << "Error: No input specified for encryption.\n";
            return 1;
        }
        return handleEncrypt(input, outputFile, entropy, scope);
    }
    else if (command == "decrypt") {
        if (input.empty()) {
            std::cerr << "Error: No input specified for decryption.\n";
            return 1;
        }
        return handleDecrypt(input, outputFile, entropy, scope, fromFile);
    }
    else {
        printUsage();
        return 1;
    }
}
