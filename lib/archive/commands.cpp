#include "commands.h"


void Create(const std::string& archive_name, std::vector<std::string>& files,
            uint8_t control_bits) {
  std::filesystem::path output_path = archive_name;
  std::fstream archive(output_path);
  archive.close();
  Append(output_path, files, control_bits);
}

void ExtractFile(std::filesystem::path archive_name,
                 std::vector<std::string> files,
                 const std::string& output) {
  std::ifstream archive(archive_name, std::ios::binary);
  for (size_t i = 0; i < files.size(); ++i) {
    DecodeFile(archive, files[i], output);
  }
  archive.close();
}

void Append(std::filesystem::path archive_name, std::vector<std::string> files, uint8_t control_bits) {
  std::ofstream archive(archive_name, std::ios::ate | std::ios::binary);
  for (size_t i = 0; i < files.size(); ++i) {
    EncodeFile(archive, files[i], control_bits);
  }
  archive.close();
}

void Delete(std::filesystem::path archive_name,
  std::vector<std::string> delete_list, const std::string& output) {
  for (size_t i = 0; i < delete_list.size(); ++i) {
    DeleteFile(archive_name, delete_list[i], output);
  }
}


void List(std::filesystem::path name) {
  std::ifstream archive(name, std::ios::binary);
  HeaderInfo info;
  info = DecodeHeaderInfo(archive);
  std::cout << info.file_name << '\n';
  while (archive.good()) {
    std::streampos next(info.size);
    archive.seekg(archive.tellg() + next);
    info = DecodeHeaderInfo(archive);
    std::cout << info.file_name << '\n';
  }
  archive.close();
}

void Concatenate(std::filesystem::path archive_name,
                 std::pair<std::filesystem::path, std::filesystem::path> archives) {
  std::ofstream archive(archive_name, std::ios::binary);
  std::ifstream first(archives.first, std::ios::binary);
  std::ifstream second(archives.second, std::ios::binary);
  char byte;
  while (first.get(byte)) {
    archive << byte;
  }
  first.close();
  while (second.get(byte)) {
    archive << byte;
  }
  second.close();
  archive.close();
}
