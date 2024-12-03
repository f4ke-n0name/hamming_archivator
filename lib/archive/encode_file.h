#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <filesystem>
#include <filesystem>
#include <math.h>

const uint8_t kCountControlBits = 4;
const uint8_t kFileSizeBits = 11;
const uint8_t kFileSizeControlBits = 4;
static const uint8_t kCountSizeControlBits = 4; 
static const uint8_t kFileNameBits = 4; 
static const uint8_t kFileNameControlBits = 3;  
static const uint8_t kCountFileNameControlBits = 4;  

const std::string kCopyExt = "copy.haf";

struct HeaderInfo {
  std::string file_name;
  uint64_t size;
  uint8_t count_control_bits;
};

void HammingEncode(std::vector<bool>& bits, uint8_t count_control_bits);

void HammingDecode(std::vector<bool>& bits, uint8_t count_control_bits);

void EncodeHeaderInfo(std::ofstream& out, const std::string& file_name,
                     uint16_t file_size, uint8_t kAdditionalBits);

void EncodeFile(std::ofstream& archive, const std::string& encode_file_name,
                uint8_t control_bits);

HeaderInfo DecodeHeaderInfo(std::ifstream& archive);  

void DecodeFile(std::ifstream& archive, const std::string& decode_file_name,
                const std::string& output);

void DeleteFile(std::filesystem::path archive_name,
                const std::string& delete_file_name, const std::string& output);
