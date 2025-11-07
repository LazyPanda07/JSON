cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
set(GTEST_VERSION 1.17.0)

project(Tests)

include(FetchContent)

FetchContent_Declare(
	gtest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG v${GTEST_VERSION}
)

FetchContent_MakeAvailable(gtest)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

if (UNIX)
    add_definitions(-D__LINUX__)
endif(UNIX)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    src/cases/JsonObjectTests.cpp
    src/cases/JsonBuilderTests.cpp
    src/cases/JsonParserTests.cpp
    src/cases/JsonRecursiveIteratorTests.cpp
    src/Utils.cpp    
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    include/
    ${GTEST_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/../JSON/include/
)

target_link_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/../JSON/lib
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    JSON
    gtest
    gtest_main
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
