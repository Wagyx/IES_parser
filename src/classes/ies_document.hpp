#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

enum struct IES_Standard : unsigned int { IES86 = 3,
                                          IES91 = 2,
                                          IES95 = 1,
                                          IES02 = 0 };

enum struct IES_TILT : unsigned char { NONE      = 0,
                                       INCLUDE   = 1,
                                       Unhandled = 2 };

enum struct IES_TILT_Orientation : unsigned int { Vertical           = 1,
                                                  AlwaysHorizontal   = 2,
                                                  NoRotateHorizontal = 3 };

struct TILT_Data {
    IES_TILT_Orientation orientation;
    unsigned int         tilt_angles_num;
    std::vector<float>   tilt_angles;
    std::vector<float>   mult_factors;
};

const std::unordered_map<std::string, IES_TILT> string_to_TILT_Value { { "NONE", IES_TILT::NONE },
                                                                       { "INCLUDE", IES_TILT::INCLUDE } };

class IES_Document
{
public:
    std::string                                                         filename;
    std::unordered_map<std::string_view, std::vector<std::string_view>> labels; //  TODO: it would be nice to have a wrapper around this
    IES_TILT                                                            tilt;

    IES_Document(std::string& path);

    [[nodiscard]] const std::string                    get_standard() const;
    [[nodiscard]] const std::vector<std::string_view>& get_lines() const;
    [[nodiscard]] const bool                           has_tilt() const;
    [[nodiscard]] std::optional<TILT_Data>&            get_tilt_data();
    [[nodiscard]] std::string_view                     operator[](unsigned int line) const;

private:
    static constexpr std::array<const char*, 3> format_identifier { "IESNA:LM-63-2002", "IESNA:LM-63-1995", "IESNA91" };

    std::string                   data;
    std::vector<std::string_view> lines;
    IES_Standard                  standard;
    std::optional<TILT_Data>      tilt_data;

    void                          read_IES(const char* path);
    std::vector<std::string_view> split_data(const std::string_view& data, const char* delimiter);
};
