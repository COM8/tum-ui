#pragma once

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace backend::eatApi {
struct Label {
    std::string id;
    std::string text;
    std::string abbreviation;

    static std::shared_ptr<Label> from_json(const nlohmann::json& j);
} __attribute__((aligned(128)));
}  // namespace backend::eatApi