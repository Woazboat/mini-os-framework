add_library(kernellib STATIC)
target_link_libraries(kernellib kernelinterface)

file(GLOB_RECURSE SOURCE_FILES *.c *.S)
target_sources(kernellib PRIVATE ${SOURCE_FILES})
target_include_directories(kernellib PUBLIC
  include)


target_link_libraries(${KERNEL_BIN} kernellib)
