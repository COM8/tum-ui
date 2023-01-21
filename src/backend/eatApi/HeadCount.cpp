#include "HeadCount.hpp"
#include "date.hpp"
#include "logger/Logger.hpp"
#include <chrono>
#include <memory>

namespace backend::eatApi {
std::shared_ptr<HeadCount> HeadCount::from_json(const nlohmann::json& j) {
    if (!j.contains("percent")) {
        SPDLOG_ERROR("Failed to parse head count. 'percent' field missing.");
        return nullptr;
    }
    double percent{};
    j.at("percent").get_to(percent);

    // Percent has to != -1 for a valid head count.
    if (percent == -1) {
        return nullptr;
    }

    if (!j.contains("count")) {
        SPDLOG_ERROR("Failed to parse head count. 'count' field missing.");
        return nullptr;
    }
    int count{};
    j.at("count").get_to(count);

    if (!j.contains("maxCount")) {
        SPDLOG_ERROR("Failed to parse head count. 'maxCount' field missing.");
        return nullptr;
    }
    int maxCount{};
    j.at("maxCount").get_to(maxCount);

    if (!j.contains("timestamp")) {
        SPDLOG_ERROR("Failed to parse head count. 'timestamp' field missing.");
        return nullptr;
    }
    std::string timestampStr{};
    j.at("timestamp").get_to(timestampStr);

    std::istringstream timestampSs{timestampStr};
    std::chrono::system_clock::time_point timestamp;
    timestampSs >> date::parse("%FT%T", timestamp);

    return std::make_shared<HeadCount>(count, maxCount, percent, timestamp);
}
}  // namespace backend::eatApi