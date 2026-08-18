set(GARAGE_EXAMPLE_SOURCES
  src/main.cpp
)

add_executable(example ${GARAGE_EXAMPLE_SOURCES})
add_executable(garage::example ALIAS example)

if(MSVC)
  add_compile_options(/utf-8)
endif()

target_link_libraries(example
  PRIVATE
    garage::runtime
)
