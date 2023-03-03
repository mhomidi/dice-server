
file(GLOB SERVER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/schedulers/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/schedulers)

list(APPEND SRC_FILES ${SERVER_SOURCE})