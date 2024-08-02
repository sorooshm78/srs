cmake_minimum_required(VERSION 3.10)

project(user-agent VERSION 1.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PJSIP REQUIRED libpjproject)

add_executable(user-agent src/user_agent.cpp)
target_link_libraries(user-agent ${PJSIP_STATIC_LIBRARIES})