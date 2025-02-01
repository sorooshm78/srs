cmake_minimum_required(VERSION 3.10)

project(srs VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PJSIP REQUIRED libpjproject)

add_executable(srs src/main.cpp src/Config.cpp src/SRSAccount.cpp src/SRSCall.cpp)

target_link_libraries(srs PRIVATE ${PJSIP_STATIC_LIBRARIES})
target_link_libraries(srs PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(srs PRIVATE Microsoft.GSL::GSL)
target_include_directories(srs PRIVATE ${CMAKE_SOURCE_DIR}/include)
