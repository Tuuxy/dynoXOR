#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <string>
#include "constants.hpp"

/*
@brief Get the standard configuration directory path for storing application data.

Returns the platform-appropriate configuration directory path based on environment variables:
- On Windows, uses %LOCALAPPDATA%\dynoXOR or falls back to a relative directory.
- On MacOS, uses ~/Library/Application Support/dynoXOR or falls back.
- On Linux/Unix, uses ~/.config/dynoXOR or falls back.

@return The path to the configuration directory as a std::string.
*/
std::string getConfigDir();

/*
@brief Process the file in chunks, XORing with key and writing to outfile.
@param filename Input file path.
@param outfile Output file path.
@param xorkey XOR key string.
@param chunkSize Size of chunks to process buffer (default Constants::chunkSize).
@throws std::runtime_error on IO errors or file operation failures.*/
void processFileInChunks(const std::string& filename,
                         const std::string& outfile, const std::string& xorkey,
                         size_t chunkSize = Constants::chunkSize);

/*
@brief Log the XOR key associated with a filename to a persistent log for auditing or record-keeping.
@param xorkey The XOR key to log.
@param filename The name of the file the key is associated with.
@throws std::runtime_error if the log directory cannot be created or the log file cannot be written.
*/
void logKey(std::string& xorkey, std::string& filename);

/*
@brief Generate a random XOR key consisting of printable and special characters.
@param xorkey String to store the generated key. Resized to Constants::generatedKeySize.
*/
void generateKey(std::string& xorkey);

/*
@brief Verify that the input file exists and is readable.
@param filename The path to the input file.
@throws std::runtime_error if file does not exist, cannot be opened, or is empty.
*/
void verifyFile(const std::string& filename);

/*
@brief Validate the XOR key input state for correctness.
Ensures either a key is provided or generation flag is set, but not both.
Also enforces minimum key length constraints.
@param xorkey The XOR key string provided.
@param generate Flag indicating whether a key should be generated instead of supplied.
@throws std::runtime_error if validation fails.
*/
void verifyKey(std::string& xorkey, bool generate);

/*
@brief Verify output file settings; prompts user confirmation if overwriting without force flag.
Defaults to overwriting input file if no separate output specified.
@param outfile The path to the output file (may be empty on input).
@param filename The input file path for default overwrite.
@param overwrite If true, skips confirmation prompt.
@throws std::runtime_error if user denies overwriting without force.
*/
void verifyOutfile(std::string& outfile, std::string& filename, bool overwrite);

/*
@brief Creates a backup of the input file by copying it to a new file with '.bak' appended.
@param filename The path to the original file.
@throws std::runtime_error if backup file cannot be created or written.
*/
void backupFile(const std::string& filename);

#endif