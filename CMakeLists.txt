cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 20)
option(CMAKE_POSITION_INDEPENDENT_CODE "" ON)

project(JSON VERSION 3.1.3)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	add_definitions(-D__ANDROID__)

	set(ANDROID_NATIVE_API_LEVEL android-35)
	set(ANDROID_PLATFORM ${ANDROID_NATIVE_API_LEVEL})
elseif (UNIX)
    add_definitions(-D__LINUX__)
endif()

add_library(
	${PROJECT_NAME} STATIC
	src/JsonObject.cpp
	src/JsonArrayWrapper.cpp
	src/JsonBuilder.cpp
	src/JsonParser.cpp
	src/JsonUtility.cpp
	src/OutputOperations.cpp
	src/RecursiveJsonIterator.cpp
	src/MapJsonIterator.cpp
	src/Exceptions/BaseJSONException.cpp
	src/Exceptions/CantFindValueException.cpp
	src/Exceptions/WrongEncodingException.cpp
	src/Exceptions/WrongInputStreamException.cpp
)

if (DEFINED ENV{MARCH} AND NOT "$ENV{MARCH}" STREQUAL "")
	target_compile_options(${PROJECT_NAME} PRIVATE -march=$ENV{MARCH})
endif()	

target_include_directories(
	${PROJECT_NAME} PUBLIC
	include
)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY include DESTINATION .)
