#include "argparser.h"

const char* create_string = "--create"; 
const char* list_string = "--list";
const char* extract_string = "--extract";
const char* append_string = "--append";
const char* delete_string = "--delete";
const char* concatenate_string = "--concatenate";
const char* output_string = "--output";
const char* file_string = "--file=";
const char* bits_string = "--bits=";

Arguments::Arguments() { archive_name = "";
  output = "D:\\test\\";
  is_create_argument = false;
  is_list_argument = false;
  is_extract_argument = false;
  is_append_argument = false;
  is_delete_argument = false;
  concatenate_argument = std::pair<std::string, std::string>();
  control_bits = 3;
}

void Arguments::Parse(int argc, char* argv[]) {
  bool flag = true;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-c" || arg == create_string) {
      is_create_argument = true;
      flag = false;
    } else if ((arg == "-l" || arg == list_string) && flag) {
      is_list_argument = true;
      flag = false;
    } else if ((arg == "-x" || arg == extract_string) && flag) {
      is_extract_argument = true;
      flag = false;
    } else if ((arg == "-a" || arg == append_string) && flag) {
      is_append_argument = true;
      flag = false;
    } else if ((arg == "-d" || arg == delete_string) && flag) {
      is_delete_argument = true;
      flag = false;
    } else if (arg == "-f") {
      ++i;
      archive_name = argv[i];
    } else if (arg.substr(0, arg.find_first_of('=') + 1) == file_string) {
      archive_name = arg.substr(arg.find_first_of('=') + 1);
    } else if ((arg == "-o" || arg == output_string) && flag) {
      ++i;
      output = argv[i];
    } else if ((arg == "-A" || arg == concatenate_string) && flag) {
      ++i;
      concatenate_argument.first = argv[i];
      ++i;
      concatenate_argument.second = argv[i];
    } else if (arg == "-b" || arg == bits_string) {
      ++i;
      control_bits = std::stoi(argv[i]);
    } else {
      file_names.push_back(arg);
    }
  }
}

std::string Arguments::GetArchiveName() { return archive_name; }

std::string Arguments::GetOutput() { return output; }

bool Arguments::GetCreate() { return is_create_argument; }

bool Arguments::GetExtract() { return is_extract_argument; }

bool Arguments::GetAppend() { return is_append_argument; }

bool Arguments::GetDelete() { return is_delete_argument; }

uint8_t Arguments::GetControlBits() { return control_bits; }

bool Arguments::GetList() { return is_list_argument; }

std::vector<std::string> Arguments::GetListing() { return file_names; }

std::pair<std::string, std::string> Arguments::GetConcatenate() {
  return concatenate_argument;
}
