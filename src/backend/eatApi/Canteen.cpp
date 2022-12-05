#include "Canteen.hpp"
#include "logger/Logger.hpp"
#include "nlohmann/json.hpp"
#include <chrono>
#include <memory>
#include <optional>

namespace backend::eatApi {
OpeningHours OpeningHours::from_json(const nlohmann::json& j, std::chrono::weekday weekDay) {
    std::string start;
    j.at("start").get_to(start);
    std::string end;
    j.at("end").get_to(end);
    return OpeningHours{weekDay, start, end};
}

std::shared_ptr<Canteen> Canteen::from_json(const nlohmann::json& j) {
    if (!j.contains("canteen_id")) {
        SPDLOG_ERROR("Failed to parse canteen. 'canteen_id' field missing.");
        return nullptr;
    }
    std::string id;
    j.at("canteen_id").get_to(id);

    if (!j.contains("name")) {
        SPDLOG_ERROR("Failed to parse canteen. 'name' field missing.");
        return nullptr;
    }
    std::string name;
    j.at("name").get_to(name);

    if (!j.contains("open_hours")) {
        SPDLOG_ERROR("Failed to parse canteen. 'open_hours' field missing.");
        return nullptr;
    }
    nlohmann::json jOpeningHours = j.at("open_hours");
    std::vector<OpeningHours> openingHours;
    if (jOpeningHours.contains("mon")) {
        openingHours.push_back(OpeningHours::from_json(jOpeningHours.at("mon"), std::chrono::Monday));
    }
    if (jOpeningHours.contains("tue")) {
        openingHours.push_back(OpeningHours::from_json(jOpeningHours.at("tue"), std::chrono::Tuesday));
    }
    if (jOpeningHours.contains("wed")) {
        openingHours.push_back(OpeningHours::from_json(jOpeningHours.at("wed"), std::chrono::Wednesday));
    }
    if (jOpeningHours.contains("thu")) {
        openingHours.push_back(OpeningHours::from_json(jOpeningHours.at("thu"), std::chrono::Thursday));
    }
    if (jOpeningHours.contains("fri")) {
        openingHours.push_back(OpeningHours::from_json(jOpeningHours.at("fri"), std::chrono::Friday));
    }
    if (jOpeningHours.contains("sat")) {
        openingHours.push_back(OpeningHours::from_json(jOpeningHours.at("sat"), std::chrono::Saturday));
    }
    if (jOpeningHours.contains("sun")) {
        openingHours.push_back(OpeningHours::from_json(jOpeningHours.at("sun"), std::chrono::Sunday));
    }

    return std::make_shared<Canteen>(id, name, openingHours);
}

const std::optional<OpeningHours> Canteen::get_current_opening_hours() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::weekday today = std::chrono::weekday(std::chrono::floor<std::chrono::days>(now));
    for (const OpeningHours& openingHours : openingHours) {
        if (openingHours.weekDay == today) {
            return std::make_optional<OpeningHours>(openingHours);
        }
    }
    return std::nullopt;
}
}  // namespace backend::eatApi