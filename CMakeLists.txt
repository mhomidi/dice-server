# WIN32 should work, but is untested

IF (WIN32)
	FIND_PATH(OPENCL_INCLUDE_DIR CL/cl.h )
	FIND_LIBRARY(OPENCL_LIBRARIES opencl64 )
	IF( OPENCL_LIBRARIES )
		FIND_LIBRARY(OPENCL_LIBRARIES opencl32 )
	ENDIF( OPENCL_LIBRARIES )
ELSE (WIN32)
	FIND_PATH(OPENCL_INCLUDE_DIR CL/cl.h ~/NVIDIA_GPU_Computing_SDK/OpenCL/common/inc/ )
	FIND_LIBRARY(OPENCL_LIBRARIES OpenCL 
	  ENV LD_LIBRARY_PATH
	)
ENDIF (WIN32)

SET( OPENCL_FOUND "NO" )
IF(OPENCL_LIBRARIES )
	SET( OPENCL_FOUND "YES" )
ENDIF(OPENCL_LIBRARIES)

MARK_AS_ADVANCED(
  OPENCL_INCLUDE_DIR
)
option(WITH_DEBUG "Debug option" 0)

if(WITH_DEBUG)
  add_definitions(-DDEBUG=1)
endif()

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads)
find_package(OpenCL REQUIRED )

include_directories( ${OPENCL_INCLUDE_DIR} )

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
    service.proto
)
#
# Add Library target with protobuf sources
#
add_library(myproto ${PROTO_FILES})
target_link_libraries(myproto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(myproto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in myproto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET myproto LANGUAGE cpp)
protobuf_generate(TARGET myproto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")


set(SOURCES
    main.cpp server.cpp execution.cpp argument_handler.cpp kernel_handler
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

#
# Target
#
add_executable(server ${SOURCES})
target_link_libraries(server
    PRIVATE
        myproto
        ${OPENCL_LIBRARIES}
)


