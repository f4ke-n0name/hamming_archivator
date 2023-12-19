#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <parser.h>

struct File {
  std::string file_name;
  uint64_t file_total_bytes = 0;
  uint8_t count_control_bits = 3;
  File() = default;
  File(const std::string& s);
  File(const std::string& s, uint8_t count_bits);
};

uint64_t CountFileBytes(std::ifstream& input);

void HammingEncode(std::vector<bool>& bits);

bool IsHammingDecode(std::vector<bool>& bits);

void AddEncodedByteToArchive(uint8_t current_byte, std::ofstream& archive_name);

void AddEncodeHammingToFile(const File& input, std::ofstream& output);