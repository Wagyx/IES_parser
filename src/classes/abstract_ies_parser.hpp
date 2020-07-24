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
    std::vector<std::string_view>::const_iterator label_line;
    std::vector<std::string_view>::const_iterator tilt_line;
    std::vector<std::string_view>::const_iterator data_line;

    virtual void parse_process(IES_Document& document) {
        find_reference_lines(document);
        parse_label(document);
        parse_tilt(document);
        //metodo 3
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
            std::stringstream ss { std::string(*(tilt_line + 1)) };

            auto get_line_as_stream = [&](auto line) {
                ss = std::stringstream { std::string(*line) };
            };

            unsigned int orientation;
            ss >> orientation;

            get_line_as_stream(tilt_line + 2);
            unsigned int angles_num;
            ss >> angles_num;

            get_line_as_stream(tilt_line + 3);
            std::vector<float> tilt_angles(angles_num);
            for (unsigned int i = 0; i < angles_num; ++i) {
                ss >> tilt_angles.at(i);
            }
            
            get_line_as_stream(tilt_line + 4);
            std::vector<float> mult_factors(angles_num);
            for (unsigned int i = 0; i < angles_num; ++i) {
                ss >> mult_factors.at(i);
            }
            
            TILT_Data extracted_data{ IES_TILT_Orientation(orientation), angles_num, std::move(tilt_angles), std::move(mult_factors) };
            document.get_tilt_data().emplace(std::move(extracted_data));
        }
    }
};

#endif // !ABSTRACT_IES_PARSER
