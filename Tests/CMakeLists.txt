cmake_minimum_required(VERSION 3.27.0)

project(Tests)

find_package(GTest)

if(NOT ${GTest_FOUND})
	set(GTEST_BOTH_LIBRARIES gtest gtest_main)
	set(GTEST_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/gtest/include/)
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 20)

if (UNIX)
    set(DLL ${PROJECT_SOURCE_DIR}/../JSON/lib/libJSON.so)
    add_definitions(-D__LINUX__)
else ()
    set(DLL ${PROJECT_SOURCE_DIR}/../JSON/dll/JSON.dll)
endif (UNIX)

if(EXISTS ${DLL})
    add_definitions(-DJSON_DLL)
    install(FILES ${DLL} DESTINATION bin)
endif()

add_executable(
    ${PROJECT_NAME}
    main.cpp
    src/JSONBuilderTests.cpp
    src/JSONParserTests.cpp
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
    ${PROJECT_SOURCE_DIR}/gtest/lib/
    ${PROJECT_SOURCE_DIR}/../JSON/lib
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    JSON
    ${GTEST_BOTH_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
