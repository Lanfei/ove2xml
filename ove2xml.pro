#=============================================================================
#  Overture Document Library
#
#  Copyright (C) 2010-2011 Rui Fan <vanferry@gmail.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#=============================================================================

TEMPLATE = app
QT += xml
CONFIG += warn_on console debug
TARGET = ove2xml

#///////////////////////////////////////////////////////////////////////////////
# OVE

INCLUDEPATH += \
    ./src/ove \

HEADERS += \
    ./src/ove/ove.h \

SOURCES += \
    ./src/ove/ove.cpp \

#///////////////////////////////////////////////////////////////////////////////
# XML
INCLUDEPATH += \
    ./src/xml \

HEADERS += \
    ./src/xml/IXmlNotify.h \
    ./src/xml/XmlSerialize.h \
    ./src/xml/XmlBuffer.h \
    ./src/xml/XmlWriter.h \
    ./src/xml/XmlWriterFactory.h \
    ./src/xml/XmlWriterMeasure.h \
    ./src/xml/XmlWriterPtr.h \
    ./src/xml/XmlWriterStructure.h \

SOURCES += \
    ./src/xml/XmlBuffer.cpp \
    ./src/xml/XmlSerialize.cpp \
    ./src/xml/XmlWriter.cpp \
    ./src/xml/XmlWriterFactory.cpp \
    ./src/xml/XmlWriterMeasure.cpp \
    ./src/xml/XmlWriterStructure.cpp \
	
#///////////////////////////////////////////////////////////////////////////////

HEADERS += \
    ./src/app/CoreData.h \
    ./src/app/OveConvertor.h \

SOURCES += \
    ./src/app/CoreData.cpp \
    ./src/app/OveConvertor.cpp \
    ./src/app/main.cpp \

RESOURCES += images/ove2xml.qrc

win32:RC_FILE = images/ove2xml.rc
#macx:ICON = app.icns
