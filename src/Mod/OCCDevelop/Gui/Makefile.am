
lib_LTLIBRARIES=libOCCDevelopGui.la OCCDevelopGui.la

BUILT_SOURCES=

libOCCDevelopGui_la_SOURCES=\
		AppOCCDevelopGuiPy.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

includedir = @includedir@/Mod/OCCDevelop/Gui

# the library search path.
libOCCDevelopGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App \
		$(QT_LIBS) $(all_libraries) \
		$(sim_ac_coin_ldflags) $(sim_ac_coin_libs) \
		$(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) \
		-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libOCCDevelopGui_la_CPPFLAGS = -DOCCDevelopAppExport= -DOCCDevelopGuiExport=

libOCCDevelopGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lOCCDevelop

#--------------------------------------------------------------------------------------
# Loader of libOCCDevelopGui

OCCDevelopGui_la_SOURCES=\
		AppOCCDevelopGui.cpp

# the library search path.
OCCDevelopGui_la_LDFLAGS = $(libOCCDevelopGui_la_LDFLAGS) -module -avoid-version
OCCDevelopGui_la_CPPFLAGS = $(libOCCDevelopGui_la_CPPFLAGS)

OCCDevelopGui_la_LIBADD   = \
		$(libOCCDevelopGui_la_LIBADD) \
		-lOCCDevelopGui

OCCDevelopGui_la_DEPENDENCIES = libOCCDevelopGui.la

#--------------------------------------------------------------------------------------

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $< -o $(@F)

# rules for Qt User Interface Compiler:
ui_%.h: %.ui
	$(QT_UIC) $< -o $(@F)

# rules for Qt Resource Compiler:
qrc_%.cpp: Resources/%.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)

# set the include path found by configure
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS) \
		-I$(sim_ac_coin_includedir) -I$(sim_ac_soqt_includedir)

libdir = $(prefix)/Mod/OCCDevelop

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		CMakeLists.txt \
		Resources/OCCDevelop.qrc

