//=============================================================================
//  ove2midi
//  Overture Document to Midi file convertor
//
//  Copyright (C) 2010-2010 Rui Fan <vanferry@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "OveConvertor.h"

#include "CoreData.h"
#include "XmlSerialize.h"
#include <stdlib.h>
#include <QDir>
#include <QDate>
#include <QFile>
#include <QFileInfo>

char* toString(const QString& str) {
#if defined(Q_OS_WIN32)
    return str.toLocal8Bit().data();
#else
    return str.toUtf8().data();
#endif
}

static QString checkExists(const QString& fromFile, const QString& toFile) {
    QFileInfo info(fromFile);
    if(!info.exists()) {
        printf("The input file is not exist!\n");
        exit(-1);
    } else if (info.suffix() != "ove") {
        printf("Please input an ove file to convert!\n");
        exit(-1);
    } else {
        QString to;
        if(toFile.isEmpty()) {
            to = info.path() + QDir::separator() + info.completeBaseName() + ".mid";
        } else {
            QFileInfo toInfo(toFile);
            QDir toDir(toInfo.absolutePath());
            if(!toDir.exists()) {
                printf("Output file path not exist.\n");
                exit(-1);
            }
            to = toFile;
        }

        return to;
    }

    return QString();
}

OveConvertor::OveConvertor() {
}

OveConvertor::~OveConvertor() {
}

bool OveConvertor::convert(const QString& from, const QString& to) {
    OVE::OveSong* ove = CoreData::get()->getOveSong();

    QString toFile = checkExists(from, to);

    // load ove file
    QFile oveFile(from);

    if (!oveFile.open(QFile::ReadOnly)) {
        printf("load ove file \"%s\" failed.\n", toString(from));
        return false;
    }

    if(to != "") {
        printf("Converting: ");
        printf(toString(from));
        printf(" to ");
        printf(toString(toFile));
        printf("\n");
    }

    QByteArray buffer = oveFile.readAll();

    oveFile.close();
    ove->clear();

    OVE::IOVEStreamLoader* oveLoader = OVE::createOveStreamLoader();
    oveLoader->setOve(ove);
    oveLoader->setNotify(this);
    oveLoader->setFileStream((unsigned char*) buffer.data(), buffer.size());
    bool result = oveLoader->load();
    oveLoader->release();

    if(!result) {
        return false;
    }

    // OveData -> xml
    QString encodeDate = QDate::currentDate().toString(Qt::ISODate);
    XML::XmlSerialize xmlSerialize;
    xmlSerialize.setOveSong(ove);
    xmlSerialize.setDate(encodeDate);
    //xmlSerialize.setNotify(xmlListener_);

    if(to == "") {
        printf(toString(xmlSerialize.toString()));
    } else {
        result = xmlSerialize.save(to);

        QString str = result ? QString("Saved as " + to) : QString("Error when creating xml file!");

        if(result) {
            printf("Saved as %s\n", toString(to));
            printf("Succeed!\n");
        } else {
            printf("Error when creating xml file: %s", toString(to));
        }
    }

    return true;
}

void OveConvertor::loadInfo(const QString& info) {
    printf(toString(info));
}

void OveConvertor::loadError() {
	printf("Encounter an error and abort.\n");
}

void OveConvertor::loadPosition(int /*currentMeasure*/, int /*totalMeasure*/, int /*currentTrack*/, int /*totalTrack*/) {
}
