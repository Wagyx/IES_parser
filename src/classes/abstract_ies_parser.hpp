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

    virtual void parse_process(IES_Document& document) {
        find_reference_lines(document);
        parse_label();
        //metodo 3
    }

    virtual void find_first_label_line(const IES_Document& document) = 0;
    virtual void parse_label() = 0;

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

};

#endif // !ABSTRACT_IES_PARSER
