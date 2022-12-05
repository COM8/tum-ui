#include "Dish.hpp"
#include "date.hpp"
#include "logger/Logger.hpp"
#include <chrono>
#include <memory>
#include <vector>
#include <bits/chrono.h>

namespace backend::eatApi {
std::shared_ptr<Dish> Dish::from_json(const nlohmann::json& j) {
    if (!j.contains("name")) {
        SPDLOG_ERROR("Failed to parse canteen. 'name' field missing.");
        return nullptr;
    }
    std::string name;
    j.at("name").get_to(name);

    if (!j.contains("labels")) {
        SPDLOG_ERROR("Failed to parse canteen. 'labels' field missing.");
        return nullptr;
    }
    std::vector<std::string> labels;
    j.at("labels").get_to(labels);

    return std::make_shared<Dish>(name, "", labels);
}

std::shared_ptr<Menu> Menu::from_json(const nlohmann::json& j) {
    if (!j.contains("date")) {
        SPDLOG_ERROR("Failed to parse menu. 'date' field missing.");
        return nullptr;
    }
    std::string dateStr;
    j.at("date").get_to(dateStr);
    std::istringstream dateSs{dateStr};
    std::chrono::system_clock::time_point date;
    dateSs >> date::parse("%Y-%m-%d", date);

    if (!j.contains("dishes")) {
        SPDLOG_ERROR("Failed to parse menu. 'dishes' field missing.");
        return nullptr;
    }
    std::vector<std::shared_ptr<Dish>> dishes;
    nlohmann::json::array_t array = j.at("dishes");
    for (const nlohmann::json& jDish : array) {
        dishes.push_back(Dish::from_json(jDish));
    }
    return std::make_shared<Menu>(std::chrono::floor<std::chrono::days>(date), dishes);
}
}  // namespace backend::eatApi