cmake_minimum_required (VERSION 2.8)
project (wh_client)
set (CMAKE_MODULE_PATH "~/devel/flogging;${CMAKE_MODULE_PATH}")

find_package (Flogging REQUIRED)
find_package (Threads)

set(BOOST_LIBS thread date_time system)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)

message ("Current dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message ("Flog includes: ${FLOGGING_INCLUDE_DIRS}")
message ("Flog srcs: ${FLOGGING_SOURCE_DIRS}")

add_definitions ("-DPROFILE_ENABLED")
add_definitions ("-DPROFILE_STATIC")
include_directories (${FLOGGING_INCLUDE_DIRS})
add_library (fprof_client_lib STATIC "${FLOGGING_SOURCE_DIRS}/trace_client/profile.cpp")

add_executable (profile_cat_server connection.cpp main.cpp)

if (MINGW)
	target_link_libraries (profile_cat_server fprof_client_lib "c:/devel/QtSDK/mingw/lib/libws2_32.a")
endif()
target_link_libraries (profile_cat_server fprof_client_lib ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
