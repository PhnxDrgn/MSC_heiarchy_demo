# add executables
add_executable(msc src/msc.c)

# linking libraries
target_link_libraries(msc shared)

# add includes
target_include_directories(msc
    PUBLIC
        inc
        ${CMAKE_SOURCE_DIR}/shared
)