#include "encode_file.h"

uint8_t kBits[2] = {0, 4};
uint8_t kDefaultControlBits = 3;

File::File(const std::string& s) {
  file_name = s;
  std::ifstream input;
  input.open(s);
  file_total_bytes = CountFileBytes(input);
}

File::File(const std::string& s, uint8_t count_bits) {
  file_name = s;
  count_control_bits = count_bits;
}

uint64_t CountFileBytes(std::ifstream& input) { 
  uint64_t res = 0;
  if (!input.eof()) {
     
  }
}


void HammingEncode(std::vector<bool>& bits) {
  size_t size = bits.size();
  for (size_t control_bit = 1; control_bit < size - 1; control_bit *= 2) {
    size_t i = control_bit - 1;
    bool current_control_bit = 0;
    while (i < size) {
      for (size_t j = i; (j < control_bit + i) && (j < size - 1); ++j) {
        if (j == control_bit - 1) {
          continue;
        }
        current_control_bit = (current_control_bit != bits[j]);
      }
      i += 2 * control_bit;
    }
    bits[control_bit - 1] = current_control_bit;
  }
  bool xor_bit = 0;
  for (size_t i = 0; i < size - 1; ++i) {
    xor_bit = (xor_bit != bits[i]);
  }
  bits[size - 1] = xor_bit;
}

bool IsHammingDecode(std::vector<bool>& bits) {
  std::vector<bool> counted_bits;
  size_t size = bits.size();
  for (size_t control_bit = 1; control_bit < size - 1; control_bit *= 2) {
    size_t i = control_bit - 1;
    bool current_control_bit = 0;
    while (i < size) {
      for (size_t j = i; (j < control_bit + i) && (j < size - 1); ++j) {
        if (j == control_bit - 1) {
          continue;
        }
        current_control_bit = (current_control_bit != bits[j]);
      }
      i += 2 * control_bit;
    }
    counted_bits.push_back(current_control_bit);
  }
  uint8_t bit_error = 0;
  size_t j = 1;
  for (size_t i = 0; i < size; ++i) {
    if (bits[i] != counted_bits[j - 1]) {
      bit_error += j;
    }
    j *= 2;
  }
  if (bit_error == 0) {
    return true;
  }
  bool xor_all = 0;
  for(size_t i = 0; i < size; ++i) {
    xor_all = (xor_all != bits[i]);
  }
  if (xor_all) {
    bits[bit_error - 1] = !bits[bit_error - 1];
    return true;
  } else {
    return false;
  }
}

uint64_t binary_pow(uint8_t base, uint16_t degree) { 
  uint64_t res = 1;
  while (degree) {
    if (degree & 1) {
      res *= base;
    }
    base *= base;
    degree >>= 1; 
  }
  return res;
}
void AddEncodedByteToArchive(uint8_t current_byte, std::ofstream& archive_stream) {
  bool curent_code[CHAR_BIT];
  std::vector<bool> ham_code(CHAR_BIT, 0);
  size_t control_bit;
  for (size_t i = 0; i < CHAR_BIT; ++i) {
    curent_code[CHAR_BIT - 1 - i] = (current_byte >> 1) & 1;
  }
  for (uint16_t count_bits : kBits) {
    control_bit = 1;
    for (size_t i = 0; i < CHAR_BIT; ++i) {
      if (control_bit - 1 == 0) {
        control_bit *= 2;
      } else {
        ham_code[i] = curent_code[count_bits];
        ++count_bits;
      }
    }
    HammingEncode(ham_code);
    current_byte = 0;
    for (size_t i = 0; i < CHAR_BIT; ++i) {
      if (ham_code[i] == 1) {
        current_byte += static_cast<uint8_t>(binary_pow(2, CHAR_BIT - 1 - i));
      }
    }
    archive_stream << current_byte;
  }
}

void AddEncodeHammingToFile(const File& input, std::ofstream& output) {
  std::ifstream input_stream;
  input_stream.open(input.file_name, std::ios::binary);
  uint8_t current_byte;
  if (input.count_control_bits == kDefaultControlBits) {
    for (size_t i = 0; i < input.file_total_bytes; ++i) {
      current_byte = input_stream.get();
      AddEncodeByteToArchive(current_byte, output);
    }
  } else if (input.count_control_bits > kDefaultControlBits) {
    uint64_t block_size = binary_pow(2, input.count_control_bits);
    uint64_t info_bits_count = block_size - input.count_control_bits - 1;
    std::vector<bool> cur_ham_code(block_size, 0);
    std::vector<bool> next_ham_bits(CHAR_BIT, 0);
    uint64_t current_count_bytes = 0;
    uint64_t control_bit = 0;
    uint64_t prev_bits = 0;
    uint64_t current_bit = 0;
    uint64_t add_bytes = 0;
    uint64_t count_next_bits = 0;
    std::vector<bool> current_bits(CHAR_BIT, 0);
    while ((current_count_bytes < input.file_total_bytes || (prev_bits != 0))) {
      add_bytes = (info_bits_count - prev_bits) / 8 +
                  (((info_bits_count - prev_bits) % 8) != 0);
      for (size_t i = 0;
           (i < add_bytes) && (current_count_bytes < input.file_total_bytes);
           ++i, ++current_byte) {
        current_byte = input_stream.get();
        for (size_t j = 0; j < CHAR_BIT; ++j) {
          current_bits[CHAR_BIT - 1 - i] = (current_byte >> 1) & 1;
        }
        for (size_t j = 0; j < CHAR_BIT; ++j) {
          while (current_bit == control_bit - 1) {
            ++current_bit;
            control_bit *= 2;
          }
          if (current_bit >= block_size) {
            next_ham_bits[count_next_bits++] = current_bits[j];
          } else {
            cur_ham_code[current_bit++] = current_bits[j]; 
          }
        }
      }
      HammingEncode(cur_ham_code);
      for (size_t i = 0; i < block_size; ++i) {
        current_byte = 0;
        for (size_t j = 0; j < CHAR_BIT; ++j) {
          if (cur_ham_code[i + j] == 1) {
            current_byte +=
                static_cast<uint8_t>(binary_pow(2, CHAR_BIT - 1 - j));
          }
        }
        output << current_byte;
      }
      for (size_t i = 0; i < block_size; ++i) {
        cur_ham_code[i] = 0;
      }
      current_bit = 0;
      control_bit = 1;
      for (size_t i = 0; i < count_next_bits; ++i) {
        while (current_bit == control_bit - 1) {
          ++current_bit;
          control_bit *= 2;
        }
        cur_ham_code[current_bit++] = next_ham_bits[i];
      }
      prev_bits = count_next_bits;
      count_next_bits = 0;
    }
  }
}

