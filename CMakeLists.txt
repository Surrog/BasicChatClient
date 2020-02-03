cmake_minimum_required (VERSION 3.2)

project (BasicChatClient)

set (CMAKE_CXX_STANDARD 17)

if (WIN32)
add_definitions(/std:c++latest)
add_definitions(/permissive-)
add_definitions(-D_WIN32_WINNT=0x0A00)
add_definitions(-DASIO_DISABLE_CONCEPTS)
else (WIN32)
add_definitions(-std=c++17)
endif(WIN32)

add_definitions(-DASIO_NO_DEPRECATED)

set(SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")
set(TRACKER_SOURCES_DIR "${SOURCES_DIR}/tracker")
set(COMMON_SOURCES_DIR "${SOURCES_DIR}/common")
set(CLIENT_SOURCES_DIR "${SOURCES_DIR}/client")
set(JSONCPP_DIR "${PROJECT_SOURCE_DIR}/jsoncpp/dist")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${COMMON_SOURCES_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/asio/include")
include_directories("${JSONCPP_DIR}")

find_package( Threads REQUIRED )

# sources in the resolver_server directory
set (TRACKER_SOURCES 
	${TRACKER_SOURCES_DIR}/main.cpp
	${TRACKER_SOURCES_DIR}/tracker.cpp
	${TRACKER_SOURCES_DIR}/tracker.hpp
	${TRACKER_SOURCES_DIR}/tracker_config.cpp
	${TRACKER_SOURCES_DIR}/tracker_config.hpp
	${TRACKER_SOURCES_DIR}/client.cpp
	${TRACKER_SOURCES_DIR}/client.hpp

	${COMMON_SOURCES_DIR}/message.hpp
	${COMMON_SOURCES_DIR}/message.cpp

	${JSONCPP_DIR}/jsoncpp.cpp
	)


add_executable(tracker ${TRACKER_SOURCES})
target_link_libraries(tracker Threads::Threads)

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1) #added to support old travis gcc-8
	target_link_libraries(tracker stdc++fs)
endif()

set (CLIENT_SOURCES
	${CLIENT_SOURCES_DIR}/main.hpp
	${CLIENT_SOURCES_DIR}/main.cpp
	${CLIENT_SOURCES_DIR}/client_main.cpp

	${CLIENT_SOURCES_DIR}/config.hpp
	${CLIENT_SOURCES_DIR}/config.cpp

	${CLIENT_SOURCES_DIR}/peer.hpp
	${CLIENT_SOURCES_DIR}/peer.cpp

	${COMMON_SOURCES_DIR}/message.hpp
	${COMMON_SOURCES_DIR}/message.cpp
	${JSONCPP_DIR}/jsoncpp.cpp
	)

add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client Threads::Threads)

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1) #added to support old travis gcc-8
	target_link_libraries(client stdc++fs)
endif()