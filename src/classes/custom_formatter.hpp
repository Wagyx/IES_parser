#include <fmt/format.h>

template <>
struct fmt::formatter<boost::program_options::options_description> : fmt::formatter<std::string_view>{

    template <typename FormatContext>
    auto format(const boost::program_options::options_description d, FormatContext& ctx) {
        std::stringstream ss;
        ss << d;
        return format_to(
            ctx.out(),
            ss.str()
        );
    }
};
