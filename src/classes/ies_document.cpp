#include "ies_document.hpp"

IES_Document::IES_Document(std::string& path) {
    read_IES(path.c_str());
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

    unsigned int i;
    for (i = 0; i < 3; ++i) {
        if (lines[0] == format_identifier[i]) {
            standard = IES_Standard(i);
            return;
        }
    }
    standard = IES_Standard(i);
    return;
}

// clang-format off
std::vector<std::string_view> IES_Document::split_data(const std::string_view& data, const char* delimiter) {
    std::vector<std::string_view> storage;

    size_t it = 0;

    while (it < data.size()) {                                                      //Search the whole string
        size_t next = data.find(delimiter, it);                                     //Find the delimiter from the next character

        std::string_view line(data.substr(it, (next - it)));                        //Take the substring from current iterator to the delimiter
        storage.push_back(line);
        it = next + 1;                                                              //Continue searching the whole string from there on
    }

    storage = std::vector(storage.begin(),                                          //Filter empty lines
                            std::remove_if(storage.begin(), storage.end(), 
                            [](auto x){return x == "";}));                          

    return storage;
};
// clang-format on

const std::string IES_Document::get_standard() const {
    switch (standard) {
    case IES_Standard::IES02:
        return "IES 2002";
    case IES_Standard::IES91:
        return "IES 1991";
    case IES_Standard::IES95:
        return "IES 1995";
    default:
        return "IES 1986";
    };
}

std::string_view IES_Document::operator[](unsigned int line) const{
    return lines.at(line);
}

const std::vector<std::string_view>& IES_Document::get_lines() const {
    return lines;
}

const bool IES_Document::has_tilt() const {
    if ((tilt == IES_TILT::Unhandled) || (tilt == IES_TILT::NONE)) {
        return false;
    } else {
        return static_cast<bool>(tilt);
    }
}

std::optional<TILT_Data>& IES_Document::get_tilt_data() {
    return tilt_data;
}

const std::vector<std::string_view>::const_iterator IES_Document::end_of_document() const {
    return lines.cend();
}