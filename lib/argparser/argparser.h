#pragma once
#include <string>
#include <vector>
#include <stdint.h>

class Arguments {
 private:
  std::string archive_name;
  std::vector<std::string> file_names;
  std::string output;
  bool is_create_argument;
  bool is_list_argument;
  bool is_extract_argument;
  bool is_append_argument;
  bool is_delete_argument;
  std::pair<std::string, std::string> concatenate_argument;
  uint8_t control_bits;

 public:
  Arguments();
  void Parse(int argc, char* argv[]);
  std::string GetArchiveName();
  std::string GetOutput();
  bool GetCreate();
  bool GetExtract();
  bool GetAppend();
  bool GetDelete();
  uint8_t GetControlBits();
  bool GetList();
  std::vector<std::string> GetListing();
  std::pair<std::string, std::string> GetConcatenate();
};
