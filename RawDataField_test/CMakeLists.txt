include(${PROJECT_SOURCE_DIR}/tools-cmake/ProjectCommon.cmake)

# All the source files
set(SRC RawDataField_test.cpp)

# We need these include directories
set(INC . ..)

# Libraries to link to
set(LIB ${PROJECT_NAME})

# Finally, add the test
add_test_executable(RawDataField_test "${SRC}" "${INC}" "${LIB}")
