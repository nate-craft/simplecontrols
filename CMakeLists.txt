cmake_minimum_required(VERSION 3.5)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
pkg_check_modules(PULSEAUDIO REQUIRED libpulse)

include_directories(${PROJECT_SOURCE_DIR}/include ${GLIB_INCLUDE_DIRS} ${LIBNOTIFY_INCLUDE_DIRS} ${PULSEAUDIO_INCLUDE_DIRS})
link_directories(${PROJECT_SOURCE_DIR}/lib ${GLIB_LIBRARY_DIRS} ${LIBNOTIFY_LIBRARY_DIRS} ${PULSEAUDIO_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} 
    src/main.c
    src/audio.c
    src/brightness.c
    src/audio.h
    src/brightness.h
    src/settings.h
)

if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release) 
endif() 

if(CMAKE_BUILD_TYPE STREQUAL "Release") 
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${PROJECT_NAME}>)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -flto -s") 
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -flto")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g") 
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")

target_link_libraries(${PROJECT_NAME} ctk ${GLIB_LIBRARIES} ${LIBNOTIFY_LIBRARIES} ${PULSEAUDIO_LIBRARIES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
