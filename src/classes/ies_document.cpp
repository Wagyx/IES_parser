#include "ies_document.hpp"

IES_Document::IES_Document(const std::string& path) {
    read_IES(path.c_str());
}

IES_Document::IES_Document(const char* path) {
    read_IES(path);
}


void IES_Document::read_IES(const char* path) {
    try {
        std::ifstream     file(path);
        std::stringstream converter;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        converter << file.rdbuf();
        data = converter.str();
    } catch (std::ifstream::failure fail) {
        fmt::print("< ERROR > Failed to read IES data.\n{}\n", fail.what());
    }
    filename = std::string(path);
    lines    = split_data(std::string_view(data), "\n");
}

// clang-format off
std::vector<std::string_view> IES_Document::split_data(const std::string_view& data, const char* delimiter) {
    std::vector<std::string_view> storage;

    size_t it = 0;

    while (it < data.size()) {                                  //Search the whole string
        size_t next = data.find(delimiter, it);                 //Find the delimiter from the next character

        std::string_view line(data.substr(it, (next - it)));    //Take the substring from current iterator to the delimiter
        if (line != "") {                                       //Filter empty lines
            storage.push_back(line);
        }

        it = next + 1;                                          //Continue searching the whole string from there on
    }

    return storage;
};
// clang-format on

std::string_view IES_Document::operator[](unsigned int line) {
    return lines.at(line);
}
