#include "encode_file.h"

bool SetCalculatedBit(uint8_t index, uint8_t step, std::vector<bool>& bits) {
  bool bit = 0;
  uint16_t n = bits.size();
  for (size_t i = index; i < n; i += (1 << step)) {
    for (size_t j = i; (j < n) && (j < (1 << step) - 1 + i); ++j) {
      bit = bit ^ bits[j];
    }
  }
  return bit;
}

uint64_t ConvertBitsToNumber(std::vector<bool>& to_number) {
  uint64_t number = 0;
  for (size_t i = 0; i < to_number.size() - 1; ++i) {
    number |= (to_number[i] << i);
  }
  return number;
}
std::vector<bool> ConvertNumberToBits(uint64_t number, uint8_t control_bits) {
  uint64_t size = ((1 << control_bits) - control_bits - 1);
  std::vector<bool> result;
  for (uint64_t i = 0; i < size; ++i) {
    result.push_back((number & (1 << i)) >> i);
  }
  return result;
}

void HammingEncode(std::vector<bool>& bits, uint8_t count_control_bits) {
  for (size_t i = 0; i < count_control_bits; ++i) {
    bits.insert(bits.begin() + ((1 << i) - 1), 0);
  }
  for (size_t i = 0; i < count_control_bits; ++i) {
    bits[(1 << i) - 1] = SetCalculatedBit((1 << i) - 1, i + 1, bits);
  }
  bool last_bit = 0;
  for (size_t i = 0; i < bits.size(); ++i) {
    last_bit = last_bit ^ bits[i];
  }
  bits.push_back(last_bit);
}

void HammingDecode(std::vector<bool>& bits, uint8_t count_control_bits) {
  bool last_bit = bits.back();
  bits.pop_back();
  std::vector<bool> control_bits;
  uint8_t error_bit = 0;
  std::vector<bool> decoded_bits;
  for (size_t i = 0; i < count_control_bits; ++i) {
    control_bits.push_back(SetCalculatedBit((1 << i) - 1, i + 1, bits));
  }
  error_bit = ConvertBitsToNumber(control_bits);
  if (last_bit && !error_bit) {
    std::cerr << "Finded double error!\n";
    exit(EXIT_FAILURE);
  } else if (!last_bit && error_bit) {
    bits[error_bit - 1] = !bits[error_bit - 1];
  }
  uint16_t current_pow = 1;
  for (size_t i = 0; i < bits.size(); ++i) {
    if (i == current_pow - 1) {
      current_pow *= 2;
      continue;
    }
    decoded_bits.push_back(bits[i]);
  }
  bits = decoded_bits;
}

void EncodeBlock(std::vector<bool>& code, uint8_t block_size,
                 uint8_t control_bits, std::ofstream& archive) {
  if (code.size() == block_size) {
    HammingEncode(code, control_bits);
    for (size_t i = 0; i < code.size() / 8; ++i) {
      uint8_t byte = 0;
      for (uint8_t j = 0; j < CHAR_BIT; ++j) {
        byte |= code[i * CHAR_BIT + j] << j;
      }
      archive << byte;
    }
    code.clear();
  }
}

std::string GetFileName(const std::string& file) {
  return file.substr(file.find_last_of("/\\") + 1);
}

void EncodeHeaderInfo(std::ofstream& archive, const std::string& file_name,
                      uint16_t size, uint8_t control_bits) {
  std::vector<bool> code;
  std::string name = GetFileName(file_name);

  uint8_t block_size =
      (1 << kCountFileNameControlBits) - kCountFileNameControlBits - 1;
  uint8_t count_name = name.size() * CHAR_BIT / kFileNameBits;
  code = ConvertNumberToBits(count_name, kCountFileNameControlBits);
  EncodeBlock(code, block_size, kCountFileNameControlBits, archive);

  block_size = kFileNameBits;
  for (size_t i = 0; i < name.size(); ++i) {
    for (size_t j = 0; j < CHAR_BIT; ++j) {
      code.push_back((name[i] & (1 << j)) >> j);
      EncodeBlock(code, block_size, kFileNameControlBits, archive);
    }
  }
  code.clear();

  code = ConvertNumberToBits(control_bits, kCountControlBits);
  block_size = (1 << kCountControlBits) - kCountControlBits - 1;
  EncodeBlock(code, block_size, kCountControlBits, archive);

  uint8_t count_file_size =
      (((uint8_t)(std::ceil(std::log2(size))) % kFileSizeBits)) == 0
          ? (uint8_t)std::ceil(std::log2(size))
          : ((uint8_t)std::ceil(std::log2(size)) +
             (kFileSizeBits -
              ((uint8_t)(std::ceil(std::log2(size))) % kFileSizeBits))) /
                kFileSizeBits;
  block_size = (1 << kCountSizeControlBits) - kCountSizeControlBits - 1;
  code = ConvertNumberToBits(count_file_size, kCountSizeControlBits);
  EncodeBlock(code, block_size, kCountSizeControlBits, archive);

  block_size = kFileSizeBits;
  std::vector<bool> size_code;
  for (size_t i = 0; i < count_file_size; ++i) {
    for (size_t j = i * kFileSizeBits; j < i * kFileSizeBits + kFileSizeBits; ++j) {
      code.push_back((size & (1 << j)) >> j);
      EncodeBlock(code, block_size, kFileSizeControlBits, archive);
    }
  }
  code.clear();
}

void EncodeFile(std::ofstream& archive, const std::string& encode_file_name,
                uint8_t control_bits) {
  std::ifstream input;

  uint8_t block_size =  1 << control_bits;
  std::filesystem::path file_path{encode_file_name};
  uint64_t size = std::filesystem::file_size(file_path) * CHAR_BIT;
  block_size -= control_bits + 1;
  size = size + (size % block_size);
  size += (size / block_size) * (control_bits + 1);

  EncodeHeaderInfo(archive, encode_file_name, size, control_bits);
  std::vector<bool> code;
  char byte = 0;
  while (input.get(byte)) {
    for (int8_t i = 0; i < CHAR_BIT; ++i) {
      code.push_back((byte & (1 << i) >> i));
      EncodeBlock(code, block_size, control_bits, archive);
    }
  }
}

HeaderInfo DecodeHeaderInfo(std::ifstream& archive) {
  HeaderInfo info;
  uint16_t block_count;
  uint8_t block_size;
  std::vector<bool> code;
  char byte;
  block_size = ((1 << kCountFileNameControlBits));
  std::vector<bool> count_name_code;
  for (size_t i = 0; i < block_size / CHAR_BIT; ++i) {
    archive.get(byte);
    for (size_t j = 0; j < CHAR_BIT; ++j) {
      code.push_back((byte & (1 << j)) >> j);
      if (code.size() % block_size == 0) {
        HammingDecode(code, kCountControlBits);
        count_name_code.insert(std::end(count_name_code), std::begin(code),
                               std::end(code));
        code.clear();
      }
    }
  }
  block_size = ConvertBitsToNumber(count_name_code);
  code.clear();
  std::vector<bool> name_bits;
  for (size_t i = 0; i < block_size; ++i) {
    archive >> byte;
    for (size_t j = 0; j < CHAR_BIT; ++j) {
      code.push_back((byte & (1 << j)) >> j);
      if (code.size() % (1 << kFileNameControlBits) == 0) {
        HammingDecode(code, kFileNameControlBits);
        name_bits.insert(std::end(name_bits), std::begin(code), std::end(code));
        code.clear();
      }
    }
  }
  for (size_t i = 0; i < name_bits.size() / CHAR_BIT; ++i) {
    char letter = 0;
    for (size_t j = 0; j < CHAR_BIT; ++j) {
      letter |= name_bits[i * 8 + j] << j;
    }
    info.file_name += letter;
  }
  code.clear();

  block_size = ((1 << kCountControlBits));
  std::vector<bool> additional_bits_code;
  for (uint8_t i = 0; i < block_size / CHAR_BIT; ++i) {
    archive >> byte;
    for (uint8_t j = 0; j < CHAR_BIT; j++) {
      code.push_back((byte & (1 << j)) >> j);
      if (code.size() % block_size == 0) {
        HammingDecode(code, kCountControlBits);
        additional_bits_code.insert(std::end(additional_bits_code),
                                    std::begin(code), std::end(code));
        code.clear();
      }
    }
  }
  info.count_control_bits = ConvertBitsToNumber(additional_bits_code);
  code.clear();
  std::vector<bool> block_size_code;
  block_size = (1 << kCountSizeControlBits);
  for (uint8_t i = 0; i < block_size / CHAR_BIT; ++i) {
    archive >> byte;
    for (uint8_t j = 0; j < CHAR_BIT; ++j) {
      code.push_back((byte & (1 << j)) >> j);
      if (code.size() % block_size == 0) {
        HammingDecode(code, kCountSizeControlBits);
        block_size_code.insert(std::end(block_size_code), std::begin(code),
                               std::end(code));
        code.clear();
      }
    }
  }
  block_size =
      ConvertBitsToNumber(block_size_code) * ((1 << kFileSizeControlBits) / CHAR_BIT);
  code.clear();
  std::vector<bool> size_code;
  for (uint8_t i = 0; i < block_size; ++i) {
    archive >> byte;
    for (uint64_t j = 0; j < CHAR_BIT; ++j) {
      code.push_back((byte & (1 << j)) >> j);
      if (code.size() % (block_size * CHAR_BIT) == 0) {
        HammingDecode(code, kFileSizeControlBits);
        size_code.insert(std::begin(size_code), std::begin(code),
                         std::end(code));
        code.clear();
      }
    }
  }
  code.clear();
  info.size = ConvertBitsToNumber(size_code);
  // Раскодировали рамзер файла
  return info;
}

void DecodeFile(std::ifstream& archive, const std::string& to_decode_file_name,
                const std::string& output) {
  HeaderInfo info;
  info = DecodeHeaderInfo(archive);
  while (info.file_name != to_decode_file_name && archive.good()) {
    std::streampos next(info.size);
    archive.seekg(archive.tellg() + next);
    info = DecodeHeaderInfo(archive);
  }
  if (info.file_name != to_decode_file_name) {
    std::cerr << "The file was not found!\n";
    exit(EXIT_FAILURE);
  }
  std::ofstream out(output + info.file_name, std::ios::binary);
  std::vector<bool> code;
  std::vector<bool> bits;
  uint64_t block_size = (1 << info.count_control_bits);
  char byte = 0;
  for (size_t i = 0; i < info.size / CHAR_BIT; ++i) {
    archive.get(byte);
    for (size_t j = 0; j < CHAR_BIT; ++j) {
      code.push_back((byte & (1 << j)) >> j);
      if (i % block_size == 0) {
        HammingDecode(code, info.count_control_bits);
        bits.insert(std::end(bits), std::begin(code), std::end(code));
        if (bits.size() == CHAR_BIT) {
          out << (char)ConvertBitsToNumber(bits);
          bits.clear();
        }
        code.clear();
      }
    }
  }
}

void DeleteFile(std::filesystem::path archive_name,
            const std::string& delete_file_name,
            const std::string& output) {
  std::ofstream copy_archive;
  std::ifstream archive(archive_name, std::ios::binary);
  copy_archive.open(output + kCopyExt, std::ios::binary);
  HeaderInfo info;
  std::streampos start = 0;
  std::streampos end = UINT16_MAX;
  while (archive.tellg() != end) {
    info = DecodeHeaderInfo(archive);
    if (info.file_name == delete_file_name) {
      archive.seekg(start + archive.tellg() +
                    std::streampos((info.size / CHAR_BIT)));
    } else {
      EncodeHeaderInfo(copy_archive, info.file_name, info.size,
                       info.count_control_bits);
      end = archive.tellg() + std::streampos(info.size / CHAR_BIT);
      char byte = 0;
      for (uint32_t i = 0; i < info.size / CHAR_BIT; ++i) {
        archive.get(byte);
        copy_archive << byte;
      }
      start = archive.tellg();
    }
  }
  copy_archive.close();
  archive.close();
  std::filesystem::remove(archive_name);
  std::filesystem::rename(std::filesystem::path(output + kCopyExt),
                          archive_name);
}


