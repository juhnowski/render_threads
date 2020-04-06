cmake_minimum_required(VERSION 3.15)
project(test_threads)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(AMQPCPP_LIBRARY amqpcpp)

add_executable(test_threads
        main.h
        main.cpp
        ImageContext.cpp
        ImageContext.h
        StreamContext.cpp
        StreamContext.h
        ApplicationContext.cpp
        ApplicationContext.h
        settings.h
        rabbit/Controller.cpp
        rabbit/Controller.h
        rabbit/Handler.cpp
        rabbit/Handler.h
        rabbit/Timer.cpp
        rabbit/Timer.h main.h commands/Command.cpp commands/Command.h commands/Executor.cpp commands/Executor.h commands/Add.cpp commands/Add.h commands/Start.cpp commands/Start.h commands/Stop.cpp commands/Stop.h libh/string_enum.h)

target_link_libraries(test_threads ${AMQPCPP_LIBRARY} ev pthread dl ssl)