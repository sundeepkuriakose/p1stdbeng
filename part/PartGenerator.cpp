#include "PartGenerator.h"

#include <algorithm>
#include <cstring>
#include <string>

namespace bufman {

std::vector<Part> generate_parts(std::size_t count, int first_part_id) {
    std::vector<Part> parts;
    parts.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        Part part{};
        part.part_id = first_part_id + static_cast<int>(i);
        part.part_weight = 1.0f + static_cast<float>(i);
        part.part_color = static_cast<int>(i % 6); // use modulo to keep color within the 6 options
        part.part_price = 10.0f + static_cast<float>(i);

        // copy leaves space for '\0' at the end of the string
        // string name is reduced to part_name's size before copying
        const std::string name = "p" + std::to_string(part.part_id);
        name.copy(part.part_name, std::min(name.size(), sizeof(part.part_name) - 1));

        const std::string material = "m" + std::to_string(i % 9); // cycles through 9 materials
        material.copy(part.part_material, std::min(material.size(), sizeof(part.part_material) - 1));

        parts.push_back(part);
    }
    return parts;
}

}
