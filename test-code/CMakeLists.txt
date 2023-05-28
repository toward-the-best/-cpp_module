#cmake 최소 버전
cmake_minimum_required(VERSION 3.16.2)
 
#프로젝트 이름
project(cpp_20)
 
#CMake 옵션 처리
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#컴파일 옵션 추가
add_compile_options(-ggdb3 -O3 -Wall -Werror)
 
#define 추가
add_definitions(-DFMT_HEADER_ONLY -D_REENTRANT)
 
#header include 경로
include_directories(
    #./include
)
 
#라이브러리 링크 디렉토리 경로
link_directories()
 
#전체 적용될 라이브러리 링크
link_libraries(
    pthread
)

#staic 실행 파일 생성 옵션
#set(CMAKE_EXE_LINKER_FLAGS "-static")

#targeting 하여 옵션을 처리합니다.
#TARGET_COMPILE_OPTIONS ( <Target_이름> PUBLIC <옵션> <옵션> ... )
#TARGET_INCLUDE_DIRECTORIES ( <Target_이름> PUBLIC <디렉토리> <디렉토리> ... )
#TARGET_LINK_LIBRARIES ( <Target_이름> <라이브러리> <라이브러리> ... )

#cmake 변수 처리
file(GLOB all_srcs "./*.cpp")
 
#실행 파일 생성
add_executable(main ${all_srcs})
