cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "DumpEventIMG")
set(DOKKAN_SDK_REV "r10")
set(PROJECT_LIBS dokkansdk)
set(INSTALL_LOCATION "C:/Users/Joffr/Downloads/MEmu Download/LooseLoad")

#----------------------------------------------------------------------#
#=== DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU ARE DOING ===#
#----------------------------------------------------------------------#
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_BUILD_TYPE MATCHES RELEASE)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
endif ()
file(GLOB_RECURSE PROJECT_SRC "src/*.h" "src/*.cpp" "src/*.c")
get_filename_component(UD_SDK_ROOT "${CMAKE_SOURCE_DIR}/../.." REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
message(STATUS "UniDokkan SDK Root: ${UD_SDK_ROOT}")
add_compile_options(-frelaxed-template-template-args)
add_definitions(-DUD_LOG_TAG="${PROJECT_NAME}")
add_subdirectory("${UD_SDK_ROOT}/dokkan-sdk-${DOKKAN_SDK_REV}" "${CMAKE_BINARY_DIR}/dokkan-sdk" EXCLUDE_FROM_ALL)
add_library(${PROJECT_NAME} SHARED ${PROJECT_SRC})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})
set(CMAKE_LINK_FLAGS_RELEASE "${CMAKE_LINK_FLAGS_RELEASE} -Wl,--exclude-libs,ALL")
if(CMAKE_BUILD_TYPE MATCHES RELEASE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE_NAME:${PROJECT_NAME}>)
endif ()
set(CMAKE_INSTALL_PREFIX ${INSTALL_LOCATION})
if (NOT INSTALL_LOCATION STREQUAL "")
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ./)
endif ()
