cmake_minimum_required (VERSION 3.8)
 
project (LZF_RtspServer)

include_directories(./src/base ./src/live ./src/scheduler ./src/net ./src/usageEnvironment)

aux_source_directory(./src/base SRC_BASE)
aux_source_directory(./src/live SRC_LIVE)
aux_source_directory(./src/net SRC_NET)
aux_source_directory(./src/scheduler SRC_SCHEDULER)
aux_source_directory(./src/usageEnvironment SRC_ENV)
 
add_executable(main ${SRC_BASE} ${SRC_LIVE} ${SRC_NET} ${SRC_ENV} ${SRC_SCHEDULER} ./src/main.cpp)