cmake_minimum_required(VERSION 3.10)

project(agent VERSION 1.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PJSIP REQUIRED libpjproject)

add_executable(agent src/agent.cpp)
target_link_libraries(agent ${PJSIP_STATIC_LIBRARIES})