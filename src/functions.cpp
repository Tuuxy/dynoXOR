#include "functions.hpp"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <random>
#include <stdexcept>
#include "constants.hpp"

std::string getConfigDir() {
#ifdef compute_win32_argv
  // On windows, try to get LOCALAPPDATA environment variable
  char* localAppData{std::getenv("LOCALAPPDATA")};
  if (localAppData) {
    return std::string(localAppData) + "\\" + Constants::appName;
  } else {
    return ".\\" + Constants::appName + "\\";
  }
#elif __APPLE__
  // On macOS, get the HOME environment variable
  char* home{std::getenv("HOME")};
  if (home) {
    return std::string(home) + "/Library/Application Support/" +
           Constants::appName + "/";
  } else {
    return "./" + Constants::appName + "/";
  }
#else
  // On Linux and others, get HOME environment variable
  char* home{std::getenv("HOME")};
  if (home) {
    return std::string(home) + "/.config/" + Constants::appName + "/";
  } else {
    return "./" + Constants::appName + "/";
  }
#endif
}

void processFileInChunks(const std::string& filename,
                         const std::string& outfile, const std::string& xorkey,
                         size_t chunkSize) {
  // Open input file stream in binary mode for reading
  std::ifstream input(filename, std::ios::binary);

  if (!input) {
    throw std::runtime_error("Failed to open input file.");
  }

  // Open output file stream in binary mode for writing
  std::ofstream output(outfile, std::ios::binary);

  if (!output) {
    throw std::runtime_error("Failed to open output file.");
  }

  // Prepare buffer to hold file chunks
  std::string buffer(chunkSize, '\0');
  // Cache key length to avoid repeated calls in loop
  size_t keyLen{xorkey.size()};

  // Read input file chunk-by-chunk until EOF or error
  while (input) {
    // Read up to chunkSize bytes into buffer
    input.read(&buffer[0], buffer.size());
    // Get number of bytes actually read ( may be less at end )
    std::streamsize bytesRead{input.gcount()};

    if (!bytesRead) {
      break;
    }

    // XOR each byte of the read chunk with the XOR key (cycling through key)
    for (std::streamsize i{0}; i < bytesRead; ++i) {
      buffer[i] ^= xorkey[i % keyLen];
    }

    // Write the XORed chunk to the output file
    output.write(buffer.data(), bytesRead);

    if (!output) {
      throw std::runtime_error("Failed writing to output file.");
    }
  }
}

void verifyFile(const std::string& filename) {
  // Open file in binary mode, starting at end to obtain file size easily
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file.good()) {
    throw std::runtime_error("Input file does not exist or cannot be read: " +
                             filename);
  }

  // Get the size of the file by current position of the read pointer
  std::streamsize size = file.tellg();

  if (!size) {
    throw std::runtime_error("Input file is empty: " + filename);
  }
}

void logKey(std::string& xorkey, std::string& filename) {
  // Get platform-specific configuration directory path
  std::filesystem::path configDir{getConfigDir()};

  if (!std::filesystem::exists(configDir)) {
    try {
      std::filesystem::create_directories(configDir);
    } catch (const std::filesystem::filesystem_error& e) {
      throw std::runtime_error("Failed to create config directory: " +
                               configDir.string() + '\n' + e.what());
    }
  }

  // Construct full path to the log file within config directory
  std::filesystem::path logPath{configDir / Constants::logFileName};

  // Open log file in append mode so keys are added without overwritng existing logs
  std::ofstream log(logPath, std::ios::app);

  if (!log) {
    throw std::runtime_error("Unable to open key log file for writing: " +
                             logPath.string());
  }

  // Write the filename and associated XOR key to the log file
  log << "filename: " << filename << ", key: " << xorkey << '\n';

  std::cout << "Keys logged at: " + getConfigDir() + Constants::logFileName
            << '\n';
}

void generateKey(std::string& xorkey) {
  static const char charset[]{
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "!@#$%^&*()-_=+[]{}|;:,.<>?"};

  // Thread-local random number generator seeded with random_device
  static thread_local std::mt19937 generator(std::random_device{}());
  // Uniform distribution to index the charset array
  std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);

  // Resize output key string to desired generated key size
  xorkey.resize(Constants::generatedKeySize);

  for (size_t i{0}; i < Constants::generatedKeySize; ++i) {
    xorkey[i] = charset[dist(generator)];
  }

  std::cout << "Generated XOR key: \n" << xorkey << '\n';
}

void verifyKey(std::string& xorkey, bool generate) {
  if ((generate && !xorkey.empty()) || (!generate && xorkey.empty())) {
    throw std::runtime_error(
        "Error: you must specify either --key or --generate, but not both.");
  }

  if (!generate) {
    if (xorkey.empty()) {
      throw std::runtime_error("XOR key cannot be empty.");
    }

    if (xorkey.length() < Constants::minimumKeySize) {
      throw std::runtime_error("XOR key too short, please make it at least " +
                               std::to_string(Constants::minimumKeySize) +
                               " characters long (random if possible).");
    }
  }
}

void verifyOutfile(std::string& outfile, std::string& filename,
                   bool overwrite) {
  if (outfile.empty()) {
    if (!overwrite) {
      std::cout
          << "Are you sure you want to overwrite the inputed file ? (y/n) ";
      char confirmation{};
      std::cin >> confirmation;

      if (confirmation != 'y') {
        throw std::runtime_error(
            "Start again using the -o or --outfile option to select an "
            "outfile.");
      }
    }

    outfile = filename;
  }
}

void backupFile(const std::string& filename) {
  // Open the original file in binary mode for reading
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Error opening original file for backup: " +
                             filename);
  }

  std::string backupName{filename + ".bak"};

  // Open backup file in binary mode for writing
  std::ofstream backup(backupName, std::ios::binary);
  if (!backup) {
    throw std::runtime_error("Error opening backup file for writing: " +
                             backupName);
  }

  // Copy the contents of the original file into the backup file
  backup << file.rdbuf();

  if (!backup) {
    throw std::runtime_error("Error writing backup file: " + backupName);
  }

  std::cout << "Backup created: " << backupName << '\n';
}