#pragma once

#include "Canteen.hpp"
#include "Dish.hpp"
#include "backend/eatApi/Label.hpp"
#include "backend/eatApi/date.hpp"
#include "iso_week.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace backend::eatApi {
std::vector<std::shared_ptr<Canteen>> request_canteens();
std::vector<std::shared_ptr<Menu>> request_dishes(const std::string& canteenId, iso_week::year year, iso_week::weeknum week);
std::unordered_map<std::string, std::shared_ptr<Label>> request_labels();
}  // namespace backend::eatApi