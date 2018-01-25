project (SagittariusSrc)

set(SOURCES 
    core/SagittariusAssert.c
    core/SagittariusCommon.cpp
    core/SagittariusException.cpp
    diag/error.cpp
    draw/tikz.cpp
    io/io.cpp
    interface/layout.cpp
    layout/arrowhead.cpp
    layout/box.cpp
    layout/canvas.cpp
    layout/fr.cpp
    layout/point.cpp
    math/cubic.cpp
    math/geom.cpp
    math/transform.cpp
    network/network.cpp
    sbml/autolayoutSBML.cpp
    util/string.c
    )

set(HEADERS 
    core/BaseWORDs.h
    core/config.h.in
    core/Debug.hpp
    core/DefaultBool.hpp
    core/SagittariusAssert.h
    core/SagittariusCommon.h
    core/SagittariusConfigure.h
    core/SagittariusCore.h
    core/SagittariusException.hpp
    core/SagittariusPlatform.h
    core/SagittariusPlatformWin.h
    core/SagittariusPrefetch.h
    diag/error.h
    draw/magick.h
    io/io.h
    interface/layout.h
    layout/arrowhead.h
    layout/box.h
    layout/canvas.h
    layout/curve.h
    layout/fr.h
    layout/layoutall.h
    layout/point.h
    math/allen.h
    math/dist.h
    math/geom.h
    math/min_max.h
    math/rand_unif.h
    math/round.h
    math/sig.h
    math/sign_mag.h
    math/transform.h
    network/network.h
    sbml/autolayoutSBML.h
    util/string.h
    )

configure_file(core/config.h.in core/config.h)

##Image Magick-related sources
if(LINK_WITH_MAGICK)
    set(MAGICK_SOURCES
        draw/magick.cpp)
    set(MAGICK_HEADERS
        draw/magick.h)
else()
    set(MAGICK_SOURCES) # Clear magick sources
    set(MAGICK_HEADERS)
endif()

set(SBNW_SOURCES ${SOURCES} ${MAGICK_SOURCES})
set(SBNW_HEADERS ${HEADERS} ${MAGICK_HEADERS})

# configure_file(math/transform.h math/transform.h COPYONLY)

if(ENABLE_PYTHON_BINDINGS)
    foreach(f ${SOURCES})
        configure_file(${f} ${f} COPYONLY)
    endforeach()
    foreach(f ${HEADERS})
        configure_file(${f} ${f} COPYONLY)
    endforeach()
    if(ENABLE_PYTHON_BINDINGS) # FIXME: Why?
        configure_file(python/sbnwmodule.c python/sbnwmodule.c COPYONLY)
    endif()
    configure_file(setup.py.in setup.py)
endif()

# EVIL!
# if(ENABLE_PYTHON3_BINDINGS)
#     foreach(f ${SOURCES})
#         add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dist/${f}
#             COMMAND ${CMAKE_COMMAND} -E
#             copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/dist/${f}
#             )
#     endforeach()
#     foreach(f ${HEADERS})
#         message("the file ${f} -> ${CMAKE_CURRENT_BINARY_DIR}/dist/${f}")
#         add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${f}
#             COMMAND ${CMAKE_COMMAND} -E
#             copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/dist/${f}
#             DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/${f})
#     endforeach()
# endif()

# Sphinx
configure_file(sphinx/source/conf.py sphinx/source/conf.py)
configure_file(sphinx/source/index.rst sphinx/source/index.rst COPYONLY)
configure_file(sphinx/Makefile sphinx/Makefile COPYONLY)

# ** Shared Library **
add_library(sbnw SHARED ${SBNW_SOURCES})

set_target_properties( sbnw PROPERTIES COMPILE_DEFINITIONS SBNW_CLIENT_BUILD=0 )
# if(SBNW_WINDOWS_BUILD)
#   set_target_properties( sbnw PROPERTIES OUTPUT_NAME "libsbnw" COMPILE_FLAGS "/DPYTHON_API_VER=3 /DPYTHON_SCREWED_IT_UP=1" PREFIX "" )
# endif()

#Link in dependencies

##SBML
if(LINK_WITH_LIBSBML)
#     if(SBNW_LINK_TO_STATIC_LIBSBML)
      target_link_libraries(sbnw ${LIBSBML_STATIC_LIBRARY} ${LIBSBML_EXTRA_LIBS}) # libxml2 libbz2 zdll libiconv Ws2_32
#     else()
#       target_link_libraries(sbnw ${LIBSBML_LIBRARY} ${LIBSBML_EXTRA_LIBS})
#     endif()
#     if(${WIN32})
#         if(SBNW_WIN_COPY_DEPS_TO_INSTALL)
#             install(CODE "execute_process( COMMAND \"${CMAKE_COMMAND}\" -E copy \"${LIBSBML_DIR}/bin/libsbml.dll\" \"${CMAKE_INSTALL_PREFIX}/bin/libsbml.dll\" )")
#         endif()
#     endif()
endif()
##Image Magick
if(LINK_WITH_MAGICK)
    target_link_libraries(sbnw ${MAGICK_LIBS})
endif()

# ** Static Library **
if(BUILD_STATIC_LIB)
  add_library(sbnw_static STATIC ${SBNW_SOURCES})
  set_target_properties( sbnw_static PROPERTIES COMPILE_DEFINITIONS SBNW_CLIENT_BUILD=0 )
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties( sbnw_static PROPERTIES OUTPUT_NAME "libsbnw_static" COMPILE_FLAGS "/DPYTHON_API_VER=${PYTHON_VERSION_MAJOR} /DPYTHON_SCREWED_IT_UP=1" PREFIX "" )
  else()
    set_target_properties( sbnw_static PROPERTIES COMPILE_FLAGS "-fPIC" )
  endif()

  # Link in dependencies

  # SBML
  target_link_libraries(sbnw_static ${LIBSBML_STATIC_LIBRARY} ${LIBSBML_EXTRA_LIBS})

  # Image Magick
  if(LINK_WITH_MAGICK)
      target_link_libraries(sbnw_static ${MAGICK_LIBS})
  endif()
endif()

#Library dist
install(TARGETS sbnw RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

#Header dist
add_custom_target(headers)
foreach(f ${SBNW_HEADERS})
    add_custom_command(TARGET headers COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/../include/sbnw/${f})
endforeach()

if(PYTHONINTERP_FOUND)
  add_library(sbnw_pythonmod SHARED python/sbnwmodule.c)
  if(BUILD_STATIC_LIB)
    target_link_libraries(sbnw_pythonmod sbnw_static)
  else()
    target_link_libraries(sbnw_pythonmod sbnw)
  endif()

  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(sbnw_pythonmod ${PYTHON_LIBRARY})
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(sbnw_pythonmod PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
  endif()

  set_target_properties(sbnw_pythonmod PROPERTIES OUTPUT_NAME "sbnw" COMPILE_FLAGS "${FPIC_FLAG} -DPYTHON_API_VER=${PYTHON_VERSION_MAJOR} -DPYTHON_SCREWED_IT_UP=1" PREFIX "" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sbnw_pythonmod_bin" LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sbnw_pythonmod_libn")
  set_property(TARGET sbnw_pythonmod PROPERTY INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIR}" APPEND)

  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
      set_target_properties(sbnw_pythonmod PROPERTIES SUFFIX ".pyd" )
  elseif(APPLE)
      set_target_properties(sbnw_pythonmod PROPERTIES SUFFIX ".so" )
  endif()
  install(TARGETS sbnw_pythonmod DESTINATION python/site-packages/sbnw)
  install(FILES python/__init__.py DESTINATION python/site-packages/sbnw)
  configure_file(python/setup.py.in setup.py)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/setup.py DESTINATION python/site-packages)
endif()

#Doxygen
#Other files besides headers that contain documentation
set(DOXYGEN_OTHER_FILES)
set(SBNW_DOXY_INPUT "")
foreach(f ${HEADERS} ${DOXYGEN_OTHER_FILES})
    set(SBNW_DOXY_INPUT "${SBNW_DOXY_INPUT} ${CMAKE_CURRENT_SOURCE_DIR}/${f}")
endforeach()
configure_file(../doxy/doxy.cfg ../doxy/doxy.cfg)
add_custom_target(docs COMMAND doxygen ../doxy/doxy.cfg)
