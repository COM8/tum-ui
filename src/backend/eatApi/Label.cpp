#include "Label.hpp"
#include "logger/Logger.hpp"
#include "nlohmann/json.hpp"
#include <chrono>
#include <memory>
#include <optional>

namespace backend::eatApi {
std::shared_ptr<Label> Label::from_json(const nlohmann::json& j) {
    if (!j.contains("enum_name")) {
        SPDLOG_ERROR("Failed to parse label. 'enum_name' field missing.");
        return nullptr;
    }
    std::string id;
    j.at("enum_name").get_to(id);

    if (!j.contains("abbreviation")) {
        SPDLOG_ERROR("Failed to parse label. 'abbreviation' field missing.");
        return nullptr;
    }
    std::string abbreviation;
    j.at("abbreviation").get_to(abbreviation);

    if (!j.contains("text")) {
        SPDLOG_ERROR("Failed to parse label. 'text' field missing.");
        return nullptr;
    }
    nlohmann::json jText = j.at("text");

    if (!jText.contains("EN")) {
        SPDLOG_ERROR("Failed to parse label. 'EN' field missing.");
        return nullptr;
    }
    std::string text;
    jText.at("EN").get_to(text);

    return std::make_shared<Label>(id, text, abbreviation);
}
}  // namespace backend::eatApi