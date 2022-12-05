cmake_minimum_required(VERSION 3.16)

add_library(ui_widgets STATIC MvgWidget.cpp
                              MvgWidget.hpp
                              WeatherWidget.cpp
                              WeatherWidget.hpp
                              DepartureWidget.cpp
                              DepartureWidget.hpp
                              SettingsWidget.cpp
                              SettingsWidget.hpp
                              CanteenWidget.cpp
                              CanteenWidget.hpp
                              DishWidget.cpp
                              DishWidget.cpp)
target_link_libraries(ui_widgets PUBLIC PkgConfig::GTKMM backend_mvg backend_weather
                                 PRIVATE logger backend_storage ui_utils re2::re2 backend_eatApi)
