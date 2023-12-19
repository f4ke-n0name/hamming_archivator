#include "archive.h"

const uint8_t kHeaderSize = 8;
const uint8_t kEncodedHeaderSize = kHeaderSize * 2;
const uint8_t kNameHeaderSize = 8;
const uint8_t kEncodedNameHeaderSize = kNameHeaderSize * 2;

void Archive::AddFileToArchive(const std::string& file_name,
                               uint8_t control_bits_count) {}

void Archive::DeleteFileFromArchive(const std::string& file_name) {}

std::vector<std::string> Archive::FileList() {
  return std::vector<std::string>();
}

void Archive::AddFilesFromArchive(const std::string& arch) {}

unsigned char Archive::ExtractFileFromArchive(const std::string& file_name,
                                              const std::string& path) {
  return 0;
}

void Archive::Clear() {}

std::string GetFileName(const std::string_view& path) {
  std::string file_name{path};
  for (size_t i = path.size() - 1; i >= 0; i--) {
    if (path[i] == '\\') {
      file_name = path.substr(i + 1, path.size() - i - 1);
      return file_name;
    }
  }
  return file_name;
}

std::string GetFileNameWithoutExtension(const std::string_view& file) {
  std::string file_name;
  file_name = file.substr(0, file.find('.'));
  return file_name;
}

Archive ArchiveOpen(const std::string& archive_name) {
  Archive archive;
  archive.archive_name = GetFileNameWithoutExtension(archive_name) + ".haf";
  return archive;
}

void CountHeaderFill(uint8_t *count_header, uint64_t bytes_count_encoded) {
  for (size_t i = 0; i < kHeaderSize; i++) {
    count_header[i] =
        (bytes_count_encoded >> ((kHeaderSize - i - 1) * CHAR_BIT));
  }
}

void AddCountHeaderToArchive(std::ofstream& archive, uint64_t bytes_count) {
  uint8_t current_byte;
  uint64_t bytes_count_encoded = bytes_count;
  uint8_t count_header[kHeaderSize];
  CountHeaderFill(count_header, bytes_count_encoded);
  for (size_t i = 0; i < kHeaderSize; ++i) {
    current_byte = count_header[i];
    AddEncodedByteToArchive(current_byte, archive);
  }
}

void AddNameHeaderToArchive(std::ofstream& archive, const std::string& path) {
  uint8_t current_byte;
  std::string file_name = GetFileName(path);
  uint8_t name[kNameHeaderSize];
  for (size_t i = 0; i < kNameHeaderSize; ++i) {
    if (i < file_name.size()) {
      name[i] = file_name[i];
    } else {
      name[i] = '\0';
    }
  }
  for (size_t i = 0; i < kNameHeaderSize; ++i) {
    current_byte = name[i];
    AddEncodedByteToArchive(current_byte, archive);
  }
}

void AddBitsHeaderToArchive(std::ofstream& archive,
                             uint8_t control_bits_count) {
  AddEncodedByteToArchive(control_bits_count, archive);
}
