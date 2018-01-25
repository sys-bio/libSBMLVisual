cmake_minimum_required (VERSION 2.8)
project (SagittariusSandbox)

add_executable(tikz-export tikz-export.c)
target_link_libraries(tikz-export sbnw)
set_target_properties( tikz-export PROPERTIES COMPILE_DEFINITIONS SBNW_CLIENT_BUILD=1 )

install(TARGETS tikz-export DESTINATION bin)
