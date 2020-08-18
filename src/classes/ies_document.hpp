#pragma once
#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

#include <fmt/format.h>
#include <fmt/color.h>

#include "label_data.hpp"

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

enum struct IES_UnitType : unsigned int { Feet   = 1,
                                          Meters = 2 };

enum struct IES_PhotoType : unsigned int { TypeC = 1,
                                           TypeB = 2,
                                           TypeA = 3 };

struct TILT_Data {
    IES_TILT_Orientation orientation;
    unsigned int         tilt_angles_num;
    std::vector<float>   tilt_angles;
    std::vector<float>   mult_factors;
};

struct Luminaire_Data {
    unsigned int lamp_number;
    float        avg_lumens_per_lamp;
    float        lum_width;
    float        lum_length;
    float        lum_height;
    IES_UnitType unit_type;
    float        input_watts;
};

struct Photometric_Data {
    float              candela_multiplier;
    unsigned int       vertical_angles;
    unsigned int       horizontal_angles;
    IES_PhotoType      photometric_type;
    std::vector<float> v_angles_list;
    std::vector<float> h_angles_list;
    std::vector<float> candela;
};

struct Ballast_Data {
    float b_factor;
    float b_lamp_factor;
};

const std::unordered_map<std::string, IES_TILT> string_to_TILT_Value { { "NONE", IES_TILT::NONE },
                                                                       { "INCLUDE", IES_TILT::INCLUDE } };

class IES_Document
{
public:
    std::string      filename;
    Label_Data   labels; //  TODO: it would be nice to have a wrapper around this
    IES_TILT         tilt;
    Luminaire_Data   luminaire_data;
    Photometric_Data photometric_data;
    Ballast_Data     ballast_data;

    IES_Document(std::string& path);

    const std::vector<std::string_view>::const_iterator end_of_document() const;
    [[nodiscard]] const std::string                     get_standard() const;
    [[nodiscard]] const std::vector<std::string_view>&  get_lines() const;
    [[nodiscard]] const bool                            has_tilt() const;
    [[nodiscard]] std::string                           get_unit_type() const;
    [[nodiscard]] std::string                           get_photometric_type() const;
    [[nodiscard]] std::optional<TILT_Data>&             get_tilt_data();
    [[nodiscard]] std::string_view                      operator[](unsigned int line) const;

private:
    static constexpr std::array<const char*, 3> format_identifier { "IESNA:LM-63-2002", "IESNA:LM-63-1995", "IESNA91" };

    std::string                   data;
    std::vector<std::string_view> lines;
    IES_Standard                  standard;
    std::optional<TILT_Data>      tilt_data;

    void                          read_IES(const char* path);
    std::vector<std::string_view> split_data(const std::string_view& data, const char* delimiter);
};
