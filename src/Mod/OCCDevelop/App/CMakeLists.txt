
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${ZLIB_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
)

set(OCCDevelop_LIBS
    FreeCADApp
)

SET(OCCDevelop_SRCS
    AppOCCDevelop.cpp
    AppOCCDevelopPy.cpp
    PreCompiled.cpp
    PreCompiled.h
)

add_library(OCCDevelop SHARED ${OCCDevelop_SRCS})
target_link_libraries(OCCDevelop ${OCCDevelop_LIBS})


fc_target_copy_resource(OCCDevelop 
    ${CMAKE_SOURCE_DIR}/src/Mod/OCCDevelop
    ${CMAKE_BINARY_DIR}/Mod/OCCDevelop
    Init.py)

if(MSVC)
    set_target_properties(OCCDevelop PROPERTIES SUFFIX ".pyd")
    set_target_properties(OCCDevelop PROPERTIES DEBUG_OUTPUT_NAME "OCCDevelop_d")
    set_target_properties(OCCDevelop PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/OCCDevelop)
    set_target_properties(OCCDevelop PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(OCCDevelop PROPERTIES SUFFIX ".pyd")
    set_target_properties(OCCDevelop PROPERTIES DEBUG_OUTPUT_NAME "OCCDevelop_d")
    set_target_properties(OCCDevelop PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/OCCDevelop)
    set_target_properties(OCCDevelop PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(OCCDevelop PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/OCCDevelop)
    set_target_properties(OCCDevelop PROPERTIES PREFIX "")
    #set_target_properties(Fem PROPERTIES INSTALL_RPATH ${INSTALL_RPATH})
endif(MSVC)

install(TARGETS OCCDevelop DESTINATION lib)
