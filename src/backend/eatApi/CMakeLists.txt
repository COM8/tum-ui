cmake_minimum_required(VERSION 3.16)

add_library(backend_eatApi STATIC EatApiHelper.cpp
                                  EatApiHelper.hpp
                                  Canteen.cpp
                                  Canteen.hpp
                                  Dish.cpp
                                  Dish.hpp
                                  Label.cpp
                                  Label.hpp
                                  iso_week.hpp
                                  date.hpp
                                  HeadCount.cpp
                                  HeadCount.hpp)

target_link_libraries(backend_eatApi PRIVATE logger cpr::cpr
                                     PUBLIC nlohmann_json::nlohmann_json)
