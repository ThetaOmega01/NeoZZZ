cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(neo-zzz LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/core CORE_SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/rotation_systems ROT_SYS_SRC)

target_sources(${PROJECT_NAME}
        PRIVATE
        ${CORE_SRC}
        ${ROT_SYS_SRC})
