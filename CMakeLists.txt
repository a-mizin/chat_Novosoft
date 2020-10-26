cmake_minimum_required(VERSION 3.17)
project(CHAT)

#
# ChatLib
#
add_library(chatLib
        scr/chatLib/Chat.h
        scr/chatLib/Chat.cpp)

set_target_properties(chatLib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

#
# project itself
#
add_executable(chat scr/main.cpp)

set_target_properties(chat PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

add_dependencies(chat chatLib)
target_link_libraries(chat chatLib)