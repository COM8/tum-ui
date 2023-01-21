#pragma once

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace backend::eatApi {
struct HeadCount {
    /**
     * The absolut count of humans in the canteen. Only valid in case percent != -1.
     **/
    int count{};
    /**
     * The maximum number of humans in the canteen for the percent to be 100.00. Only valid in case percent != -1.
     **/
    int maxCount{};
    /**
     * Current capacity utilization of the canteen clamped to 0 and 100 or -1 in case no data is available.
     **/
    double percent{-1};
    /**
     * A time stamp indicating how up to date the response is. Only valid in case percent != -1.
     **/
    std::chrono::system_clock::time_point timestamp{};

    static std::shared_ptr<HeadCount> from_json(const nlohmann::json& j);
} __attribute__((aligned(32)));
}  // namespace backend::eatApi