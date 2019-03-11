cmake_minimum_required(VERSION 3.12)
project(metal_api)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} src/main.cpp)

set(PROJECT_INCLUDE_DIR "src/include")
set(PISTACHE_INCLUDE_DIR "/usr/local/include/pistache")
set(HEADER_SEARCH_PATHS ${PROJECT_INCLUDE_DIR} ${PISTACHE_INCLUDE_DIR})
set(EXTRA_LIBRARY "-pthread -lssl -lpistache -lmongocxx -lbsoncxx")
set(LIBRARIES_SEARCH_PATHS ${EXTRA_LIBRARY})
set(EXTRA_DEFINITIONS "-DRAPIDJSON_HAS_STDSTRING=1")
add_definitions(${EXTRA_DEFINITIONS})

find_package(RapidJSON)
find_package(libmongoc-1.0 1.7 REQUIRED)
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

file(GLOB COMMON_LIBRARIES ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES})

include_directories(${HEADER_SEARCH_PATHS})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES_SEARCH_PATHS} "${MONGOC_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${MONGOC_INCLUDE_DIRS}")
target_compile_definitions(${PROJECT_NAME} PRIVATE "${MONGOC_DEFINITIONS}")

target_include_directories(${PROJECT_NAME} PUBLIC ${LIBMONGOCXX_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBBSONCXX_INCLUDE_DIRS})
