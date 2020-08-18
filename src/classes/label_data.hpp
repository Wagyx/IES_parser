#pragma once
#include <algorithm>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/ostream.h>

using label_map  = std::unordered_map<std::string_view, std::vector<std::string_view>>;
using label_pair = std::pair<std::string_view, std::vector<std::string_view>>;

class Label_Data
{
public:
    std::vector<std::string_view> keys() const {
        std::vector<std::string_view> keys {};

        auto key_extraction = [](label_pair key_values_pair) {
            return key_values_pair.first;
          };

        std::transform(data.begin(), data.end(), std::back_inserter(keys), key_extraction);
        return keys;
    }

    const std::string operator[](const std::string_view key) const {
        try {
            label_map::mapped_type           values = data.at(key);
            label_map::mapped_type::iterator it     = values.begin();

            std::string value(*it);
            std::for_each(std::next(it), values.end(), [&value](auto sv) { value += " " + std::string(sv); });
            return value;
        } catch (std::out_of_range exception) {
            fmt::print(std::cerr, "ERROR: {} was not found.\n", key);
            return std::string();
        }
    }

    std::pair<label_map::iterator, bool> emplace(std::string_view&& key, std::vector<std::string_view> values) {
        return data.emplace(key, values);
    }

    bool empty() const noexcept {
        return data.empty();
    }

    std::size_t count(const std::string_view& key) const {
        return data.count(key);
    }

private:
    label_map data;
};
