
file(GLOB SERVER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/models/argument/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/models/argument)

list(APPEND SRC_FILES ${SERVER_SOURCE})