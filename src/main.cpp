#include <exception>
#include <filesystem>
#include <string>
#include "../include/CLI11.hpp"
#include "../include/constants.hpp"
#include "../include/functions.hpp"

int main(int argc, char* argv[]) {

  try {
    // Initialize CLI11 app with description (used for option flags)
    CLI::App app{Constants::appName +
                 "\nA Simple XOR Encryption TOOL by @Tuuxy."};

    // Variables for CLI options
    std::string filename;
    std::string xorkey;
    std::string outfile;

    bool overwrite{false};
    bool backup{false};
    bool generate{false};
    bool keyLog{false};

    // Define CLI options and flags with descriptions, required flags set appropriately
    app.add_option(Constants::keyFlag, xorkey, Constants::keyFlagDescription)
        ->required(false);
    app.add_flag(Constants::generateFlag, generate,
                 Constants::generateFlagDescription)
        ->required(false);
    app.add_option(Constants::fileFlag, filename,
                   Constants::fileFlagDescription)
        ->required(true);
    app.add_option(Constants::outFlag, outfile, Constants::outFlagDescription)
        ->required(false);
    app.add_flag(Constants::overwriteFlag, overwrite,
                 Constants::overwriteFlagDescription)
        ->required(false);
    app.add_flag(Constants::backupFlag, backup,
                 Constants::backupFlagDescription)
        ->required(false);
    app.add_flag(Constants::logFlag, keyLog, Constants::logFlagDescription)
        ->required(false);

    try {
      app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
      return app.exit(e);
    }

    verifyFile(filename);
    verifyKey(xorkey, generate);
    verifyOutfile(outfile, filename, overwrite);

    if (generate) {
      generateKey(xorkey);
    }

    if (keyLog) {
      logKey(xorkey, filename);
    }

    if (backup) {
      backupFile(filename);
    }

    // Process the file: if output path equals input, use a temp file and rename to avoid data loss
    if (filename == outfile) {
      std::string tempFileName{filename + ".tmp"};
      try {
        processFileInChunks(filename, tempFileName, xorkey);
      } catch (const std::exception& e) {
        std::cerr << "Error during processing: " << e.what() << '\n';

        return 1;
      }

      try {
        std::filesystem::rename(tempFileName, outfile);
      } catch (const std::filesystem::filesystem_error& e) {

        std::cerr << "Error renaming temporary file: " << e.what() << '\n';
        std::cerr << "Temporary file left as: " << tempFileName << '\n';

        return 1;
      }

    } else {
      try {
        processFileInChunks(filename, outfile, xorkey);
      } catch (const std::exception& e) {

        std::cerr << "Error during processing: " << e.what() << '\n';
        return 1;
      }
    }

  } catch (const std::exception& e) {

    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}