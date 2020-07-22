#include "abstract_ies_parser.hpp"

class IES86_Parser : public Concrete_IES_Parser
{
public:
    void parse(IES_Document& document) override {
        if (document.get_standard() == "IES 1986") {
            //Do the parsing
            fmt::print("Parsing as {}", document.get_standard());
            parse_process(document);
        } else {
            fmt::print("< ERROR > Cannot parse this document.\n");
        }
    }

    void find_first_label_line(const IES_Document& document) final override {
        label_line = document.get_lines().cbegin();
    }

    void parse_label(IES_Document& document) final override {

        auto clean_prefix = [](std::string_view& sv) {
            while (std::isspace(sv[0])) {
                sv.remove_prefix(1);
            }
        };

        auto clean_suffix = [](std::string_view& sv) {
            while (std::isspace(sv[sv.length() - 1])) {
                sv.remove_suffix(1);
            }
        };

        auto clean = [&](std::string_view& sv) {
            clean_prefix(sv);
            clean_suffix(sv);
        };

        for (auto line = label_line; line != tilt_line; ++line) {
            auto colon_index = line->find(":");
            if (colon_index == line->npos)
                continue;

            auto [key, first_value] = std::make_pair(line->substr(0, colon_index), line->substr(colon_index + 1));
            clean(key);
            clean(first_value);

            std::vector<std::string_view> values;

            if (line == label_line) {
                colon_index = first_value.find("DATE:");
                auto [date, date_value] = std::make_pair(first_value.substr(colon_index, 4), first_value.substr(colon_index + 5));
                first_value.remove_suffix(first_value.length() - colon_index);
                clean(first_value);
                clean(date_value);
                values.push_back(date_value);
                document.labels.emplace(date, values);
                values.clear();
            }

            values.push_back(std::move(first_value));

            while ((line + 1) != tilt_line && (line + 1)->find(":") == (line + 1)->npos) {
                line += 1;
                auto v_continue = *(line);
                clean(v_continue);
                values.push_back(std::move(v_continue));
            }

            document.labels.emplace(key, values);
        }

        if (document.labels.empty()) {
            std::vector<std::string_view> values;
            for (auto line = label_line; line != tilt_line; ++line) {
                values.push_back(*line);
            }
            document.labels.emplace(std::move(std::string_view("UNIQUE LABEL")), values);
        }
    }
};
