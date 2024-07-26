cmake_minimum_required(VERSION 3.10)

project(pjsip-project VERSION 1.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PJSIP REQUIRED libpjproject)

add_executable(pjsip-project src/server.cpp)
target_link_libraries(pjsip-project ${PJSIP_STATIC_LIBRARIES})