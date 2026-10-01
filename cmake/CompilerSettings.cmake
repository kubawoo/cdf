set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -std=c23 selects strict ISO C, under which glibc hides the POSIX declarations
# the tui module needs (cfmakeraw, wcwidth). Both macros are required: the
# first exposes the termios/ioctl interfaces, the second the wide-char widths.
# They are additive and do not change the dialect.
if(CDF_BUILD_TUI)
    add_compile_definitions(_DEFAULT_SOURCE _XOPEN_SOURCE=700)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -Wall")
endif()
