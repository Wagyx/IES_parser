#include "ies86_parser.hpp"
#include "ies91_parser.hpp"
#include "ies95_parser.hpp"

class IES_Parser
{
private:
    std::unique_ptr<Concrete_IES_Parser> parser;

public:
    IES_Parser() {
        parser = std::make_unique<IES95_Parser>();

        //std::unique_ptr<IES91_Parser> parser91 = std::make_unique<IES91_Parser>();
        //std::unique_ptr<IES86_Parser> parser86 = std::make_unique<IES86_Parser>();
        //parser->to(std::move(parser91))->to(std::move(parser86));
        
        parser->to(std::make_unique<IES91_Parser>())->to(std::make_unique<IES86_Parser>());
    }

    void parse(IES_Document& document) {
        parser->parse(document);
    }
};