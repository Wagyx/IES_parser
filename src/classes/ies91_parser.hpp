#include <cassert>

#include "abstract_ies_parser.hpp"

class IES91_Parser : public Concrete_IES_Parser
{
public:
    void parse(IES_Document& document) override {
        if (document.get_standard() == "IES 1991") {
            //Do the parsing
            fmt::print("Parsing as {}\n", document.get_standard());
            parse_process(document);
        } else {
            return Concrete_IES_Parser::parse(document);
        }
    }

    void find_first_label_line(const IES_Document& document) final override {
        label_line = document.get_lines().cbegin() + 1;
    }

    void parse_label(IES_Document& document) { //TODO: cleanup the values from leading
                                               // and trailing whitespaces
        for (auto line = label_line; line != tilt_line; ++line) {
            auto [square1, square2] = find_key_delimiters(line);
            auto key                = line->substr(square1 + 1, square2 - square1 - 1);
            auto value              = line->substr(square2 + 1);

            std::vector<std::string_view> values { value };

            for (auto other_line = line + 1; other_line != tilt_line; ++other_line) {
                auto index = other_line->find("[MORE]");
                if (index != other_line->npos) {
                    auto another_value = other_line->substr(index + 6);
                    values.push_back(another_value);
                    line = other_line;
                } else {
                    break;
                }
            }

            document.labels.emplace(key, values);
        }

        assert(document.labels.count("TEST") > 0);
        assert(document.labels.count("MANUFAC") > 0);
    }

private:
    std::pair<size_t, size_t> find_key_delimiters(std::vector<std::string_view>::const_iterator line) {
        return std::make_pair(line->find("["), line->find("]"));
    }
};
