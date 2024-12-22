cmake_minimum_required(VERSION 3.10)

project(srs VERSION 1.0)

find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PJSIP REQUIRED libpjproject)

add_executable(srs src/main.cpp)
target_link_libraries(srs ${PJSIP_STATIC_LIBRARIES})
target_link_libraries(srs nlohmann_json::nlohmann_json)
