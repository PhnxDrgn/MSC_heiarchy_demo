project(shared)

add_library(shared
    SHARED
        src/client.c
)

target_include_directories(shared
    PUBLIC
        inc
)