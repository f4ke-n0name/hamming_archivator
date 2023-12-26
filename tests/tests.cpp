
#include <gtest/gtest.h>
#include <lib/argparser/argparser.h>
#include <lib/archive/commands.h>
#include <lib/archive/encode_file.h>

#include <run/run.cpp>

std::vector<bool> SetBits(size_t number, uint8_t control_bits,
                          uint8_t for_encode = 1) {
  uint64_t lenght = ((1 << control_bits) - for_encode * control_bits - 1);
  std::vector<bool> result(lenght, 0);

  for (uint64_t i = 0; i < lenght; ++i) {
    result[i] = ((number & (1 << i)) >> i);
  }
  return result;
}

uint64_t GetBits(std::vector<bool>& bits) {
  uint64_t result = 0;

  for (int64_t i = 0; i < bits.size() - 1; ++i) {
    result |= (bits[i] << i);
  }
  return result;
}

TEST(ArgParserTest, TestParse_0) {
  Arguments arguments;
  char** argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-c"};
  argv[2] = new char[3]{"-f"};
  argv[3] = new char[8]{"HAMARCH"};
  arguments.Parse(4, argv);
  ASSERT_EQ(arguments.GetArchiveName(), "HAMARCH");
}

TEST(ArgParserTest, TestParse_1) {
  Arguments arguments;
  char** argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[15]{"--file=HAMARCH"};
  argv[3] = new char[10]{"File1.txt"};
  argv[4] = new char[10]{"File2.txt"};
  arguments.Parse(5, argv);
  std::vector<std::string> list{"File1.txt", "File2.txt"};
  ASSERT_EQ(arguments.GetListing(), list);
}
TEST(ArgParserTest, TestParse_2) {
  Arguments arguments;
  char** argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"-A"};
  argv[2] = new char[6]{"FILE1"};
  argv[3] = new char[6]{"FILE2"};
  argv[4] = new char[15]{"--file=HAMARCH"};
  arguments.Parse(5, argv);
  std::pair<std::string, std::string> list{"FILE1", "FILE2"};
  ASSERT_EQ(arguments.GetConcatenate(), list);
}


TEST(ArchiveTest, TestEncodeHamming_0) {
  std::vector<bool> bits = SetBits(153, 4);
  HammingEncode(bits, 4);
  ASSERT_EQ(GetBits(bits), 2373);
}

TEST(ArchiveTest, TestEncodeHamming_1) {
  std::vector<bool> bits = SetBits(4652, 5);
  HammingEncode(bits, 5);
  ASSERT_EQ(GetBits(bits), 172651);
}

TEST(ArchiveTest, TestDecodeHamming_0) {
  std::vector<bool> bits = SetBits(153, 4, 0);
  HammingEncode(bits, 4);
  HammingDecode(bits, 4);
  ASSERT_EQ(GetBits(bits), 153);
}

TEST(ArchiveTest, TestDecodeHamming_1) {
  std::vector<bool> bits = SetBits(4652, 5, 0);
  // Одна ошибка
  HammingEncode(bits, 5);
  bits[3] = !bits[3];
  HammingDecode(bits, 5);
  ASSERT_EQ(GetBits(bits), 4652);
}

TEST(ArchiveTest, TestDecodeHamming_2) {
  std::vector<bool> bits = SetBits(6345, 5, 0); 
  HammingEncode(bits, 5);
  bits[4] = !bits[4];
  bits[7] = !bits[7];
  ASSERT_EXIT(HammingDecode(bits, 5), ::testing::ExitedWithCode(1),
              "Error: double error\n");
}

TEST(ArchiveTest, CreateTest) {
  char** argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[15]{"--file=HAMARCH"};
  argv[3] = new char[44] {"D:\\test\\file1.txt"};
  argv[4] = new char[44]{"D:\\test\\file2.txt"};
  ASSERT_NO_THROW(run(5, argv));
}

TEST(ArchiveTest, ExtractTest) {
  char** argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-x"};
  argv[2] = new char[15]{"--file=HAMARCH"};
  argv[3] = new char[10]{"file1.txt"};
  ASSERT_NO_THROW(run(4, argv));
}

TEST(ArchiveTest, ConcatenateTest) {
  char** argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[16]{"--file=HAMARCH1"};
  argv[3] = new char[44]{"D:\\test\\file1.txt"};
  run(4, argv);

  argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[16]{"--file=HAMARCH2"};
  argv[3] = new char[44]{"D:\\test\\file2.txt"};
  run(4, argv);

  argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-A"};
  argv[2] = new char[9]{"HAMARCH1"};
  argv[3] = new char[9]{"HAMARCH2"};
  argv[4] = new char[16]{"--file=HAMARCH3"};
  ASSERT_NO_THROW(run(5, argv));
}

TEST(ArchiveTest, DeleteTest) {
  char** argv = new char*[5];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[9]{"--create"};
  argv[2] = new char[16]{"--file=HAMARCH4"};
  argv[3] = new char[44]{"D:\\test\\file1.txt"};
  argv[4] = new char[44]{"D:\\test\\file2.txt"};
  run(5, argv);

  argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[16]{"--file=HAMARCH4"};
  argv[2] = new char[9]{"--delete"};
  argv[3] = new char[10]{"file1.txt"};
  run(4, argv);

  argv = new char*[4];
  argv[0] = new char[7]{"hamarc"};
  argv[1] = new char[3]{"-x"};
  argv[2] = new char[16]{"--file=HAMARCH4"};
  argv[3] = new char[10]{"file1.txt"};
  ASSERT_EXIT(run(4, argv), ::testing::ExitedWithCode(1),
              "Error: File is not in archive\n");
}