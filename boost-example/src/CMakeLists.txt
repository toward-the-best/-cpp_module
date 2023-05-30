project(BoostExample)

cmake_minimum_required(VERSION 3.16)

add_compile_options(-std=c++17 -ggdb3 -O3 -Wall -Werror -fPIE)

#find_package(Boost 1.82 REQUIRED COMPONENTS system filesystem)
INCLUDE_DIRECTORIES(
    ../thirdparty/boost_1_82_0/
)

LINK_DIRECTORIES(
    ../thirdparty/boost_1_82_0/stage/lib/
)

LINK_LIBRARIES(
#    boost_filesystem
)

#add_executable(ex_filesystem ex_filesystem.cpp)
#target_link_libraries(ex_filesystem boost_filesystem)

#add_executable(ex_thread ex_thread.cpp)
#target_link_libraries(ex_thread boost_thread)

add_executable(ex_thread_v2 ex_thread_v2.cpp)
target_link_libraries(ex_thread_v2 pthread)

#add_executable(ex_webserver ex_webserver.cpp)
#target_link_libraries(ex_webserver boost_thread)
