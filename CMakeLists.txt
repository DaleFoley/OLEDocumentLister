cmake_minimum_required(VERSION 3.10)
project(OLEDocumentLister)

set(CMAKE_CXX_STANDARD 11)

include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(/usr/include/libgsf-1)
include_directories(/usr/include/libxml2)

add_executable(OLEDocumentLister src/main.cpp)

IF (LIBGSF_INCLUDE_DIR AND LIBGSF_LIBRARIES)
    # in cache already
    SET(LIBGSF_FIND_QUIETLY TRUE)
ENDIF (LIBGSF_INCLUDE_DIR AND LIBGSF_LIBRARIES)

IF (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    FIND_PACKAGE(PkgConfig)
    PKG_CHECK_MODULES(PC_LIBGSF libgsf-1)
    SET(LIBGSF_DEFINITIONS ${PC_LIBGSF_CFLAGS_OTHER})
ENDIF (NOT WIN32)

FIND_PATH(LIBGSF_INCLUDE_DIR gsf/gsf.h
        HINTS
        ${PC_LIBGSF_INCLUDEDIR}
        ${PC_LIBGSF_INCLUDE_DIRS}
        PATH_SUFFIXES libgsf-1
        )

FIND_LIBRARY(LIBGSF_LIBRARIES NAMES gsf-1 libgsf-1
        HINTS
        ${PC_LIBGSF_LIBDIR}
        ${PC_LIBGSF_LIBRARY_DIRS}
        )

FIND_PROGRAM(LIBGSF_GSF_EXECUTABLE gsf)
FIND_PROGRAM(LIBGSF_GSFOFFICETHUMBNAILER_EXECUTABLE gsf-office-thumbnailer)
FIND_PROGRAM(LIBGSF_GSFVBADUMP_EXECUTABLE gsf-vba-dump)

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBGSF_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBGSF DEFAULT_MSG LIBGSF_LIBRARIES LIBGSF_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBGSF_INCLUDE_DIR LIBGSF_LIBRARIES LIBGSF_GSF_EXECUTABLE LIBGSF_GSFOFFICETHUMBNAILER_EXECUTABLE LIBGSF_GSFVBADUMP_EXECUTABLE )

target_link_libraries(OLEDocumentLister ${GLIB_LIBRARIES})
target_link_libraries(OLEDocumentLister /usr/lib/x86_64-linux-gnu/libgobject-2.0.so)
target_link_libraries(OLEDocumentLister /usr/lib/x86_64-linux-gnu/libgsf-1.so)

