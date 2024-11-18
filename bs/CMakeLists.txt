# add executables
add_executable(bs src/bs.c)

# find pthread module
find_package(Threads REQUIRED)

# linking libraries
target_link_libraries(bs
    shared
    Threads::Threads
)

# add includes
target_include_directories(bs
    PUBLIC
        inc
        ${CMAKE_SOURCE_DIR}/shared
)