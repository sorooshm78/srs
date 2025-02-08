#!/bin/bash

# Define color codes
RED="\e[31m"
GREEN="\e[32m"
YELLOW="\e[33m"
BLUE="\e[34m"
MAGENTA="\e[35m"
CYAN="\e[36m"
RESET="\e[0m"

# Define directories
SRC_DIR=$(pwd)
PJPROJECT_DIR="$SRC_DIR/pjproject"
SRS_DIR="$SRC_DIR/src"
BUILD_DIR="$SRC_DIR/build"

# Function to build and install pjproject
build_pjproject() {
    echo -e "${YELLOW} ########## Building PJProject dependencies... ${RESET}"
    make -C $PJPROJECT_DIR dep
    if [ $? -ne 0 ]; then
        echo -e "${RED} ########## PJProject build failed. Exiting... ${RESET}"
        exit 1
    fi

    echo -e "${YELLOW} ########## Cleaning previous PJProject build... ${RESET}"
    make -C $PJPROJECT_DIR clean
    if [ $? -ne 0 ]; then
        echo -e "${RED} ########## PJProject clean failed. Exiting... ${RESET}"
        exit 1
    fi

    echo -e "${YELLOW} ########## Building PJProject... ${RESET}"
    make -C $PJPROJECT_DIR
    if [ $? -ne 0 ]; then
        echo -e "${RED} ########## PJProject build failed. Exiting... ${RESET}"
        exit 1
    fi
    
    echo -e "${YELLOW} ########## Instaling PJProject... ${RESET}"
    make -C $PJPROJECT_DIR install
    if [ $? -ne 0 ]; then
        echo -e "${RED} ########## PJProject install failed. Exiting... ${RESET}"
        exit 1
    fi
    
    echo -e "${GREEN} ########## PJProject built and installed successfully! ${RESET}"
}

# Function to build srs using CMake
build_srs() {
    echo -e "${YELLOW} ########## Building SRS... ${RESET}"

    # Create the build directory if it doesn't exist
    if [ ! -d "$BUILD_DIR" ]; then
        echo -e "${GREEN} ########## Creating build directory... ${RESET}"
        mkdir -p "$BUILD_DIR"
    fi

    # Run CMake to generate build files in the build directory
    echo -e "${YELLOW} ########## Running CMake... ${RESET}"
    cmake -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake -S $SRC_DIR -B $BUILD_DIR
    if [ $? -ne 0 ]; then
        echo -e "${RED} ########## CMake failed. Exiting... ${RESET}"
        exit 1
    fi

    # Run make to build the project inside the build directory
    echo -e "${YELLOW} ########## Running make... ${RESET}"
    make -C "$BUILD_DIR"
    if [ $? -ne 0 ]; then
        echo -e "${RED} ########## Build failed. Exiting... ${RESET}"
        exit 1
    fi

    echo -e "${GREEN} ########## SRS built successfully! ${RESET}"
}

# Main execution
build_pjproject  # Build and install PJProject
build_srs      # Build SRS using CMake
