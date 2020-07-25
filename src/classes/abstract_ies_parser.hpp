#ifndef ABSTRACT_IES_PARSER
#define ABSTRACT_IES_PARSER

#include "ies_document.hpp"

class Abstract_IES_Parser
{
public:
    virtual std::unique_ptr<Abstract_IES_Parser>& to(std::unique_ptr<Abstract_IES_Parser> next_parser) = 0;

    virtual void parse(IES_Document& document) = 0;

    virtual ~Abstract_IES_Parser() = default;
};

class Concrete_IES_Parser : public Abstract_IES_Parser
{
    using string_view_vector = std::vector<std::string_view>;

public:
    Concrete_IES_Parser()
        : next { nullptr } {};

    std::unique_ptr<Abstract_IES_Parser>& to(std::unique_ptr<Abstract_IES_Parser> next_parser) final override {
        next = std::move(next_parser);
        return next;
    }

    void parse(IES_Document& document) override {
        if (next != nullptr) {
            return next->parse(document);
        } else {
            fmt::print("No parsing implementations were found.\n");
            return;
        }
    }

    std::unique_ptr<Abstract_IES_Parser> next;

protected:
    string_view_vector::const_iterator label_line;
    string_view_vector::const_iterator tilt_line;
    //string_view_vector::const_iterator data_line;

    virtual void parse_process(IES_Document& document) {
        find_reference_lines(document);
        parse_label(document);
        parse_tilt(document);
        parse_data(document);
    }

    template <typename T>
    auto parse_element(const string_view_vector::const_iterator line) {
        std::stringstream ss { std::string(*line) };
        T                 result;
        ss >> result;
        return result;
    }

    template <typename T>
    auto parse_elements(const size_t n, const string_view_vector::const_iterator line, const IES_Document& document) {
        std::vector<T> result;
        result.reserve(n);

        const string_view_vector::const_iterator end_of_document { document.end_of_document() };
        string_view_vector::const_iterator       current_line { line };
        std::stringstream                        sstream { std::string(*current_line) };

        while (result.size() < n) {
            auto begin = std::istream_iterator<T>(sstream);
            std::copy_n(begin, 1, std::back_inserter(result));
            if (sstream.eof() && (current_line + 1) != end_of_document) {
                ++current_line;
                sstream = std::stringstream(std::string(*current_line));
            }
        }

        return std::move(result);
    }

    virtual void find_first_label_line(const IES_Document& document) = 0;
    virtual void parse_label(IES_Document& document)                 = 0;

private:
    virtual void find_reference_lines(const IES_Document& document) {
        find_first_label_line(document);
        find_tilt_line(document);
    }

    void find_tilt_line(const IES_Document& document) {
        const auto& lines = document.get_lines();

        auto starts_with_TILT = [](const std::string_view& line) {
            return (line.find("TILT", 0) == 0);
        };

        tilt_line = std::find_if(lines.cbegin(), lines.cend(), starts_with_TILT);
    }

    void parse_tilt(IES_Document& document) {

        auto tilt_index = tilt_line->find("=") + 1;
        auto tilt_value = tilt_line->substr(tilt_index);

        try {
            document.tilt = string_to_TILT_Value.at(std::string(tilt_value));
        } catch (std::out_of_range oor) {
            document.tilt = IES_TILT::Unhandled;
            fmt::print("TILT value is unhandled. NONE will be considered as its value.\nException: {}\n", oor.what());
        }

        if (document.tilt == IES_TILT::INCLUDE) {
            unsigned int       orientation  = parse_element<unsigned int>(tilt_line + 1);
            unsigned int       angles_num   = parse_element<unsigned int>(tilt_line + 2);
            std::vector<float> tilt_angles  = parse_elements<float>(angles_num, tilt_line + 3, document);
            std::vector<float> mult_factors = parse_elements<float>(angles_num, tilt_line + 4, document);

            TILT_Data extracted_data { IES_TILT_Orientation(orientation),
                                       angles_num,
                                       std::move(tilt_angles),
                                       std::move(mult_factors) };

            document.get_tilt_data().emplace(std::move(extracted_data));
        }
    }

    void parse_data(IES_Document& document) {
        string_view_vector::const_iterator header_line = (document.tilt == IES_TILT::INCLUDE) ? tilt_line + 5 : tilt_line + 1;

        std::vector<float> header                    = parse_elements<float>(10, header_line, document);
        document.luminaire_data.lamp_number          = std::move(static_cast<unsigned int>(header[0]));
        document.luminaire_data.avg_lumens_per_lamp  = std::move(header[1]);
        document.photometric_data.candela_multiplier = std::move(header[2]);
        document.photometric_data.vertical_angles    = std::move(static_cast<unsigned int>(header[3]));
        document.photometric_data.horizontal_angles  = std::move(static_cast<unsigned int>(header[4]));
        document.photometric_data.photometric_type   = std::move(static_cast<IES_PhotoType>(header[5]));
        document.luminaire_data.unit_type            = std::move(static_cast<IES_UnitType>(header[6]));
        document.luminaire_data.lum_width            = std::move(header[7]);
        document.luminaire_data.lum_length           = std::move(header[8]);
        document.luminaire_data.lum_height           = std::move(header[9]);

        std::vector<float> ballast_and_watts = parse_elements<float>(5, header_line + 1, document);
        document.ballast_data.b_factor       = std::move(ballast_and_watts[0]);
        document.ballast_data.b_lamp_factor  = std::move(ballast_and_watts[1]);
        document.luminaire_data.input_watts  = std::move(ballast_and_watts[2]);

        unsigned int v { document.photometric_data.vertical_angles };
        unsigned int h { document.photometric_data.horizontal_angles };
        std::vector<float> data = parse_elements<float>(v + h + v * h, header_line + 2, document);
        std::transform(data.begin(), data.end(), data.begin(),
            [&](auto x) { return x * document.photometric_data.candela_multiplier * document.ballast_data.b_factor * document.ballast_data.b_lamp_factor; });
        std::move(data.begin(), data.begin() + v, std::back_inserter(document.photometric_data.v_angles_list));
        std::move(data.begin() + v, data.begin() + v + h, std::back_inserter(document.photometric_data.h_angles_list));
        std::move(data.begin() + v + h, data.end(), std::back_inserter(document.photometric_data.candela));

    }
};

#endif // !ABSTRACT_IES_PARSER
