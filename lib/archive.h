#pragma once
#include "encode_file.h"

struct Archive {
  std::string archive_name;
  void AddFileToArchive(const std::string& file_name,
                        uint8_t control_bits_count);
  void DeleteFileFromArchive(const std::string& file_name);
  std::vector<std::string> FileList();
  void AddFilesFromArchive(const std::string& archive);
  uint8_t ExtractFileFromArchive(const std::string& file_name,
                                       const std::string& path);
  void Clear();
};


void Create(const std::string& archive_name,
            const std::vector<std::string>& file_names,
            uint8_t control_bits_count);

void List(const std::string& archive_name);

void Extract(const std::string& archive_name,
             const std::vector<std::string>& file_names);

void Append(const std::string& archive_name,
            const std::vector<std::string>& file_names,
            uint8_t control_bits_count);

void Delete(const std::string& archive_name,
            const std::vector<std::string>& file_names);

void Concatenate(const std::string& archive_name,
                 const std::vector<std::string>& file_names);