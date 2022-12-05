#pragma once

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace backend::eatApi {
struct Dish {
    std::string name;
    std::string price;
    std::vector<std::string> labels;

    static std::shared_ptr<Dish> from_json(const nlohmann::json& j);
} __attribute__((aligned(128)));

struct Menu {
    std::chrono::year_month_day date;
    std::vector<std::shared_ptr<Dish>> dishes;

    static std::shared_ptr<Menu> from_json(const nlohmann::json& j);
} __attribute__((aligned(32)));
}  // namespace backend::eatApi