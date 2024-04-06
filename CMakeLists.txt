 cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

 project(neuron)

 set (CMAKE_CXX_FLAGS "-g -Wall")

 set(SOURCES
    neuron.c
 )

 add_executable(neuron ${SOURCES})
 target_link_libraries(neuron m)
 
 set(SOURCES
    network_config_checker.c
    network_config_helper.c
 )

 add_executable(network_config_checker ${SOURCES})

 file(GLOB_RECURSE ALL_SOURCE_FILES
    *.c *.h *.cpp *.hpp *.cxx *.hxx *.cc *.hh *.cppm *.ipp *.ixx)
add_custom_target(format
    COMMAND clang-format
    -i
    ${ALL_SOURCE_FILES}
)