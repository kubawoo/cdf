cmake_minimum_required(VERSION 3.20)
project(cdf LANGUAGES C)

include(cmake/GetGitVersion.cmake)
set(PROJECT_VERSION      "${CDF_VERSION}")
set(PROJECT_VERSION_MAJOR "${CDF_VERSION_MAJOR}")
set(PROJECT_VERSION_MINOR "${CDF_VERSION_MINOR}")
set(PROJECT_VERSION_PATCH "${CDF_VERSION_PATCH}")

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
