aux_source_directory(. SRC_LIST)
add_executable(example ${SRC_LIST})
# 添加对gdb的支持, 需要放到源文件路径下
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")