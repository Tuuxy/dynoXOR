#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

// Constants used throughout the application.
// Scoped in Constants namespace to avoid global pollution & improve clarity.
namespace Constants {

inline const std::string& appName{"dynoXOR"};
inline const std::string& logFileName{"keys.log"};

// Command-line flags with shortened and long options
inline const std::string& fileFlag{"-f, --file"};
inline const std::string& keyFlag{"-k, --key"};
inline const std::string& outFlag{"-o, --output"};
inline const std::string& overwriteFlag{"-O, --overwrite"};
inline const std::string& backupFlag{"-b, --backup"};
inline const std::string& generateFlag{"-g, --generate"};
inline const std::string& logFlag{"-l, --log"};

// Descriptions appearing in CLI help messages
inline const std::string& fileFlagDescription{
    "Specify the input file to encrypt or decrypt."};
inline const std::string& keyFlagDescription{
    "Provide the XOR key for encryption/decryption."};
inline const std::string& outFlagDescription{
    "Specify the output file for the result."};
inline const std::string& overwriteFlagDescription{
    "Skip confirmation and overwrite the output file if it exists."};
inline const std::string& backupFlagDescription{
    "Create a backup (.bak) of the original input file before modification."};
inline const std::string& generateFlagDescription{
    "Generate a random XOR key instead of supplying a custom key."};
inline const std::string& logFlagDescription{
    "Log used XOR keys alongside their corresponding filenames for auditing."};

// Minimum Allowed XOR key size
inline const int minimumKeySize{16};
// Length of generated random XOR key
inline const int generatedKeySize{64};
// Buffer chunk size for file processing (64 KiB)
inline const int chunkSize{64 * 1024};

}  // namespace Constants

#endif