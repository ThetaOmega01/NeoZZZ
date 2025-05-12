cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(neo-zzz LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/core)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/core CORE_SRC)

target_sources(${PROJECT_NAME}
        PRIVATE
        ${CORE_SRC})
