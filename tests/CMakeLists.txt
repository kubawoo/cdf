function(add_cdf_tests dir)
    file(GLOB test_src CONFIGURE_DEPENDS ${dir}/tc_*.c)
    foreach(src ${test_src})
        get_filename_component(test_name ${src} NAME_WE)
        add_executable(${test_name} ${src})
        target_link_libraries(${test_name} cdf)
        add_test(NAME ${test_name} COMMAND ${test_name})
        set_tests_properties(${test_name} PROPERTIES WORKING_DIRECTORY ${dir})
    endforeach()
endfunction()

# Core tests — always built when BUILD_TESTING is ON
add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/core)

# Module-specific tests — only when module is enabled
if(CDF_BUILD_JSON)
    add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/json)
endif()

if(CDF_BUILD_HTTP)
    add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/http)
endif()

if(CDF_BUILD_LOG)
    add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/log)
endif()

if(CDF_BUILD_DB)
    add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/db)
    add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/db-sqlite)
    add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/db-entity)
endif()
