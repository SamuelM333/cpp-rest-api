cmake_minimum_required(VERSION 3.12)
project(metal_api)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

set(PROJECT_INCLUDE_DIR "src/include")
set(PISTACHE_INCLUDE_DIR "/usr/local/include/pistache")
set(RAPIDJSON_ROOT_DIR "${PROJECT_SOURCE_DIR}/third-party/rapidjson")

set(HEADER_SEARCH_PATHS ${PROJECT_INCLUDE_DIR} ${PISTACHE_INCLUDE_DIR})


set(EXTRA_LIBRARY "-pthread -lssl -lpistache -lmongocxx -lbsoncxx")

set(LIBRARIES_SEARCH_PATHS ${PISTACHE_LIBRARY} ${EXTRA_LIBRARY})

find_package(RapidJSON)


find_package(libmongoc-1.0 1.7 REQUIRED)
message("-- mongoc found version \"${MONGOC_VERSION}\"")
message("-- mongoc include path \"${MONGOC_INCLUDE_DIRS}\"")
message("-- mongoc libraries \"${MONGOC_LIBRARIES}\"")

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

message("LIBMONGOCXX_INCLUDE_DIRS = ${LIBMONGOCXX_INCLUDE_DIRS}")
message("LIBMONGOCXX_LIBRARIES = ${LIBMONGOCXX_LIBRARIES}")

message("LIBBSONCXX_INCLUDE_DIRS = ${LIBBSONCXX_INCLUDE_DIRS}")
message("LIBBSONCXX_LIBRARIES = ${LIBBSONCXX_LIBRARIES}")

file(GLOB COMMON_LIBRARIES ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES})


include_directories(${HEADER_SEARCH_PATHS})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_SEARCH_PATHS} "${MONGOC_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${MONGOC_INCLUDE_DIRS}")
target_compile_definitions(${PROJECT_NAME} PRIVATE "${MONGOC_DEFINITIONS}")

target_include_directories(${PROJECT_NAME} PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBBSONCXX_INCLUDE_DIRS})
#target_link_libraries(App ${COMMON_LIBRARIES})