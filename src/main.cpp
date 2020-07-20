#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <fmt/core.h>

#include "classes/custom_formatter.hpp"
#include "classes/ies_parser.hpp"

int main(int argc, char* argv[]) {
    IES_Parser* parser = new IES95_Parser();
    IES91_Parser* parser91 = new IES91_Parser();
    IES86_Parser* parser86 = new IES86_Parser();
    parser->to(parser91)->to(parser86);

    try {
        po::options_description descr("Allowed options:");
        descr.add_options()("help", "producing help message")("target", po::value<std::string>(), "specifying the file to be parsed")("print-to", po::value<std::string>(), "specifying where the parsing result should be printed to");

        po::positional_options_description target;
        target.add("target", 1);

        po::variables_map varMap;
        po::store(po::command_line_parser(argc, argv).options(descr).positional(target).run(), varMap);
        po::notify(varMap);

        if (varMap.count("help")) {
            fmt::print("{}", descr);
            return 0;
        }

        if (varMap.count("target")) {
            auto c = varMap["target"].as<std::string>();

            IES_Document doc(c);
            parser->parse(doc);
            
        } else {
            fmt::print("No target to parse was specified.\n");
        }

        if (varMap.count("print-to")) {
            //  TODO: retrieve parameter and print to file
            fmt::print("print-to function is WIP - Not Implemented Yet\n");
        }
    } catch (std::exception& e) {
        fmt::print("ERROR - {}\n", e.what());
        return 1;
    } catch (...) {
        fmt::print("UNHANDLED ERROR /!\\\n");
    }

    delete parser;
    delete parser86;
    delete parser91;

    return 0;
}
