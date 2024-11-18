# add executables
add_executable(msc src/msc.c)

# find pthread module
find_package(Threads REQUIRED)

# linking libraries
target_link_libraries(msc shared)
target_link_libraries(msc Threads::Threads)

# add includes
target_include_directories(msc
    PUBLIC
        inc
        ${CMAKE_SOURCE_DIR}/shared
)