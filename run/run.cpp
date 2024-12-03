#include <lib/argparser/argparser.h>
#include <lib/archive/commands.h>

int run(int argc, char** argv) {
  Arguments arguments;
  arguments.Parse(argc, argv);
  std::string archname =
      arguments.GetOutput() + arguments.GetArchiveName() + ".haf";
  uint8_t additional_bits = arguments.GetControlBits();
  if (arguments.GetCreate()) {
    Create(archname, arguments.GetListing(), additional_bits);
  } else if (arguments.GetList()) {
   List(std::filesystem::path(archname));
  } else if (arguments.GetAppend()) {
    Append(std::filesystem::path(archname), arguments.GetListing(),
           additional_bits);
  } else if (arguments.GetExtract()) {
    ExtractFile(std::filesystem::path(archname), arguments.GetListing(),
                arguments.GetOutput());
  } else if (arguments.GetConcatenate() !=
             std::pair<std::string, std::string>()) {
    std::pair<std::string, std::string> pairs(arguments.GetConcatenate());
    pairs.first = arguments.GetOutput() + pairs.first + ".haf";
    pairs.second = arguments.GetOutput() + pairs.second + ".haf";
    Concatenate(
        std::filesystem::path(archname),
        std::pair<std::filesystem::path, std::filesystem::path>(pairs));
  } else if (arguments.GetDelete()) {
    Delete(std::filesystem::path(archname), arguments.GetListing(),
           arguments.GetOutput());
  }
  return 0;
}