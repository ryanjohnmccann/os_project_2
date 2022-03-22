
cmake_minimum_required(VERSION 3.16)
project(project_2_c C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(Main main.c prod_cons_MT.c prod_cons_MT.h)
target_link_libraries(Main PRIVATE Threads::Threads)
