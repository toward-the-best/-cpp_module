project(BoostExample)

cmake_minimum_required(VERSION 3.25)

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

add_executable(ex_filesystem ex_filesystem.cpp)
target_link_libraries(ex_filesystem boost_filesystem)