#pragma once
#include <format>
#include <ranges>
#include <regex>
/**
扩展std::formatter格式化任何可迭代的容器
语法为{:起始`元素间隔`结尾<元素格式化>}

例如
auto test = { 1, 2, 42, 66, 87 };
std::cout << std::format("{:[`, `]<:02>}", test) << std::endl;
输出: [01, 02, 42, 66, 87]
*/
template<std::ranges::input_range InputContainer>
struct std::formatter<InputContainer> {
    static inline const std::regex kFmtRegex{R"(([\s\S]*?)`([\s\S]*?)`([\s\S]*?)<([\s\S]*?)>)"};

    std::string beg_str;
    std::string end_str;
    std::string separator;
    std::string elem_fmt;

    constexpr auto parse(std::format_parse_context& ctx) {
        auto beg = ctx.begin();
        if (beg != ctx.end()) {
            std::smatch matches;
            auto end = std::ranges::find(ctx, '}');
            std::string total{ beg, end };
            if (std::regex_match(total, matches, kFmtRegex)) {
                beg_str = matches[1].str();
                separator = matches[2].str();
                end_str = matches[3].str();
                elem_fmt = '{' + matches[4].str() + '}';
                return end;
            }
            else
                throw std::format_error{"Illegal format container specifier."};
        }
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const InputContainer& container, FormatContext& ctx) const {
        std::string res = beg_str;
        auto iter = std::cbegin(container);
        for (; iter != std::cend(container) - 1; iter++) {
            res += std::vformat(elem_fmt, std::make_format_args(*iter)) + separator;
        }
        res += std::vformat(elem_fmt, std::make_format_args(*iter)) + end_str;
        return std::vformat_to(ctx.out(), res, std::make_format_args());
    }
};