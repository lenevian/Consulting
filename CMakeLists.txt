cmake_minimum_required(VERSION 3.25)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(ConsultingSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

find_package(Drogon CONFIG REQUIRED)

if(NOT MSVC)
    foreach(_tgt Drogon::Drogon Trantor::Trantor jsoncpp_lib)
        if(TARGET ${_tgt})
            set_target_properties(${_tgt} PROPERTIES INTERFACE_COMPILE_OPTIONS "")
        endif()
    endforeach()
endif()

find_package(GTest CONFIG REQUIRED)

set(SOURCES
        src/main.cpp
        src/db/Database.cpp
        src/controllers/WebController.cpp
)

add_executable(Consulting ${SOURCES})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/public" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/users.txt")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/users.txt" "")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/requests.txt")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/requests.txt" "")
endif()


target_include_directories(Consulting PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)


target_link_libraries(Consulting PRIVATE Drogon::Drogon)


enable_testing()
add_executable(ConsultingTests
        tests/TestUser.cpp
        tests/TestDatabase.cpp
        tests/TestVariant.cpp
        src/db/Database.cpp
)
target_include_directories(ConsultingTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(ConsultingTests PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ConsultingTests)