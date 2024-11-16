# add executables
add_executable(msc src/msc.c)

# add includes
target_include_directories(msc PUBLIC inc)

include_directories(${CMAKE_SOURCE_DIR}/shared)