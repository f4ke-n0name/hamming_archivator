#pragma once
#include "encode_file.h"
#include <filesystem>


void Create(const std::string& archive_name,
            std::vector<std::string>& files,
            uint8_t control_bits);
void ExtractFile(std::filesystem::path arhive_name,
                          std::vector<std::string> files,
                          const std::string& output);
void Append(std::filesystem::path archive_name, std::vector<std::string> files,
            uint8_t control_bits);
void Delete(std::filesystem::path archive_name,
            std::vector<std::string> delete_list, const std::string& output);
void List(std::filesystem::path name);
void Concatenate(
    std::filesystem::path archive_name,
    std::pair<std::filesystem::path, std::filesystem::path> archives);
