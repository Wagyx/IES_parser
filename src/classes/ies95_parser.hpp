#include "abstract_ies_parser.hpp"

class IES95_Parser : public Concrete_IES_Parser
{
public:
    void parse(IES_Document& document) override {
        if (document.get_standard() == "IES 1995") {
            //Do the parsing
            fmt::print("Parsing as {}", document.get_standard());
            parse_process(document);

        } else {
            return Concrete_IES_Parser::parse(document);
        }
    }

    void find_first_label_line(const IES_Document& document) final override {
        label_line = document.get_lines().cbegin() + 1;
    }

    void parse_label(IES_Document& document) {
    }
};
