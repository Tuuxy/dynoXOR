#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <stdexcept>
#include "../externals/Catch2/src/catch2/catch_test_macros.hpp"
#include "../include/constants.hpp"
#include "../include/functions.hpp"

// Helper function that creates temporary test file
void createTestFile(const std::string& filename, const std::string& content) {
  std::ofstream file(filename, std::ios::binary);
  file << content;
}

// Helper function that reads file content
std::string readTestFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      (std::istreambuf_iterator<char>()));

  return content;
}

// Helper function that cleans up test files
void cleanupTestFile(const std::string& filename) {
  if (std::filesystem::exists(filename)) {
    std::filesystem::remove(filename);
  }
}

// TEST: getConfigDir()

TEST_CASE("getConfigDir returns valid path", "[config]") {
  std::string configDir{getConfigDir()};

  SECTION("Config directory path is not empty") {
    REQUIRE_FALSE(configDir.empty());
  }

  SECTION("Config directory contains app name") {
    REQUIRE(configDir.find(Constants::appName) != std::string::npos);
  }

#ifdef __linux__
  SECTION("Linux path contains .config") {
    REQUIRE(configDir.find(".config") != std::string::npos);
  }
#elif __APPLE__
  SECTION("macOS path contains Library/Application Support") {
    REQUIRE(configDir.find("Library/Application Support") != std::string::npos);
  }
#endif
}

// TEST: verifyFile()

TEST_CASE("verifyFile() validates file existence and content",
          "[file],[verify]") {
  const std::string testFile{"test_verify.txt"};

  SECTION("Throws exception for non-existent file") {
    REQUIRE_THROWS_AS(verifyFile("nonexistent_file.txt"), std::runtime_error);
  }

  SECTION("Throws exception for empty file") {
    createTestFile(testFile, "");
    REQUIRE_THROWS_AS(verifyFile(testFile), std::runtime_error);
    cleanupTestFile(testFile);
  }

  SECTION("Succeeds for valid non-empty files") {
    createTestFile(testFile, "Some content");
    REQUIRE_NOTHROW(verifyFile(testFile));
    cleanupTestFile(testFile);
  }
}

// TEST: generateKey()

TEST_CASE("generateKey creates valid random keys", "[key][generation]") {
  std::string key1{};
  std::string key2{};

  SECTION("Generated key has correct length") {
    generateKey(key1);
    REQUIRE(key1.length() == Constants::generatedKeySize);
  }

  SECTION("Generated keys are different") {
    generateKey(key1);
    generateKey(key2);
    REQUIRE(key1 != key2);
  }

  SECTION("Generated key contains valid characters") {
    generateKey(key1);
    bool allValid{true};

    for (char c : key1) {
      bool isValidChar = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
                         (c >= 'a' && c <= 'z') ||
                         std::string("!@#$%^&*()-_=+[]{}|;:,.<>?").find(c) !=
                             std::string::npos;

      if (!isValidChar) {
        allValid = false;
        break;
      }
    }

    REQUIRE(allValid);
  }
}

// TEST: verifyKey()

TEST_CASE("verifyKey validates key constraints", "[key],[validation]") {
  std::string key{};

  SECTION("Throws when both key and generate flag are provided") {
    key = "this_is_a_very_long_and_secure_key_with_enough_characters";
    REQUIRE_THROWS_AS(verifyKey(key, true), std::runtime_error);
  }

  SECTION("Throws when neither key nor generate flag provided") {
    key = "";
    REQUIRE_THROWS_AS(verifyKey(key, false), std::runtime_error);
  }

  SECTION("Throws when key is too short") {
    key = "short";
    REQUIRE_THROWS_AS(verifyKey(key, false), std::runtime_error);
  }

  SECTION("Succeeds with valid long key") {
    key = "this_is_a_very_long_and_secure_key_with_enough_characters";
    REQUIRE_NOTHROW(verifyKey(key, false));
  }
}

// TEST: verifyOutFile()

TEST_CASE("verifyOutFile handles output file logic", "[file][outfile]") {
  std::string infile{"input.txt"};
  std::string outfile{};

  SECTION("Empty outfile with overwrite=true sets outfile to infile") {
    outfile = "";
    REQUIRE_NOTHROW(verifyOutfile(outfile, infile, true));
    REQUIRE(outfile == infile);
  }

  SECTION("Non-empty outfile remains unchanged") {
    outfile = "output.txt";
    std::string original{outfile};
    verifyOutfile(outfile, infile, true);
    REQUIRE(outfile == original);
  }
}

// TEST: backupFile()

TEST_CASE("backupFile creates backup copies", "[file][backup]") {
  const std::string testFile{"test_backup.txt"};
  const std::string backedUpFile{testFile + ".bak"};
  const std::string content{"Important data to backup"};

  SECTION("Throw exception for non-existent file") {
    REQUIRE_THROWS_AS(backupFile("non_existent.txt"), std::runtime_error);
  }

  SECTION("Creates backup with same content") {
    createTestFile(testFile, content);

    REQUIRE_NOTHROW(backupFile(testFile));
    REQUIRE(std::filesystem::exists(backedUpFile));

    std::string originalContent{readTestFile(testFile)};
    std::string backupContent{readTestFile(backedUpFile)};

    REQUIRE(originalContent == backupContent);

    cleanupTestFile(testFile);
    cleanupTestFile(backedUpFile);
  }
}

// TEST: processFileInChunks

TEST_CASE("processFileInChunks performs XOR encryption", "[xor][processing]") {
  const std::string inputFile{"test_input.bin"};
  const std::string outputFile{"test_output.bin"};
  const std::string key{"SecretKey123456789"};
  const std::string testData{"Hello world! This is some test data."};

  SECTION("Throw exception for non-existent input file") {
    REQUIRE_THROWS_AS(
        processFileInChunks("non_existent.txt", outputFile, key, 1024),
        std::runtime_error);
  }

  SECTION("XOR encryption and decryption are reversible ?") {
    createTestFile(inputFile, testData);

    // Encrypt
    REQUIRE_NOTHROW(processFileInChunks(inputFile, outputFile, key, 1024));

    std::string encryptedData{readTestFile(outputFile)};
    REQUIRE(encryptedData != testData);

    // Decrypt (XOR again with same key)
    std::string decryptedFile{"test_decrypted.bin"};
    REQUIRE_NOTHROW(processFileInChunks(outputFile, decryptedFile, key, 1024));

    std::string decryptedData{readTestFile(decryptedFile)};
    REQUIRE(decryptedData == testData);

    cleanupTestFile(inputFile);
    cleanupTestFile(outputFile);
    cleanupTestFile(decryptedFile);
  }

  SECTION("Works with small chunk sizes") {
    createTestFile(inputFile, testData);

    // Process with very small chunks ( 5 bytes at a time )
    REQUIRE_NOTHROW(processFileInChunks(inputFile, outputFile, key, 5));

    std::string decryptedFile{"test_small_chunks.bin"};
    processFileInChunks(outputFile, decryptedFile, key, 5);

    std::string decryptedData{readTestFile(decryptedFile)};
    REQUIRE(decryptedData == testData);

    cleanupTestFile(inputFile);
    cleanupTestFile(outputFile);
    cleanupTestFile(decryptedFile);
  }

  SECTION("Handles binary data correctly") {
    // Create binary test data
    std::string binaryData;

    for (int i{0}; i < 256; ++i) {
      binaryData += static_cast<char>(i);
    }

    createTestFile(inputFile, binaryData);
    processFileInChunks(inputFile, outputFile, key, 1024);

    // Decrypt
    std::string decryptedFile{"test_binary.bin"};
    processFileInChunks(outputFile, decryptedFile, key, 1024);

    std::string decryptedData{readTestFile(decryptedFile)};
    REQUIRE(decryptedData == binaryData);

    cleanupTestFile(inputFile);
    cleanupTestFile(outputFile);
    cleanupTestFile(decryptedFile);
  }
}

// TEST: logKey()

TEST_CASE("logKey writes keys to log file", "[logging]") {
  std::string testKey{"TestKey12345!@#$%"};
  std::string testFile{"test_file.txt"};

  SECTION("Creates log file and logs key") {
    REQUIRE_NOTHROW(logKey(testKey, testFile));

    std::string configDir{getConfigDir()};
    std::filesystem::path logPath{std::filesystem::path(configDir) /
                                  Constants::logFileName};

    // Verify log file exists
    REQUIRE(std::filesystem::exists(logPath));

    // Read log file content
    std::string logContent{readTestFile(logPath.string())};

    // Verify log contains filename and key
    REQUIRE(logContent.find(testFile) != std::string::npos);
    REQUIRE(logContent.find(testKey) != std::string::npos);
  }
}
