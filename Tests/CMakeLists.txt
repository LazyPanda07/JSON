cmake_minimum_required(VERSION 3.27.0)

project(Tests)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    src/JSONBuilderTests.cpp
    src/JSONParserTests.cpp
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    include/
    ${PROJECT_SOURCE_DIR}/gtest/include/
    ${PROJECT_SOURCE_DIR}/../src/
)

target_link_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/gtest/lib/
    ${PROJECT_SOURCE_DIR}/../JSON/
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    gtest
    gtest_main
    JSON
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
