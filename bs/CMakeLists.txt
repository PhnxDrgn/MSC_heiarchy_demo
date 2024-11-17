# add executables
add_executable(bs src/bs.c)

# linking libraries
target_link_libraries(bs shared)

# add includes
target_include_directories(bs
    PUBLIC
        inc
        ${CMAKE_SOURCE_DIR}/shared
)