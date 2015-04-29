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

INCLUDE_DIRECTORIES ( 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ove
    ${CMAKE_CURRENT_SOURCE_DIR}/xml
)

LINK_DIRECTORIES (
    ${CMAKE_CURRENT_BINARY_DIR}
)

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ove2xml_version.h
     "const QString PGM_VERSION(\"${VERSION}\");\n")

SET(ove2xml-gui_forms_SRCS
    app/About.ui
    app/ConvertWindow.ui
    app/FileBrowser.ui
)

QT4_WRAP_UI(ove2xml-gui_ui_SRCS ${ove2xml-gui_forms_SRCS})

SET(ove2xml-gui_SRCS
    app/About.cpp
    app/About.h
    app/ConvertWindow.cpp
    app/ConvertWindow.h
    app/ConvertXmlThread.cpp
    app/ConvertXmlThread.h
    app/CoreData.cpp
    app/CoreData.h
    app/FileBrowser.cpp
    app/FileBrowser.h
    app/LoadOveThread.cpp
    app/LoadOveThread.h
    app/main-gui.cpp
    app/OveConvertor.cpp
    app/OveConvertor.h
    ove/ove.cpp
    ove/ove.h
    xml/IXmlNotify.h
    xml/XmlBuffer.cpp
    xml/XmlBuffer.h
    xml/XmlSerialize.cpp
    xml/XmlSerialize.h
    xml/XmlWriter.cpp
    xml/XmlWriter.h
    xml/XmlWriterFactory.cpp
    xml/XmlWriterFactory.h
    xml/XmlWriterMeasure.cpp
    xml/XmlWriterMeasure.h
    xml/XmlWriterPtr.h
    xml/XmlWriterStructure.cpp
    xml/XmlWriterStructure.h
)

SET(ove2xml_SRCS
    app/CoreData.cpp
    app/CoreData.h
    app/main.cpp
    app/OveConvertor.cpp
    app/OveConvertor.h
    ove/ove.cpp
    ove/ove.h
    xml/IXmlNotify.h
    xml/XmlBuffer.cpp
    xml/XmlBuffer.h
    xml/XmlSerialize.cpp
    xml/XmlSerialize.h
    xml/XmlWriter.cpp
    xml/XmlWriter.h
    xml/XmlWriterFactory.cpp
    xml/XmlWriterFactory.h
    xml/XmlWriterMeasure.cpp
    xml/XmlWriterMeasure.h
    xml/XmlWriterPtr.h
    xml/XmlWriterStructure.cpp
    xml/XmlWriterStructure.h
)

QT4_ADD_RESOURCES(ove2xml-gui_SRCS ../images/ove2xml.qrc)
#QT4_ADD_RESOURCES(ove2xml_SRCS ../images/ove2xml.qrc)

SET(ove2xml-gui_qtobject_SRCS
    app/About.h
    app/ConvertWindow.h
    app/ConvertXmlThread.h
    app/FileBrowser.h
    app/LoadOveThread.h
)

QT4_WRAP_CPP(ove2xml-gui_moc_SRCS ${ove2xml-gui_qtobject_SRCS})

IF (UNIX AND NOT APPLE)
    ADD_EXECUTABLE ( ove2xml-gui
        ${ove2xml-gui_ui_SRCS}
        ${ove2xml-gui_moc_SRCS}
        ${ove2xml-gui_SRCS} )
    INSTALL ( TARGETS ove2xml-gui
              RUNTIME DESTINATION bin )
    ADD_EXECUTABLE ( ove2xml
        ${ove2xml_SRCS} )
    INSTALL ( TARGETS ove2xml
              RUNTIME DESTINATION bin )
ENDIF (UNIX AND NOT APPLE)

IF (WIN32)
    ADD_WINDRES_OBJS(ove2xml_SRCS ../images/ove2xml.rc)
	ADD_WINDRES_OBJS(ove2xml-gui_SRCS ../images/ove2xml.rc)
    ADD_EXECUTABLE ( ove2xml-gui WIN32
        ${ove2xml-gui_ui_SRCS}
        ${ove2xml-gui_moc_SRCS}
        ${ove2xml-gui_SRCS} )
    INSTALL ( TARGETS ove2xml-gui
              RUNTIME DESTINATION . )
    ADD_EXECUTABLE ( ove2xml
        ${ove2xml_SRCS} )
    INSTALL ( TARGETS ove2xml
              RUNTIME DESTINATION . )
ENDIF (WIN32)

IF (APPLE)
    SET(MACOSX_BUNDLE_INFO_STRING "ove2xml" )
    SET(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION} )
    SET(MACOSX_BUNDLE_ICON_FILE "ove2xml.icns" )
    SET(MACOSX_BUNDLE_GUI_IDENTIFIER "net.sourceforge" )
    SET(MACOSX_BUNDLE_BUNDLE_NAME "ove2xml" )
    SET(ove2xml_RSC ../images/ove2xml.icns)
    SET(ove2xml-gui_RSC ../images/ove2xml.icns)
    SET_SOURCE_FILES_PROPERTIES(${ove2xml_RSC} PROPERTIES
                                MACOSX_PACKAGE_LOCATION Resources)
    ADD_EXECUTABLE ( ove2xml-gui MACOSX_BUNDLE
        ${ove2xml-gui_ui_SRCS}
        ${ove2xml-gui_moc_SRCS}
        ${ove2xml-gui_SRCS}
        ${ove2xml-gui_RSC} )
    ADD_EXECUTABLE ( ove2xml MACOSX_BUNDLE
        ${ove2xml_SRCS}
        ${ove2xml_RSC} )
ENDIF (APPLE)
