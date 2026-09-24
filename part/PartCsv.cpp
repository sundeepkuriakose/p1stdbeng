#include "PartCsv.h"

#include <array>
#include <charconv>
#include <cmath>
#include <fstream>
#include <string>
#include <ostream>

namespace bufman {
namespace {

bool parse_integer(const std::string& text, int& value) {
    if (text.empty()) {
        return false;
    }

    const char* first = text.data();
    const char* last = first + text.size();

    const auto result = std::from_chars(first, last, value);

    return result.ec == std::errc{} && result.ptr == last;
}

bool parse_float(const std::string& text, float& value) {
    if (text.empty()) {
        return false;
    }

    const char* first = text.data();
    const char* last = first + text.size();

    const auto result = std::from_chars(first, last, value);

    return result.ec == std::errc{} &&
           result.ptr == last &&
           std::isfinite(value);
}

bool split_fields(
    const std::string& line,
    std::array<std::string, 6>& fields) {
    std::size_t comma_count = 0;

    for (char character : line) {
        if (character == ',') {
            ++comma_count;
        }
    }

    if (comma_count != 5) {
        return false;
    }

    std::size_t start = 0;

    for (std::size_t i = 0; i < 5; ++i) {
        const std::size_t comma = line.find(',', start);
        fields[i] = line.substr(start, comma - start);
        start = comma + 1;
    }

    fields[5] = line.substr(start);
    return true;
}

bool parse_line(
    const std::string& line,
    Part& part,
    std::string& error) {
    std::array<std::string, 6> fields;

    if (!split_fields(line, fields)) {
        error = "expected exactly six comma-separated fields";
        return false;
    }

    int part_id = 0;
    int part_color = 0;
    float part_weight = 0.0F;
    float part_price = 0.0F;

    if (!parse_integer(fields[0], part_id) || part_id <= 0) {
        error = "part_id must be a positive integer";
        return false;
    }

    if (fields[1].size() > 9) {
        error = "part_name must contain at most 9 characters";
        return false;
    }

    if (!parse_float(fields[2], part_weight) || part_weight < 0.0F) {
        error = "part_weight must be a nonnegative number";
        return false;
    }

    if (!parse_integer(fields[3], part_color) ||
        part_color < PART_COLOR_WHITE ||
        part_color > PART_COLOR_BLUE) {
        error = "part_color must be an integer from 0 to 5";
        return false;
    }

    if (!parse_float(fields[4], part_price) || part_price < 0.0F) {
        error = "part_price must be a nonnegative number";
        return false;
    }

    if (fields[5].size() > 9) {
        error = "part_material must contain at most 9 characters";
        return false;
    }

    part = Part{};
    part.part_id = part_id;
    part.part_weight = part_weight;
    part.part_color = part_color;
    part.part_price = part_price;

    fields[1].copy(part.part_name, fields[1].size());
    fields[5].copy(part.part_material, fields[5].size());

    return true;
}

} // namespace

PartLoadResult load_parts(
    const std::string& path,
    std::ostream& diagnostics) {
    PartLoadResult result;
    std::ifstream input(path);

    if (!input) {
        diagnostics << "cannot open CSV file: " << path << '\n';
        result.skipped = 1;
        return result;
    }

    std::string line;
    std::size_t line_number = 0;

    while (std::getline(input, line)) {
        ++line_number;

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            ++result.skipped;
            diagnostics << "line " << line_number
                        << ": blank line\n";
            continue;
        }

        Part part{};
        std::string error;

        if (!parse_line(line, part, error)) {
            ++result.skipped;
            diagnostics << "line " << line_number
                        << ": " << error << '\n';
            continue;
        }

        result.parts.push_back(part);
    }

    return result;
}

} // namespace bufman
