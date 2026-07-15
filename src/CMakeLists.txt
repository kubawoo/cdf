include(CMakePackageConfigHelpers)

add_library(cdf_objects OBJECT
    core/array.c
    core/collection.c
    core/datetime.c
    core/iterator.c
    core/list.c
    core/map.c
    core/memory.c
    core/object.c
    core/pair.c
    core/primitives.c
    core/queue.c
    core/singleton.c
    core/stack.c
    core/thestring.c
    core/stringtokenizer.c

    db/connection.c
    db/database.c
    db/prepared_statement.c
    db-entity/entity.c
    db-entity/entitymanager.c

    $<$<BOOL:${CDF_BUILD_DB_SQLITE}>:db-sqlite/sqlite_connection.c>
    $<$<BOOL:${CDF_BUILD_DB_SQLITE}>:db-sqlite/sqlite_database.c>
    $<$<BOOL:${CDF_BUILD_DB_SQLITE}>:db-sqlite/sqlite_prepared_statement.c>

    http/http_client.c
    http/http_server.c
    http/http_types.c
    http/http_utils.c

    io/console.c
    io/consoleoutputstream.c
    io/directory.c
    io/file.c
    io/fileinputstream.c
    io/fileoutputstream.c
    io/inputstream.c
    io/outputstream.c
    io/stringinputstream.c
    io/stringoutputstream.c

    json/eventparser.c
    json/jsonobject.c
    json/parserhandlers.c

    log/loggerfactory.c
    log/logger.c
)

target_include_directories(cdf_objects PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/db
    ${CMAKE_CURRENT_SOURCE_DIR}/db-entity
    $<$<BOOL:${CDF_BUILD_DB_SQLITE}>:${CMAKE_CURRENT_SOURCE_DIR}/db-sqlite>
    ${CMAKE_CURRENT_SOURCE_DIR}/http
    ${CMAKE_CURRENT_SOURCE_DIR}/io
    ${CMAKE_CURRENT_SOURCE_DIR}/json
    ${CMAKE_CURRENT_SOURCE_DIR}/log
)

target_link_libraries(cdf_objects PUBLIC
    $<$<BOOL:${CDF_BUILD_DB_SQLITE}>:sqlite3>
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
    ${CMAKE_CURRENT_SOURCE_DIR}/db
    ${CMAKE_CURRENT_SOURCE_DIR}/db-entity
    ${CMAKE_CURRENT_SOURCE_DIR}/db-sqlite
    ${CMAKE_CURRENT_SOURCE_DIR}/http
    ${CMAKE_CURRENT_SOURCE_DIR}/io
    ${CMAKE_CURRENT_SOURCE_DIR}/json
    ${CMAKE_CURRENT_SOURCE_DIR}/log
    DESTINATION ${CDF_INCLUDE_INSTALL_DIR}
    FILES_MATCHING PATTERN "*.h"
)
