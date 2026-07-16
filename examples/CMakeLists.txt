add_executable(helloworld helloworld/main.c)
target_link_libraries(helloworld cdf)

add_executable(shapes
    shapes/main.c
    shapes/shape.c
    shapes/circle.c
    shapes/rectangle.c
    shapes/square.c
)
target_link_libraries(shapes cdf)

add_executable(wwwserver wwwserver/main.c)
target_link_libraries(wwwserver cdf)

# todo-list-api — depends on core + http + json + db + db-sqlite + db-entity + log
if(CDF_BUILD_DB_SQLITE)
    add_executable(todo-list-api
        todo-list-api/main.c
        todo-list-api/handlers.c
        todo-list-api/todo_item.c
    )
    target_link_libraries(todo-list-api cdf)
endif()
