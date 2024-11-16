# add executables
add_executable(msc src/msc.c)

# add includes
target_include_directories(msc PUBLIC inc)