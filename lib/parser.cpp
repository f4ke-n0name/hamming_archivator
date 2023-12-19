#include "parser.h"

const char* create_string = "--create"; 
const char* list_string = "--list";
const char* extract_string = "--extract";
const char* append_string = "--append";
const char* delete_string = "--delete";
const char* concatenate_string = "--concatenate";
const char* file_string = "--file=";
const char* bits_string = "--bits=";

void Arguments::Parse(int argc, char* argv[]) {
  bool is_archive_name = false;
  bool is_bits = false;
  for (int i = 1; i < argc; ++i) {
    if (is_archive_name) {
      is_archive_name = false;
      archive_name = argv[i];
    } else if (is_bits) {
      is_bits = false;
      control_bits = std::stoi(argv[i]);
    } else if (argv[i][0] == '-') {
      if (std::strcmp(argv[i], create_string) == 0 ||
          std::strcmp(argv[i], "-c") == 0) {
        is_create_argument = true;
      } else if (std::strcmp(argv[i], list_string) == 0 ||
                 std::strcmp(argv[i], "-l") == 0) {
        is_list_argument = true;
      } else if (std::strcmp(argv[i], extract_string) == 0 ||
                 std::strcmp(argv[i], "-x") == 0) {
        is_extract_argument = true;
      } else if (std::strcmp(argv[i], append_string) == 0 ||
                 std::strcmp(argv[i], "-a") == 0) {
        is_append_argument = true;
      } else if (std::strcmp(argv[i], delete_string) == 0 ||
                 std::strcmp(argv[i], "-d") == 0) {
        is_delete_argument = true;
      } else if (std::strcmp(argv[i], concatenate_string) == 0 ||
                 std::strcmp(argv[i], "-A") == 0) {
        is_concatenate_argument = true;
      } else if (std::strcmp(argv[i], "-f") == 0) {
        is_archive_name = true;
      } else if (std::strcmp(argv[i], "-b") == 0) {
        is_bits = true;
      } else {
        std::string argument = argv[i];
        std::string type = argument.substr(0, 7);
        if (type == file_string) {
          archive_name = argument.substr(7, argument.size() - 7);
        } else if (type == bits_string) {
          control_bits = std::stoi(argument.substr(7, argument.size() - 7));
        }
      }
    } else {
      file_names.push_back(argv[i]);
    }
  }
}
