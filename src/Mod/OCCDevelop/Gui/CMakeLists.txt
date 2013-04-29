SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${COIN3D_INCLUDE_DIR}
    ${OCC_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${SOQT_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${XERCESC_INCLUDE_DIR}
)

set(OCCDevelopGui_LIBS
    OCCDevelop
    FreeCADGui
    Part
)

qt4_add_resources(OCCDevelop_QRC_SRCS Resources/OCCDevelop.qrc)

SET(DBus_MOC_HDRS
    DBus/dbusserver.h
    DBus/dbusserver_impl.h
)
qt4_wrap_cpp(DBus_MOC_SRCS ${DBus_MOC_HDRS})

SET(OCCDevelopGui_SRCS
    ${OCCDevelop_QRC_SRCS}
    AppOCCDevelopGui.cpp
    AppOCCDevelopGuiPy.cpp
    Command.cpp
    PreCompiled.cpp
    PreCompiled.h
    Workbench.cpp
    Workbench.h
    ${DBus_MOC_HDRS}
    ${DBus_MOC_SRCS}
    DBus/dbusserver.cpp
    DBus/dbusserver_impl.cpp
    DisectBrep.h
    DisectBrep.cpp
)

add_library(OCCDevelopGui SHARED ${OCCDevelopGui_SRCS})
target_link_libraries(OCCDevelopGui ${OCCDevelopGui_LIBS})

fc_target_copy_resource(OCCDevelopGui 
    ${CMAKE_SOURCE_DIR}/src/Mod/OCCDevelop
    ${CMAKE_BINARY_DIR}/Mod/OCCDevelop
    InitGui.py)

if(MSVC)
    set_target_properties(OCCDevelopGui PROPERTIES SUFFIX ".pyd")
    set_target_properties(OCCDevelopGui PROPERTIES DEBUG_OUTPUT_NAME "OCCDevelopGui_d")
    set_target_properties(OCCDevelopGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/OCCDevelop)
    set_target_properties(OCCDevelopGui PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(OCCDevelopGui PROPERTIES SUFFIX ".pyd")
    set_target_properties(OCCDevelopGui PROPERTIES DEBUG_OUTPUT_NAME "OCCDevelopGui_d")
    set_target_properties(OCCDevelopGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/OCCDevelop)
    set_target_properties(OCCDevelopGui PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(OCCDevelopGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/OCCDevelop)
    set_target_properties(OCCDevelopGui PROPERTIES PREFIX "")
    #set_target_properties(Fem PROPERTIES INSTALL_RPATH ${INSTALL_RPATH})
endif(MSVC)

install(TARGETS OCCDevelopGui DESTINATION lib)
