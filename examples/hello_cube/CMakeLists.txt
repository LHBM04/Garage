cmake_minimum_required(VERSION 3.22)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(GarageExample02 VERSION 0.1.0 LANGUAGES CXX)

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    endif()

    option(BUILD_SHARED_LIBS "Build shared libraries" ON)

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../.." "${CMAKE_CURRENT_BINARY_DIR}/garage")
endif()

file(GLOB EXAMPLE_02_ROOT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE EXAMPLE_02_SRC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
set(EXAMPLE_02_SOURCES ${EXAMPLE_02_ROOT_SOURCES} ${EXAMPLE_02_SRC_SOURCES})

file(GLOB EXAMPLE_02_ROOT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
file(GLOB_RECURSE EXAMPLE_02_SRC_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)
set(EXAMPLE_02_HEADERS ${EXAMPLE_02_ROOT_HEADERS} ${EXAMPLE_02_SRC_HEADERS})

if(EXAMPLE_02_SOURCES)
    add_executable(GarageExample02 ${EXAMPLE_02_SOURCES} ${EXAMPLE_02_HEADERS})

    if(TARGET garage-clangd)
        add_dependencies(GarageExample02 garage-clangd)
    endif()

    target_compile_features(GarageExample02 PRIVATE cxx_std_20)
    set_target_properties(GarageExample02 PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_include_directories(GarageExample02 PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        )
    endif()

    target_link_libraries(GarageExample02 PRIVATE
        Garage::Garage
    )

    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        if(BUILD_SHARED_LIBS)
            target_compile_definitions(GarageExample02 PRIVATE GARAGE_DYNAMIC)
        else()
            target_compile_definitions(GarageExample02 PRIVATE GARAGE_STATIC)
        endif()

        target_compile_definitions(GarageExample02 PRIVATE
            $<$<CONFIG:Debug>:GARAGE_DEBUG>
            $<$<NOT:$<CONFIG:Debug>>:GARAGE_RELEASE>
        )
    endif()

    if(WIN32 AND BUILD_SHARED_LIBS)
        add_custom_command(TARGET GarageExample02 POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                $<TARGET_RUNTIME_DLLS:GarageExample02>
                "$<TARGET_FILE_DIR:GarageExample02>"
            COMMAND_EXPAND_LISTS
            COMMENT "Copying runtime DLLs for GarageExample02"
        )
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/content")
        add_custom_command(TARGET GarageExample02 POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/content"
                "$<TARGET_FILE_DIR:GarageExample02>/content"
            COMMENT "Copying content for GarageExample02"
        )
    endif()
else()
    message(STATUS "[GarageExample02] No .cpp source files found in examples/hello_cube/. GarageExample02 target creation skipped.")
endif()
