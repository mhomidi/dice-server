# WIN32 should work, but is untested

include(./cmake/utils/openclFind.cmake)
include(./cmake/utils/protobufFind.cmake)
include(./cmake/services/grpc.cmake)

option(WITH_DEBUG "Debug option" 0)

if(WITH_DEBUG)
  add_definitions(-DDEBUG=1)
endif()

file(GLOB SRC_FILES)

include(./cmake/sources/server.cmake)
include(./cmake/sources/scheduler.cmake)
include(./cmake/sources/argumentModel.cmake)
include(./cmake/sources/kernelModel.cmake)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/readerwriterqueue")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/schedulers")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

message("source files: " ${SRC_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})

#
# Target
#
add_executable(server ${SRC_FILES})
target_link_libraries(server
    PRIVATE
        myproto
        ${OPENCL_LIBRARIES}
)


