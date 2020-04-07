cmake_minimum_required(VERSION 3.15)
project(test_threads)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        global.h)

target_link_libraries(test_threads ${AMQPCPP_LIBRARY} ev pthread dl ssl)