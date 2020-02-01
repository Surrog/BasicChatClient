cmake_minimum_required (VERSION 3.2)

project (BasicChatClient)

if (WIN32)
add_definitions(/std:c++latest)
add_definitions(/permissive-)
add_definitions(-D_WIN32_WINNT=0x0A00)
add_definitions(-DASIO_DISABLE_CONCEPTS)
else (WIN32)
add_definitions(-std=c++17)
endif(WIN32)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/asio/include")
include_directories("${PROJECT_SOURCE_DIR}/jsoncpp/include")

set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")

find_package( Threads REQUIRED )

# sources in the resolver_server directory
set (TRACKER_SOURCES 
	${SOURCES_DIR}/main.cpp
	)

add_executable(tracker ${TRACKER_SOURCES})
target_link_libraries( tracker Threads::Threads)