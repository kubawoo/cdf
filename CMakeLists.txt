cmake_minimum_required(VERSION 3.20)
project(cdf VERSION 0.5.0 LANGUAGES C)

include(cmake/Options.cmake)
include(cmake/CompilerSettings.cmake)

add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CDF_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
