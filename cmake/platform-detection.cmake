set(GARAGE_PLATFORM_WINDOWS FALSE)
set(GARAGE_PLATFORM_LINUX FALSE)
set(GARAGE_PLATFORM_MACOS FALSE)

if(WIN32)
    set(GARAGE_PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    set(GARAGE_PLATFORM_MACOS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GARAGE_PLATFORM_LINUX TRUE)
else()
    message(WARNING "[Garage] Untested platform detected: ${CMAKE_SYSTEM_NAME}")
endif()

if(NOT (CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|x86_64)$" AND CMAKE_SIZEOF_VOID_P EQUAL 8))
    message(FATAL_ERROR "[Garage] Only x64 architecture is supported. Detected: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SIZEOF_VOID_P} bytes pointer)")
endif()

message(STATUS "[Garage] Detected Platform: ${CMAKE_SYSTEM_NAME}")
