set(TEST_PROJECT_NAME "${PROJECT_NAME}Test")

# GTEST Dependency
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

add_executable("${TEST_PROJECT_NAME}"
  ../src/dataset.cpp
  ../src/entity.cpp
  ../src/subject.cpp
  ../src/utils.cpp

	./src/main.cpp
  ./src/test_dataset.cpp
	./src/test_entity.cpp
  ./src/test_enums.cpp
  ./src/test_subject.cpp
  ./src/test_utils.cpp
)

target_include_directories("${TEST_PROJECT_NAME}" BEFORE
	PUBLIC
		../include
		./include
		"${gtest_SOURCE_DIR}/include"
    "${JSON_INCLUDE_DIR}"
    "${Qt6Widgets_INCLUDE_DIRS}"
)

target_link_libraries("${TEST_PROJECT_NAME}"
	PUBLIC
		gtest_main
    json
    Qt6::Widgets
)
