project (SagittariusSrc)

set(SOURCES 
    core/SagittariusAssert.c
    core/SagittariusCommon.cpp
    core/SagittariusException.cpp
    diag/gf_error.cpp
    draw/gf_tikz.cpp
    io/gf_io.cpp
    layout/gf_arrowhead.cpp
    layout/gf_box.cpp
    layout/gf_canvas.cpp
    layout/gf_fr.cpp
    layout/gf_point.cpp
    math/gf_cubic.cpp
    math/gf_geom.cpp
    math/gf_transform.cpp
    network/gf_network.cpp
    sbml/gf_autolayoutSBML.cpp
    sbml/gf_layout.cpp
    util/gf_string.c
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
    diag/gf_error.h
    draw/gf_magick.h
    io/gf_io.h
    layout/gf_arrowhead.h
    layout/gf_box.h
    layout/gf_canvas.h
    layout/gf_curve.h
    layout/gf_fr.h
    layout/gf_layoutall.h
    layout/gf_point.h
    math/gf_allen.h
    math/gf_dist.h
    math/gf_geom.h
    math/gf_min_max.h
    math/gf_rand_unif.h
    math/gf_round.h
    math/gf_sig.h
    math/gf_sign_mag.h
    math/gf_transform.h
    network/gf_network.h
    sbml/gf_autolayoutSBML.h
    sbml/gf_layout.h
    util/gf_string.h
    )

configure_file(core/config.h.in core/config.h)

##Image Magick-related sources
if(LINK_WITH_MAGICK)
    set(MAGICK_SOURCES
        draw/gf_magick.cpp)
    set(MAGICK_HEADERS
        draw/gf_magick.h)
else()
    set(MAGICK_SOURCES) # Clear magick sources
    set(MAGICK_HEADERS)
endif()

set(SBNW_SOURCES ${SOURCES} ${MAGICK_SOURCES})
set(SBNW_HEADERS ${HEADERS} ${MAGICK_HEADERS})

message("SBNW_SOURCES = ${SBNW_SOURCES}")

# configure_file(math/gf_transform.h math/gf_transform.h COPYONLY)

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
if(SBNW_WINDOWS_BUILD)
  set_target_properties( sbnw PROPERTIES OUTPUT_NAME "libsbnw" COMPILE_FLAGS "/DPYTHON_API_VER=3 /DPYTHON_SCREWED_IT_UP=1" PREFIX "" )
endif()

#Link in dependencies

##SBML
if(LINK_WITH_LIBSBML)
    if(NOT APPLE)
      if(SBNW_LINK_TO_STATIC_LIBSBML)
        # Note: Winsock is an implicit dependency of libxbml2
        target_link_libraries(sbnw libsbml-static libxml2 libbz2 zdll libiconv Ws2_32)
      else()
        target_link_libraries(sbnw ${LIBSBML_LIBS})
      endif()
    else()
      if(SBNW_LINK_TO_STATIC_LIBSBML)
        target_link_libraries(sbnw libsbml-static.a xml2 bz2 z iconv)
      else()
        target_link_libraries(sbnw ${LIBSBML_LIBS})
      endif()
    endif()
    if(${WIN32})
        if(SBNW_WIN_COPY_DEPS_TO_INSTALL)
            install(CODE "execute_process( COMMAND \"${CMAKE_COMMAND}\" -E copy \"${LIBSBML_DIR}/bin/libsbml.dll\" \"${CMAKE_INSTALL_PREFIX}/bin/libsbml.dll\" )")
        endif()
    endif()
endif()
##Image Magick
if(LINK_WITH_MAGICK)
    target_link_libraries(sbnw ${MAGICK_LIBS})
endif()

# ** Static Library **
if(BUILD_STATIC_LIB)
  add_library(sbnw_static STATIC ${SBNW_SOURCES})
  set_target_properties( sbnw_static PROPERTIES COMPILE_DEFINITIONS SBNW_CLIENT_BUILD=0 )
  if(SBNW_WINDOWS_BUILD)
    set_target_properties( sbnw_static PROPERTIES OUTPUT_NAME "libsbnw_static" COMPILE_FLAGS "/DPYTHON_API_VER=3 /DPYTHON_SCREWED_IT_UP=1" PREFIX "" )
  else()
    set_target_properties( sbnw_static PROPERTIES COMPILE_FLAGS "-fPIC" )
  endif()

  #Link in dependencies

  ##SBML
  if(LINK_WITH_LIBSBML)
      if(NOT APPLE)
        if(SBNW_LINK_TO_STATIC_LIBSBML)
          # Note: Winsock is an implicit dependency of libxbml2
          target_link_libraries(sbnw_static libsbml-static libxml2 libbz2 zdll libiconv Ws2_32)
        else()
          target_link_libraries(sbnw_static ${LIBSBML_LIBS})
        endif()
      else()
        if(SBNW_LINK_TO_STATIC_LIBSBML)
          target_link_libraries(sbnw_static libsbml-static.a xml2 bz2 z iconv)
        else()
          target_link_libraries(sbnw ${LIBSBML_LIBS})
        endif()
      endif()
  endif()
  ##Image Magick
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

# add_custom_target(frag ALL COMMAND echo -e \"frag\\nfrag\\nfrag\\n\" DEPENDS sbnw)
#add_custom_target(frag ALL COMMAND echo ladeda DEPENDS sbnw)
if(ENABLE_PYTHON3_BINDINGS)
    if(NOT ${WIN32})
        add_custom_target(python3build ALL COMMAND python3 setup.py build DEPENDS sbnw)
    endif()
#   if(PYTHONLIBS_FOUND)
    add_library(sbnw_python3mod SHARED python/sbnwmodule.c)
    target_link_libraries(sbnw_python3mod sbnw)
#     if(NOT APPLE)
      target_link_libraries(sbnw_python3mod ${PYTHON3_LIBRARY})
#     else()
#       set_target_properties(sbnw_python3mod PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
#     endif()
    set_target_properties(sbnw_python3mod PROPERTIES OUTPUT_NAME "sbnw" COMPILE_FLAGS "-fPIC -DPYTHON_API_VER=3 -DPYTHON_SCREWED_IT_UP=1" PREFIX "" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sbnw_python3mod_bin" LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sbnw_python3mod_lib")
    set_property(TARGET sbnw_python3mod PROPERTY INCLUDE_DIRECTORIES "${PYTHON3_INCLUDE_DIR}" APPEND)
    if(SBNW_WINDOWS_BUILD)
        set_target_properties(sbnw_python3mod PROPERTIES SUFFIX ".pyd" )
    elseif(APPLE)
        set_target_properties(sbnw_python3mod PROPERTIES SUFFIX ".so" )
    endif()
    install(TARGETS sbnw_python3mod DESTINATION python3/site-packages)
#   endif()
endif()
if(ENABLE_PYTHON2_BINDINGS)
#   if(PYTHONLIBS_FOUND)
    add_library(sbnw_python2mod SHARED python/sbnwmodule.c)
    if(BUILD_STATIC_LIB)
      target_link_libraries(sbnw_python2mod sbnw_static)
    else()
      target_link_libraries(sbnw_python2mod sbnw)
    endif()
    if(NOT APPLE)
      target_link_libraries(sbnw_python2mod ${PYTHON2_LIBRARY})
    else()
      set_target_properties(sbnw_python2mod PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    set_target_properties(sbnw_python2mod PROPERTIES OUTPUT_NAME "sbnw" COMPILE_FLAGS "-fPIC -DPYTHON_API_VER=2 -DPYTHON_SCREWED_IT_UP=1" PREFIX "" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sbnw_python2mod_bin" LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/sbnw_python2mod_libn")
    set_property(TARGET sbnw_python2mod PROPERTY INCLUDE_DIRECTORIES "${PYTHON2_INCLUDE_DIR}" APPEND)
    if(SBNW_WINDOWS_BUILD)
        set_target_properties(sbnw_python2mod PROPERTIES SUFFIX ".pyd" )
    elseif(APPLE)
        set_target_properties(sbnw_python2mod PROPERTIES SUFFIX ".so" )
    endif()
    install(TARGETS sbnw_python2mod DESTINATION python2/site-packages)
#   endif()
endif()

#Doxygen
#Other files besides headers that contain documentation
set(DOXYGEN_OTHER_FILES 
    #lexer/sm_c.c
    )
set(SBNW_DOXY_INPUT "")
foreach(f ${HEADERS} ${DOXYGEN_OTHER_FILES})
    set(SBNW_DOXY_INPUT "${SBNW_DOXY_INPUT} ${CMAKE_CURRENT_SOURCE_DIR}/${f}")
endforeach()
configure_file(../doxy/doxy.cfg ../doxy/doxy.cfg)
add_custom_target(docs COMMAND doxygen ../doxy/doxy.cfg)
