#pragma once

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace backend::eatApi {
struct OpeningHours {
    std::chrono::weekday weekDay;
    std::string start;
    std::string end;

    static OpeningHours from_json(const nlohmann::json& j, std::chrono::weekday weekDay);
} __attribute__((aligned(128)));

struct Canteen {
    std::string id;
    std::string name;
    std::vector<OpeningHours> openingHours;

    static std::shared_ptr<Canteen> from_json(const nlohmann::json& j);
    const std::optional<OpeningHours> get_current_opening_hours();
} __attribute__((aligned(128)));
}  // namespace backend::eatApi