#include "Departure.hpp"
#include "logger/Logger.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <memory>
#include <spdlog/spdlog.h>

namespace backend::mvg {
std::shared_ptr<Departure> Departure::from_json(const nlohmann::json& j) {
    if (!j.contains("plannedDepartureTime")) {
        SPDLOG_ERROR("Failed to parse departure. 'plannedDepartureTime' field missing.");
        return nullptr;
    }
    int64_t plannedDepartureTime = 0;  // In ms Unix time
    j.at("plannedDepartureTime").get_to(plannedDepartureTime);
    std::chrono::system_clock::time_point plannedDepartureTP = std::chrono::system_clock::from_time_t(static_cast<time_t>(plannedDepartureTime / 1000));

    if (!j.contains("transportType")) {
        SPDLOG_ERROR("Failed to parse departure. 'transportType' field missing.");
        return nullptr;
    }
    std::string product;
    std::string lineBackgroundColor = "#006cb3";
    j.at("transportType").get_to(product);
    ProductType pType = ProductType::UNKNOWN;
    if (product == "UBAHN") {
        pType = ProductType::U_BAHN;
        lineBackgroundColor = "#006cb3";
    } else if (product == "SBAHN") {
        pType = ProductType::S_BAHN;
        lineBackgroundColor = "#408335";
    } else if (product == "BUS") {
        pType = ProductType::BUS;
        lineBackgroundColor = "#00586a";
    } else if (product == "TRAM") {
        pType = ProductType::TRAM;
        lineBackgroundColor = "#d82020";
    } else if (product == "REGIONAL_BUS") {
        pType = ProductType::REGIONAL_BUS;
        lineBackgroundColor = "#00586a";
    } else {
        SPDLOG_WARN("Unknown MVG transport type '{}'.", product);
    }

    if (!j.contains("label")) {
        SPDLOG_ERROR("Failed to parse departure. 'label' field missing.");
        return nullptr;
    }
    std::string label;
    j.at("label").get_to(label);

    if (!j.contains("destination")) {
        SPDLOG_ERROR("Failed to parse departure. 'destination' field missing.");
        return nullptr;
    }
    std::string destination;
    j.at("destination").get_to(destination);

    // Delay is optional:
    int delayInMinutes = 0;
    if (j.contains("delayInMinutes")) {
        j.at("delayInMinutes").get_to(delayInMinutes);
    }

    if (!j.contains("cancelled")) {
        SPDLOG_ERROR("Failed to parse departure. 'cancelled' field missing.");
        return nullptr;
    }
    bool cancelled = false;
    j.at("cancelled").get_to(cancelled);

    int platform = 0;
    if (j.contains("platform")) {
        j.at("platform").get_to(platform);
    }

    // Is of type array:
    // if (!j.contains("infoMessages")) {
    //     SPDLOG_ERROR("Failed to parse departure. 'infoMessages' field missing.");
    //     return nullptr;
    // }
    // std::string infoMessages;
    // j.at("infoMessages").get_to(infoMessages);

    return std::make_shared<Departure>(Departure{plannedDepartureTP,
                                                 pType,
                                                 label,
                                                 destination,
                                                 delayInMinutes,
                                                 cancelled,
                                                 lineBackgroundColor,
                                                 platform,
                                                 ""});
}
}  // namespace backend::mvg
