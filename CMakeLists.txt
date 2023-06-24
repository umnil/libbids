# Replace values
# ProjectName: Change to your project name

cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
include(ExternalProject)
include(FetchContent)
set(PROJECT_NAME clibbids)
project("${PROJECT_NAME}" VERSION 0.0.2 LANGUAGES CXX)

SET(PY ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Flags
if(APPLE)
    SET(CMAKE_CXXFLAGS "${CMAKE_CXXFLAGS} -mmacos-version-min=10.15")
endif()

# Insert Dependencies here
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11
  GIT_TAG master
)
FetchContent_MakeAvailable(pybind11)
add_subdirectory("${pybind11_SOURCE_DIR}")
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

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
message(STATUS "Qt6 Found: ${Qt6_FOUND}")
message(STATUS "Qt Include: ${Qt6Widgets_INCLUDE_DIRS}")

# Build the app
if(NOT PY)
add_executable("${PROJECT_NAME}"
	# List your files to include in the project
  src/dataset.cpp
  src/entity.cpp
  src/event.cpp
  src/main.cpp
  src/session.cpp
  src/subject.cpp
  src/utils.cpp
)
else()
# Build the python module
pybind11_add_module("${PROJECT_NAME}"
  src/add.cpp
  src/entity.cpp
  src/ext.cpp
  src/session.cpp
)
endif()

# Include directories for your dependencies so that the #include directive
# works
target_include_directories("${PROJECT_NAME}" BEFORE
	PUBLIC
		include
    ${JSON_INCLUDE_DIR}
    ${Qt6Widgets_INCLUDE_DIRS}
)

# Link personally-made dependencies
# add_subdirectory

# Link your dependencies
target_link_libraries("${PROJECT_NAME}"
  PUBLIC json
)
#target_link_libraries("${PROJECT_NAME}" PRIVATE Qt6::Widgets)

# Add Tests
if(NOT PY)
add_subdirectory(tests)
endif()


# INSTALL
install(TARGETS "${PROJECT_NAME}" DESTINATION bin)
