# add executables
add_executable(mn src/mn.c)

# find pthread module
find_package(Threads REQUIRED)

# linking libraries
target_link_libraries(mn
    shared
    Threads::Threads
)

# add includes
target_include_directories(mn
    PUBLIC
        inc
        ${CMAKE_SOURCE_DIR}/shared
)