# ove2xml
# Copyright (C) 2010-2011 Rui Fan <vanferry@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along 
# with this program; If not, see <http://www.gnu.org/licenses/>.

PROJECT(ove2xml)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Show full commands and messages. 
# SET(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL)

# Seems that relative paths break the link scripts. Don't set it ON
SET(CMAKE_USE_RELATIVE_PATHS OFF)

# Assorted settings
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)
SET(CMAKE_SKIP_RULE_DEPENDENCY FALSE)
SET(CMAKE_CXX_FLAGS_DEBUG "-g -DQT_DEBUG -DDEBUG -D__RTMIDI_DEBUG__")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DQT_NO_DEBUG -DNDEBUG")

# Additional CMake modules for 3rd party library checks reside here
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_admin")

# ove2xml version string
SET (VERSION_MAJOR "0")
SET (VERSION_MINOR "6")
SET (VERSION_PATCH "")
SET (VERSION_SUFFIX "")
SET (VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_SUFFIX})

MESSAGE(STATUS "ove2xml ${VERSION} prefix: ${CMAKE_INSTALL_PREFIX}")

# User options
MESSAGE(STATUS "Build configuration: ${CMAKE_BUILD_TYPE}")

# Check for Qt
SET(QT_MIN_VERSION "4.4.0")
FIND_PACKAGE(Qt4 REQUIRED QUIET)
IF(QT4_FOUND)
    MESSAGE(STATUS "Found Qt4.${QT_EDITION} version = ${QTVERSION}")
ELSE(QT4_FOUND)
    MESSAGE(FATAL_ERROR "Qt4 >= 4.4 not found")
ENDIF(QT4_FOUND)

SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${QT_INCLUDES}) 
LINK_DIRECTORIES(${QT_LIBRARY_DIR})
LINK_LIBRARIES(${QT_LIBRARIES})
ADD_DEFINITIONS(-DQT_STRICT_ITERATORS)

IF(${CMAKE_SYSTEM} MATCHES "Linux")
# Check for PKG-CONFIG
    FIND_PACKAGE(PkgConfig REQUIRED)
    IF(PKG_CONFIG_FOUND)
        MESSAGE(STATUS "Program pkg-config found (${PKG_CONFIG_EXECUTABLE})")
    ELSE(PKG_CONFIG_FOUND)
        MESSAGE(FATAL_ERROR "Program pkg-config not found")
    ENDIF(PKG_CONFIG_FOUND)
ENDIF(${CMAKE_SYSTEM} MATCHES "Linux")

IF(${CMAKE_SYSTEM} MATCHES "Darwin")
    ADD_DEFINITIONS(-D__MACOSX_CORE__)
    LINK_LIBRARIES("-framework CoreFoundation -framework Carbon")
ENDIF(${CMAKE_SYSTEM} MATCHES "Darwin")

IF(${CMAKE_SYSTEM} MATCHES "Windows")
    FIND_PACKAGE(WINDRES REQUIRED)
    ADD_DEFINITIONS(-D__WINDOWS_MM__)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
ENDIF(${CMAKE_SYSTEM} MATCHES "Windows")

ADD_SUBDIRECTORY (src)
CONFIGURE_FILE (ove2xml.spec.in ove2xml.spec IMMEDIATE @ONLY)

# uninstall custom target
CONFIGURE_FILE(
    "${CMAKE_SOURCE_DIR}/cmake_admin/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET ( uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

IF (UNIX)
    ADD_CUSTOM_TARGET ( distclean
        COMMAND rm -rf build/*
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    ADD_DEPENDENCIES (distclean clean)
    
    ADD_CUSTOM_TARGET ( tarball
        COMMAND cp -r cmake_admin ove2xml-${VERSION}
        COMMAND cp -r images ove2xml-${VERSION}
        COMMAND cp -r src ove2xml-${VERSION}
        COMMAND cp AUTHORS ChangeLog CMakeLists.txt COPYING gpl.rtf NEWS README setup.nsi TODO ove2xml.desktop ove2xml.pro updateqm.pri ove2xml.spec.in qt.conf ove2xml-${VERSION}
        COMMAND tar -cj --exclude=.* -f ove2xml-${VERSION}.tar.bz2 ove2xml-${VERSION}
        COMMAND tar -cz --exclude=.* -f ove2xml-${VERSION}.tar.gz ove2xml-${VERSION}
        COMMAND rm -rf ove2xml-${VERSION}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
ENDIF (UNIX)

IF (UNIX AND NOT APPLE)

    INSTALL(FILES images/ove2xml_16x16.png 
        DESTINATION share/icons/hicolor/16x16/apps   
        RENAME ove2xml.png)
    
    INSTALL(FILES images/ove2xml_32x32.png 
        DESTINATION share/icons/hicolor/32x32/apps   
        RENAME ove2xml.png)
    
    INSTALL(FILES images/ove2xml_48x48.png 
        DESTINATION share/icons/hicolor/48x48/apps   
        RENAME ove2xml.png)
    
    INSTALL ( FILES 
        ove2xml.desktop
    DESTINATION 
        share/applications
    )

ENDIF (UNIX AND NOT APPLE)

IF (WIN32)
    INCLUDE (InstallRequiredSystemLibraries)
    SET ( QTLIBLIST QtCore QtGui QtXml )
    GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
    FOREACH(qtlib ${QTLIBLIST})
        INSTALL ( FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                  DESTINATION . )
    ENDFOREACH(qtlib)
    INSTALL ( FILES ${QT_DLL_PATH_tmp}/mingwm10.dll 
                    ${QT_DLL_PATH_tmp}/libgcc_s_dw2-1.dll
              DESTINATION . )
ENDIF (WIN32)

IF (APPLE)
    SET(target "ove2xml")
    CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake_admin/CompleteBundle.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/build/CompleteBundle.cmake" @ONLY IMMEDIATE)
    CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake_admin/CreateBundle.sh.in"
      "${CMAKE_CURRENT_BINARY_DIR}/build/CreateBundle.sh" @ONLY IMMEDIATE)
    INSTALL(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/build/CompleteBundle.cmake")
ENDIF (APPLE)
