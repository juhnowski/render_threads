cmake_minimum_required(VERSION 3.15)
project(test_threads)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set ( CMAKE_EXE_LINKER_FLAGS_RELEASE  "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -std=c++11 -Wall ")
set ( CMAKE_EXE_LINKER_FLAGS_DEBUG  "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -std=c++11 -Wall -O0 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined -D__FORTIFY_SOURCE=2 -rdynamic -DDEBUG")


find_package(OpenCV REQUIRED)

include(CMakeToolsHelpers OPTIONAL)

include_directories(${OpenCV_INCLUDE_DIRS})

find_library(AVCODEC_LIBRARY avcodec)
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVUTIL_LIBRARY avutil)
find_library(SWSCALE_LIBRARY swscale)

find_library(AMQPCPP_LIBRARY amqpcpp)

add_executable(test_threads
        main.h
        main.cpp
        rabbit/Timer.h
        rabbit/Timer.cpp
        rabbit/Handler.h
        rabbit/Handler.cpp
        rabbit/Controller.h
        rabbit/Controller.cpp
        commands/Executor.h
        commands/Executor.cpp
        commands/Add.h
        commands/Add.cpp
        commands/Start.h
        commands/Start.cpp
        commands/Stop.h
        commands/Stop.cpp
        commands/Command.h
        commands/Command.cpp
        ImageContext.h
        ImageContext.cpp
        StreamContext.h
        StreamContext.cpp
        settings.h
        global.h
        cvrender/master.cpp
        cvrender/master.h)

target_link_libraries(test_threads
        ${OpenCV_LIBS}
        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${SWSCALE_LIBRARY}
        ${AMQPCPP_LIBRARY}
        ev pthread dl ssl)