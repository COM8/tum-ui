#include "MvgHelper.hpp"
#include "cpr/body.h"
#include "logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <cpr/api.h>
#include <cpr/cpr.h>
#include <cpr/cprtypes.h>
#include <cpr/payload.h>
#include <cpr/response.h>
#include <cpr/session.h>
#include <spdlog/spdlog.h>

namespace backend::mvg {
cpr::Url build_url(const std::string& stationId, bool bus, bool ubahn, bool sbahn, bool tram) {
    std::string transportTypes;
    if (bus) {
        transportTypes += "BUS";
    }

    if (ubahn) {
        if (!transportTypes.empty()) {
            transportTypes += ",";
        }
        transportTypes += "UBAHN";
    }

    if (sbahn) {
        if (!transportTypes.empty()) {
            transportTypes += ",";
        }
        transportTypes += "SBAHN";
    }

    if (tram) {
        if (!transportTypes.empty()) {
            transportTypes += ",";
        }
        transportTypes += "TRAM";
    }

    return cpr::Url("https://www.mvg.de/api/fib/v2/departure?globalId=" + stationId + "&limit=10" + "&offsetInMinutes=0" + "&transportTypes=" + transportTypes);
}

std::vector<std::shared_ptr<Departure>> parse_response(const std::string& response) {
    try {
        nlohmann::json j = nlohmann::json::parse(response);

        // Departures:
        nlohmann::json::array_t array = j;

        std::vector<std::shared_ptr<Departure>> result{};
        for (const nlohmann::json& jDep : array) {
            std::shared_ptr<Departure> dep = Departure::from_json(jDep);
            if (dep) {
                result.push_back(std::move(dep));
            } else {
                SPDLOG_DEBUG("Departure: {}", jDep.dump());
            }
        }
        SPDLOG_DEBUG("Found {} departures.", result.size());
        return result;

    } catch (nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("Error parsing departures from '{}' with: {}", response, e.what());
    }
    return std::vector<std::shared_ptr<Departure>>();
}

std::vector<std::shared_ptr<Departure>> request_departures(const std::string& stationId, bool bus, bool ubahn, bool sbahn, bool tram) {
    cpr::Session session;
    session.SetUrl(build_url(stationId, bus, ubahn, sbahn, tram));

    SPDLOG_DEBUG("Requesting departure times for station '{}'...", stationId);
    cpr::Response response = session.Get();
    if (response.status_code != 200) {
        if (response.error.code == cpr::ErrorCode::OK) {
            SPDLOG_ERROR("Requesting departures failed. Status code: {}\nResponse: {}", response.status_code, response.text);
        } else {
            SPDLOG_ERROR("Requesting departures failed. Status code: {}\nError: {}", response.status_code, response.error.message);
        }
        return std::vector<std::shared_ptr<Departure>>();
    }
    SPDLOG_DEBUG("Departures requested successfully. Parsing...");
    return parse_response(response.text);
}
}  // namespace backend::mvg
