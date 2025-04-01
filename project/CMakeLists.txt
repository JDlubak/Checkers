cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Werror")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -gdwarf-4")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(Boost_USE_MULTITHREADED ON)

project(Checkers)
enable_language(CXX)

find_package(Boost 1.65 COMPONENTS date_time unit_test_framework REQUIRED)

include_directories(
    include
    include/model
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIR})

set(SOURCE_FILES
        src/model/Computer.cpp
        src/model/Human.cpp
        src/model/Pawn.cpp
        src/model/Piece.cpp
        src/model/Player.cpp
        src/model/Queen.cpp
        src/model/State.cpp
        src/model/UI.cpp
)

add_executable(Checkers ${SOURCE_FILES}
        src/main.cpp
)

target_link_libraries(Checkers
        ${Boost_DATE_TIME_LIBRARY})


add_executable(Checker ${SOURCE_FILES}
        test/PiecesTest.cpp
        test/PlayersTest.cpp
        test/StateTest.cpp
        test/UITest.cpp
        test/master.cpp
)

target_include_directories(Checker PUBLIC include)

target_link_libraries(Checker
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

enable_testing()

add_test(NAME checkertester
    COMMAND Checker
    --report_level=detailed
    --log_level=all
    --color_output=yes
)











