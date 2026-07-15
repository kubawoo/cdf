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

add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/core)
add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/db)
add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/db-entity)
add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/http)
add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/io)
add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/json)
add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/log)

# Module-specific tests — only when module is enabled
if(CDF_BUILD_DB_SQLITE)
    add_cdf_tests(${CMAKE_CURRENT_SOURCE_DIR}/db-sqlite)
endif()

