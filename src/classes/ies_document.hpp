#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <array>
#include <string>
#include <string_view>
#include <vector>

enum struct IES_Standard : unsigned int { IES86 = 2,
                                          IES91 = 1,
                                          IES95 = 0 };


class IES_Document
{
public:
    std::string filename;

    IES_Document(std::string& path);
    std::string      get_standard();

    std::string_view operator[](unsigned int line);

private:
    const std::array<const char*, 2> format_identifier {"IESNA:LM-63-1995", "IESNA91"};

    std::string                   data;
    std::vector<std::string_view> lines;
    IES_Standard                  standard;

    void                          read_IES(const char* path);
    std::vector<std::string_view> split_data(const std::string_view& data, const char* delimiter);
};
