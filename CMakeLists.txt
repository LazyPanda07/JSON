cmake_minimum_required(VERSION 3.27.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dir CACHE STRING "")
option(BUILD_SHARED_LIBS "" OFF)
option(CMAKE_POSITION_INDEPENDENT_CODE "" ON)

if (UNIX)
    add_definitions(-D__LINUX__)
endif (UNIX)

if (${BUILD_SHARED_LIBS})
	add_definitions(-DJSON_DLL)
endif ()

project(JSON)

add_library(
	${PROJECT_NAME}
	src/JSONArrayWrapper.cpp
	src/JSONBuilder.cpp
	src/JSONParser.cpp
	src/JSONUtility.cpp
	src/RecursiveJSONIterator.cpp
	src/Exceptions/BaseJSONException.cpp
	src/Exceptions/CantFindValueException.cpp
	src/Exceptions/WrongEncodingException.cpp
	src/Exceptions/WrongInputStreamException.cpp
)

install(
	TARGETS ${PROJECT_NAME} 
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/dll
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
	install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.h")
endif ()
