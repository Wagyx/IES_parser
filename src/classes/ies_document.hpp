#include <fmt/format.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

enum struct IES_Standard : unsigned int { IES86 = 86,
                                          IES91 = 91,
                                          IES95 = 95 };

class IES_Document
{
public:
    IES_Standard standard;
    std::string  filename;

    IES_Document(const std::string& path);
    IES_Document(const char* path);

    std::string_view operator[](unsigned int line);

private:
    std::string data;
    std::vector<std::string_view> lines;
    
    void read_IES(const char* path);
    std::vector<std::string_view> split_data(const std::string_view& data, const char* delimiter);
};
