
file(GLOB SERVER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/models/kernel/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/models/kernel)

list(APPEND SRC_FILES ${SERVER_SOURCE})