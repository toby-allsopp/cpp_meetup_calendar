cmake_minimum_required(VERSION 3.0)

project(cpp_meetup_range_v3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(range-v3 INTERFACE)
target_include_directories(range-v3 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/range-v3/include)

add_executable(cpp_meetup_range_v3
    main.cpp
)
target_link_libraries(cpp_meetup_range_v3 PRIVATE range-v3)
if(MSVC)
target_compile_options(cpp_meetup_range_v3 PUBLIC /std:c++latest /await)
else()
    target_compile_options(cpp_meetup_range_v3 PUBLIC -Wall -Wpedantic -std=c++17 -fcoroutines-ts -stdlib=libc++)
    target_link_libraries(cpp_meetup_range_v3 PUBLIC -stdlib=libc++)
#elseif(CLANG)
#target_compile_options(cpp_meetup_range_v3 INTERFACE -Wall -Wpedantic -std=c++17 -fcoroutines-ts -stdlib=libc++)
#target_link_libraries(cpp_meetup_range_v3 INTERFACE -stdlib=libc++)
endif()

enable_testing()
add_test(cpp_meetup_range_v3 cpp_meetup_range_v3)
