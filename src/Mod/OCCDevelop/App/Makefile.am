
lib_LTLIBRARIES=libOCCDevelop.la OCCDevelop.la

libOCCDevelop_la_SOURCES=\
		AppOCCDevelopPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h

includedir = @includedir@/Mod/OCCDevelop/App

# the library search path.
libOCCDevelop_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) \
		-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libOCCDevelop_la_CPPFLAGS = -DOCCDevelopAppExport=

libOCCDevelop_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp

#--------------------------------------------------------------------------------------
# Loader of libOCCDevelop

OCCDevelop_la_SOURCES=\
		AppOCCDevelop.cpp

# the library search path.
OCCDevelop_la_LDFLAGS = $(libOCCDevelop_la_LDFLAGS) -module -avoid-version
OCCDevelop_la_CPPFLAGS = $(libOCCDevelop_la_CPPFLAGS)

OCCDevelop_la_LIBADD   = \
		$(libOCCDevelop_la_LIBADD) \
		-lOCCDevelop

OCCDevelop_la_DEPENDENCIES = libOCCDevelop.la

#--------------------------------------------------------------------------------------

# set the include path found by configure
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes)

libdir = $(prefix)/Mod/OCCDevelop

EXTRA_DIST = \
		CMakeLists.txt
