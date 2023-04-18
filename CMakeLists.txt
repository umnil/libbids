# Replace values
# ProjectName: Change to your project name

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
include(ExternalProject)
set(PROJECT_NAME bids)
project("${PROJECT_NAME}" VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Insert Dependencies here
set(JSON_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/json")
ExternalProject_Add(jsoncpp
  PREFIX json
  GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
  GIT_TAG master
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${JSON_INSTALL_DIR}
)
set(JSON_PATH "${JSON_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jsoncpp${CMAKE_STATIC_LIBRARY_SUFFIX}")
set(JSON_INCLUDE_DIR "${JSON_INSTALL_DIR}/include")
message(STATUS "${JSON_INCLUDE_DIR}")
message(STATUS "${JSON_PATH}")
add_library(json STATIC IMPORTED)
set_target_properties(json PROPERTIES IMPORTED_LOCATION "${JSON_PATH}")

# find_package
# add_library
# set_target_properties ## Where applicable

# Build the app
add_executable("${PROJECT_NAME}"
	# List your files to include in the project
  src/entity.cpp
	src/main.cpp
	)

# Include directories for your dependencies so that the #include directive
# works
target_include_directories("${PROJECT_NAME}" BEFORE
	PUBLIC
		include
    ${JSON_INCLUDE_DIR}
)

# Link personally-made dependencies
# add_subdirectory

# Link your dependencies
target_link_libraries("${PROJECT_NAME}"
  PUBLIC json
)

# Add Tests
add_subdirectory(tests)


# INSTALL
install(TARGETS "${PROJECT_NAME}" DESTINATION bin)
