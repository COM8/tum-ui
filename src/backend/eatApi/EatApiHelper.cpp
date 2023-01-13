#include "EatApiHelper.hpp"
#include "backend/eatApi/Canteen.hpp"
#include "backend/eatApi/Dish.hpp"
#include "backend/eatApi/Label.hpp"
#include "backend/eatApi/date.hpp"
#include "cpr/api.h"
#include "cpr/cprtypes.h"
#include "logger/Logger.hpp"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include <memory>
#include <string>
#include <cpr/cpr.h>

namespace backend::eatApi {
std::vector<std::shared_ptr<Canteen>> parse_canteens_response(const std::string& response) {
    std::vector<std::shared_ptr<Canteen>> result;
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        nlohmann::json::array_t array = j;
        for (const nlohmann::json& jCanteen : array) {
            std::shared_ptr<Canteen> canteen = Canteen::from_json(jCanteen);
            if (canteen) {
                result.push_back(canteen);
            } else {
                SPDLOG_WARN("Parsing canteen JSON failed: {}", jCanteen.dump());
            }
        }
        SPDLOG_DEBUG("Found {} canteens.", result.size());
        return result;

    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing canteens from '{}' with: {}", response, e.what());
        return {};
    }
}

std::vector<std::shared_ptr<Canteen>> request_canteens() {
    SPDLOG_DEBUG("Requesting canteens...");
    cpr::Response response = cpr::Get(cpr::Url{"https://tum-dev.github.io/eat-api/enums/canteens.json"});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting canteens failed. Status code: {}\nResponse: {}", response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting canteens failed. Status code: {}\nError: {}", response.status_code, response.error.message);
        }
        return {};
    }
    SPDLOG_DEBUG("Canteens requested successfully. Parsing...");
    return parse_canteens_response(response.text);
}

std::vector<std::shared_ptr<Menu>> parse_dishes_response(const std::string& response) {
    std::vector<std::shared_ptr<Menu>> result;
    try {
        nlohmann::json j = nlohmann::json::parse(response);

        if (j.contains("days")) {
            nlohmann::json::array_t array = j.at("days");
            for (const nlohmann::json& jDay : array) {
                result.push_back(Menu::from_json(jDay));
            }
        }

        SPDLOG_DEBUG("Found {} days with dishes.", result.size());
        return result;

    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing dishes from '{}' with: {}", response, e.what());
        return {};
    }
}

std::vector<std::shared_ptr<Menu>> request_dishes(const std::string& canteenId, iso_week::year year, iso_week::weeknum week) {
    unsigned weekU = static_cast<unsigned>(week);
    SPDLOG_DEBUG("Requesting dishes for canteen '{}', year {} and week number {}...", canteenId, static_cast<int>(year), weekU);
    cpr::Response response = cpr::Get(cpr::Url{"https://tum-dev.github.io/eat-api/" + canteenId + "/" + std::to_string(static_cast<int>(year)) + "/" + (weekU < 10 ? "0" : "") + std::to_string(weekU) + ".json"});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting dishes failed. Status code: {}\nResponse: {}", response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting dishes failed. Status code: {}\nError: {}", response.status_code, response.error.message);
        }
        return {};
    }
    SPDLOG_DEBUG("Dishes requested successfully. Parsing...");
    return parse_dishes_response(response.text);
}

std::unordered_map<std::string, std::shared_ptr<Label>> parse_labels_response(const std::string& response) {
    std::unordered_map<std::string, std::shared_ptr<Label>> result;
    try {
        nlohmann::json j = nlohmann::json::parse(response);
        nlohmann::json::array_t array = j;
        for (const nlohmann::json& jLabel : array) {
            std::shared_ptr<Label> label = Label::from_json(jLabel);
            if (label) {
                result[label->id] = label;
            } else {
                SPDLOG_WARN("Parsing label JSON failed: {}", jLabel.dump());
            }
        }
        SPDLOG_DEBUG("Found {} labels.", result.size());
        return result;

    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing labels from '{}' with: {}", response, e.what());
        return {};
    }
}

std::unordered_map<std::string, std::shared_ptr<Label>> request_labels() {
    SPDLOG_DEBUG("Requesting labels...");
    cpr::Response response = cpr::Get(cpr::Url{"https://tum-dev.github.io/eat-api/enums/labels.json"});
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting labels failed. Status code: {}\nResponse: {}", response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting labels failed. Status code: {}\nError: {}", response.status_code, response.error.message);
        }
        return {};
    }
    SPDLOG_DEBUG("Labels requested successfully. Parsing...");
    return parse_labels_response(response.text);
}
}  // namespace backend::eatApi