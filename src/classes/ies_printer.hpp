#pragma once
#include <fstream>

#include <fmt/ostream.h>

#include "ies_document.hpp"

class IES_Printer
{
public:
    static void print(const IES_Document& document, bool candela_to_file = false) {
        print_header(" DOCUMENT INFO ");
        print_key_value("Filename", document.filename);
        print_key_value("Document standard", document.get_standard());

        print_header(" LABELS ");
        print_labels(document.labels);

        print_header(" LAMP DATA ");
        print_key_value("Number of lamps", std::to_string(document.luminaire_data.lamp_number));
        print_key_value("Average lumens per lamp", std::to_string(document.luminaire_data.avg_lumens_per_lamp));
        print_key_value("Measurement unit", document.get_unit_type());
        print_key_value("Width", std::to_string(document.luminaire_data.lum_width));
        print_key_value("Length", std::to_string(document.luminaire_data.lum_length));
        print_key_value("Height", std::to_string(document.luminaire_data.lum_height));
        print_key_value("Total power consumed (watts)", std::to_string(document.luminaire_data.input_watts));
        print_key_value("Ballast factor", std::to_string(document.ballast_data.b_factor));
        print_key_value("Ballast-lamp factor", std::to_string(document.ballast_data.b_lamp_factor));

        print_header(" PHOTOMETRIC DATA ");
        print_key_value("Goniometer Type", document.get_photometric_type());
        print_key_value("Total lumens per lamp [average]",
                        std::to_string(document.luminaire_data.avg_lumens_per_lamp * document.luminaire_data.lamp_number));

        if (candela_to_file) {
            std::string new_path(document.filename);
            new_path = new_path.substr(0, new_path.length() - 3) + "csv";
            fmt::print(fmt::fg(fmt::color::yellow), "\nCandela values will be printed to:\n");
            fmt::print("{}\n", new_path);
            print_candela_to(new_path, document.photometric_data.h_angles_list, document.photometric_data.v_angles_list, document.photometric_data.candela);
        }
    }

private:
    static void print_header(const std::string&& str) {
        fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "\n{:-^60}\n", str);
    }

    static void print_key(const std::string& str) {
        fmt::print(fmt::fg(fmt::color::yellow), "{}:\n", str);
    }

    static void print_value(const std::string& val) {
        std::string str(val);
        if (str.length() > 60) {
            size_t split_index = 0;
            while (str.length() - split_index > 60) {
                split_index      = str.find_last_of(" ", split_index + 60);
                str[split_index] = '\n';
            }
        }
        fmt::print("{}", str);
    }

    static void print_key_value(const std::string& key, const std::string& val) {
        print_key(key);
        print_value(val);
        fmt::print("\n");
    }


    static void print_labels(Label_Data labels) {
        for (const auto& key: labels.keys()) {
            print_key(std::string(key));
            print_value(labels[key]);
            fmt::print("\n");
        }
    }

    static void print_candela_to(std::string path, const std::vector<float>& h_angles, const std::vector<float>& v_angles, const std::vector<float>& values) {
        std::ofstream candela_file;
        candela_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            candela_file.open(path, std::ios_base::out | std::ios_base::trunc);
            unsigned int columns = h_angles.size();
            
            fmt::print(candela_file, "{:8},", "");

            for (auto h : h_angles) {
                fmt::print(candela_file, "{:8.4f},", h);
            }

            fmt::print(candela_file, "\n");
            for (auto v : v_angles) {
                fmt::print(candela_file, "{:8.4f},", v);
                for (unsigned int i = 0; i < columns; ++i) {
                    fmt::print(candela_file, "{:8.4f},", values[i]);
                }
                fmt::print(candela_file, "\n");
            }
            candela_file.close();
        } catch (std::fstream::failure fail) {
            fmt::print(fmt::fg(fmt::color::red), "< ERROR > Failed to print candela values to csv.\n{}\n", fail.what());
        }
    }
};
