#pragma once

#include "backend/storage/Serializer.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace backend::storage {
struct SettingsData {
    /**
     * The location id, where the MVG widget should fetch departures for.
     * Default value: "de:09184:460" (Garching Forschungszentrum)
     **/
    std::string mvgLocation{"de:09184:460"};
    bool mvgBusEnabled = true;
    bool mvgTramEnabled = true;
    bool mvgSBahnEnabled = true;
    bool mvgUBahnEnabled = true;
    std::string mvgDestRegex{"Garching, Forschungszentrum"};
    bool mvgDestRegexEnabled = false;

    /**
     * The open weather map location for the weather forecast. 
     **/
    std::string weatherLat{"48.2617389"};
    std::string weatherLong{"11.6684135"};
    std::string openWeatherApiKey{"https://openweathermap.org/price"};

    /**
     * eat-api configuration for the selected canteen.
     **/
    std::string canteenId{"mensa-garching"};

} __attribute__((aligned(128)));

class Settings {
 public:
    explicit Settings(const std::filesystem::path& configFilePath);

    SettingsData data{};
    void write_settings();

 private:
    storage::Serializer fileHandle;
};

Settings* get_settings_instance();
}  // namespace backend::storage
