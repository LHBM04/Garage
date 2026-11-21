cmake_minimum_required(VERSION 3.22)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(GarageExample03 VERSION 0.1.0 LANGUAGES CXX)

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    endif()

    option(BUILD_SHARED_LIBS "Build shared libraries" ON)

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/garage")
endif()

file(GLOB EXAMPLE_03_ROOT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE EXAMPLE_03_SRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
set(EXAMPLE_03_SOURCES ${EXAMPLE_03_ROOT_SOURCES} ${EXAMPLE_03_SRC_SOURCES})

file(GLOB EXAMPLE_03_ROOT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
file(GLOB_RECURSE EXAMPLE_03_SRC_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)
set(EXAMPLE_03_HEADERS ${EXAMPLE_03_ROOT_HEADERS} ${EXAMPLE_03_SRC_HEADERS})

if(EXAMPLE_03_SOURCES)
    add_executable(GarageExample03 ${EXAMPLE_03_SOURCES} ${EXAMPLE_03_HEADERS})

    if(TARGET garage-clangd)
        add_dependencies(GarageExample03 garage-clangd)
    endif()

    target_compile_features(GarageExample03 PRIVATE cxx_std_20)
    set_target_properties(GarageExample03 PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_include_directories(GarageExample03 PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        )
    endif()

    target_link_libraries(GarageExample03 PRIVATE
        Garage::Garage
    )

    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        if(BUILD_SHARED_LIBS)
            target_compile_definitions(GarageExample03 PRIVATE GARAGE_DYNAMIC)
        else()
            target_compile_definitions(GarageExample03 PRIVATE GARAGE_STATIC)
        endif()

        target_compile_definitions(GarageExample03 PRIVATE
            $<$<CONFIG:Debug>:GARAGE_DEBUG>
            $<$<NOT:$<CONFIG:Debug>>:GARAGE_RELEASE>
        )
    endif()

    if(WIN32 AND BUILD_SHARED_LIBS)
        add_custom_command(TARGET GarageExample03 POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                $<TARGET_RUNTIME_DLLS:GarageExample03>
                "$<TARGET_FILE_DIR:GarageExample03>"
            COMMAND_EXPAND_LISTS
            COMMENT "Copying runtime DLLs for GarageExample03"
        )
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/content")
        add_custom_command(TARGET GarageExample03 POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/content"
                "$<TARGET_FILE_DIR:GarageExample03>/content"
            COMMENT "Copying content for GarageExample03"
        )
    endif()
else()
    message(STATUS "[GarageExample03] No .cpp source files found in examples/hello_terrain/. GarageExample03 target creation skipped.")
endif()
