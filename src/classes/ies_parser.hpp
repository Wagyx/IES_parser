#include "ies_document.hpp"

class Abstract_IES_Parser
{
public:
    virtual Abstract_IES_Parser* to(Abstract_IES_Parser* next_parser) = 0;

    virtual void parse(IES_Document& document) = 0;
};

class IES_Parser : public Abstract_IES_Parser
{
public:
    IES_Parser()
        : next(nullptr) { }

    Abstract_IES_Parser* to(Abstract_IES_Parser* next_parser) override {
        next = next_parser;
        return next_parser;
    }

    void parse(IES_Document& document) override {
        if (next != nullptr) {
            return next->parse(document);
        } else {
            fmt::print("No parsing implementations were found.\n");
            return;
        }
    }

private:
    Abstract_IES_Parser* next;
};

class IES95_Parser : public IES_Parser
{
public:
    void parse(IES_Document& document) override {
        if (document.get_standard() == "IES 1995") {
            //Do the parsing
            fmt::print("Parsing as {}", document.get_standard());
        } else {
            return IES_Parser::parse(document);
        }
    }
};

class IES91_Parser : public IES_Parser
{
public:
    void parse(IES_Document& document) override {
        if (document.get_standard() == "IES 1991") {
            //Do the parsing
            fmt::print("Parsing as {}", document.get_standard());
        } else {
            return IES_Parser::parse(document);
        }
    }
};

class IES86_Parser : public IES_Parser
{
public:
    void parse(IES_Document& document) override {
        if (document.get_standard() == "IES 1986") {
            //Do the parsing
            fmt::print("Parsing as {}", document.get_standard());
        } else {
            fmt::print("< ERROR > Cannot parse this document.\n");
        }
    }
};