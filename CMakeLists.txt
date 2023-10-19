cmake_minimum_required(VERSION 3.10)

project(fc-sui)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

# debug flags
add_compile_definitions(LOGGING=0)

add_executable(fc-sui ${CMAKE_CURRENT_SOURCE_DIR}/fc-sui.cc)
target_sources(fc-sui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/card.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/card-storage.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/evaluation-type.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/game.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/memusage.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/mem_watch.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/move.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/search-interface.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/strategies-provided.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sui-solution.cc
)

add_executable(test-bin ${CMAKE_CURRENT_SOURCE_DIR}/test-main.cc)
target_sources(test-bin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/test.cc)
target_sources(test-bin PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/card.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/card-storage.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/evaluation-type.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/game.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/memusage.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/mem_watch.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/move.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/search-interface.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/strategies-provided.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sui-solution.cc
)