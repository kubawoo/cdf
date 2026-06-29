include(CMakePackageConfigHelpers)

add_library(cdf_objects OBJECT
    core/ooc_array.c
    core/ooc_collection.c
    core/ooc_datetime.c
    core/ooc_iterator.c
    core/ooc_list.c
    core/ooc_map.c
    core/ooc_memory.c
    core/ooc_object.c
    core/ooc_pair.c
    core/ooc_primitives.c
    core/ooc_queue.c
    core/ooc_singleton.c
    core/ooc_stack.c
    core/ooc_string.c
    core/ooc_stringtokenizer.c
    core/console.c
    core/consoleoutputstream.c
    core/directory.c
    core/file.c
    core/fileinputstream.c
    core/fileoutputstream.c
    core/inputstream.c
    core/outputstream.c
    core/stringinputstream.c
    core/stringoutputstream.c
    $<$<BOOL:${CDF_BUILD_JSON}>:json/eventparser.c>
    $<$<BOOL:${CDF_BUILD_JSON}>:json/jsonobject.c>
    $<$<BOOL:${CDF_BUILD_JSON}>:json/parserhandlers.c>
    $<$<BOOL:${CDF_BUILD_HTTP}>:http/http_client.c>
    $<$<BOOL:${CDF_BUILD_HTTP}>:http/http_server.c>
    $<$<BOOL:${CDF_BUILD_HTTP}>:http/http_types.c>
    $<$<BOOL:${CDF_BUILD_HTTP}>:http/http_utils.c>
    $<$<BOOL:${CDF_BUILD_LOG}>:log/logger.c>
    $<$<BOOL:${CDF_BUILD_LOG}>:log/loggerfactory.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db/connection.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db/database.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db/prepared_statement.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db-sqlite/sqlite_connection.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db-sqlite/sqlite_database.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db-sqlite/sqlite_prepared_statement.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db-entity/entity.c>
    $<$<BOOL:${CDF_BUILD_DB}>:db-entity/entitymanager.c>
)

target_include_directories(cdf_objects PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    $<$<BOOL:${CDF_BUILD_JSON}>:${CMAKE_CURRENT_SOURCE_DIR}/json>
    $<$<BOOL:${CDF_BUILD_HTTP}>:${CMAKE_CURRENT_SOURCE_DIR}/http>
    $<$<BOOL:${CDF_BUILD_LOG}>:${CMAKE_CURRENT_SOURCE_DIR}/log>
    $<$<BOOL:${CDF_BUILD_DB}>:${CMAKE_CURRENT_SOURCE_DIR}/db>
    $<$<BOOL:${CDF_BUILD_DB}>:${CMAKE_CURRENT_SOURCE_DIR}/db-sqlite>
    $<$<BOOL:${CDF_BUILD_DB}>:${CMAKE_CURRENT_SOURCE_DIR}/db-entity>
)

target_link_libraries(cdf_objects PUBLIC
    $<$<BOOL:${CDF_BUILD_DB}>:sqlite3>
    m
)

add_library(cdf SHARED $<TARGET_OBJECTS:cdf_objects>)
target_link_libraries(cdf PUBLIC cdf_objects)

if(CDF_BUILD_STATIC)
    add_library(cdf_static STATIC $<TARGET_OBJECTS:cdf_objects>)
    target_link_libraries(cdf_static PUBLIC cdf_objects)
    set_target_properties(cdf_static PROPERTIES OUTPUT_NAME cdf)
    install(TARGETS cdf_static ARCHIVE DESTINATION lib)
endif()

install(TARGETS cdf
    LIBRARY DESTINATION lib
)

set(CDF_INCLUDE_INSTALL_DIR "include/cdf")
set(CDF_LIB_INSTALL_DIR "lib")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cdfConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/cdfConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cdfConfig.cmake
    INSTALL_DESTINATION ${CDF_LIB_INSTALL_DIR}/cmake/cdf
    PATH_VARS CDF_INCLUDE_INSTALL_DIR CDF_LIB_INSTALL_DIR
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cdfConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cdfConfigVersion.cmake"
    DESTINATION ${CDF_LIB_INSTALL_DIR}/cmake/cdf
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cdf.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/cdf.h
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cdf.h" DESTINATION ${CDF_INCLUDE_INSTALL_DIR})

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/json
    ${CMAKE_CURRENT_SOURCE_DIR}/http
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    ${CMAKE_CURRENT_SOURCE_DIR}/db
    ${CMAKE_CURRENT_SOURCE_DIR}/db-sqlite
    ${CMAKE_CURRENT_SOURCE_DIR}/db-entity
    DESTINATION ${CDF_INCLUDE_INSTALL_DIR}
    FILES_MATCHING PATTERN "*.h"
    PATTERN "http_utils.h" EXCLUDE
)
