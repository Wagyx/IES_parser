#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <fmt/core.h>

int main(int argc, char* argv[]) {
    try {
        po::options_description descr("Allowed options");
        descr.add_options()
            ("help", "producing help message")
            ("compression", po::value<double>(), "set compression level")
        ;

        po::variables_map varMap;
        po::store(po::parse_command_line(argc, argv, descr), varMap);
        po::notify(varMap);

        if (varMap.count("help")) {
            //fmt::print("{}", descr);              //  a formatter should be implemented for this: https://fmt.dev/latest/api.html#formatting-user-defined-types
            std::cout << descr << "\n";
            return 0;
        }

        if (varMap.count("compression")) {
            auto c = varMap["compression"].as<double>();
            fmt::print("Compression level was set to {}\n", c);
        } else {
            fmt::print("Compression level was not set.\n");
        }
    }
    catch (std::exception& e) {
        fmt::print("ERROR - {}\n", e.what());
        return 1;
    }
    catch (...) {
        fmt::print("UNHANDLED ERROR /!\\\n");
    }

    return 0;
}