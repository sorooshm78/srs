# Format all .cpp files in src/
find src/ -name '*.cpp' -exec clang-format -i {} +

# Format all header files (.h, .hpp) in include/
find include/ -name '*.h' -o -name '*.hpp' -exec clang-format -i {} +