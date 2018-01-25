cmake_minimum_required (VERSION 2.8)
project (SagittariusSandbox)

enable_testing()

add_executable(twocompsys_ex twocompsys_ex.c)
target_link_libraries(twocompsys_ex graphfab ${GTEST_BOTH_LIBRARIES})
set_target_properties( twocompsys_ex PROPERTIES COMPILE_DEFINITIONS SBNW_CLIENT_BUILD=1 )

#Library dist
install(TARGETS twocompsys_ex DESTINATION test)
