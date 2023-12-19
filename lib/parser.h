#pragma once
#include <string>
#include <vector>
#include <stdint.h>

struct Arguments {
  std::string archive_name;
  std::vector<std::string> file_names;
  bool is_create_argument;
  bool is_list_argument;
  bool is_extract_argument;
  bool is_append_argument;
  bool is_delete_argument;
  bool is_concatenate_argument;
  uint8_t control_bits;
  void Parse(int argc, char* argv[]);
};