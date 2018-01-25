cmake_minimum_required (VERSION 2.8)
project (SagittariusSandbox)

add_executable(affine2d-inv-test affine2d-inv-test.cpp)
target_link_libraries(affine2d-inv-test sbnw)
set_target_properties( affine2d-inv-test PROPERTIES COMPILE_DEFINITIONS SBNW_CLIENT_BUILD=1 )

#Library dist
install(TARGETS affine2d-inv-test DESTINATION bin)
