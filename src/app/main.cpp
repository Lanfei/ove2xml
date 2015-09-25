//=============================================================================
//  ove2xml
//  Overture Document To MusicXML Document Convertor
//
//  Copyright (C) 2010-2011 Rui Fan <vanferry@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//=============================================================================

#include <QApplication>

#include <stdio.h>
#include <stdlib.h>
#include "CoreData.h"
#include "OveConvertor.h"

//---------------------------------------------------------
//   usage
//---------------------------------------------------------

static void usage() {
    printf("ove2xml version 0.6\n");
    printf("usage: ove2xml -c ove_file_with_path -o xml_file_with_path\n");
    exit(-1);
}

int main(int argc, char *argv[]) {
    //Q_INIT_RESOURCE(ove2xml);

    // set local file system codc
    //QTextCodec* tc = QTextCodec::codecForName("utf8");
    //QTextCodec::setCodecForLocale(tc);

    QApplication app(argc, argv);

    /////////////////////////////////////////////
    QString fromFile;
    QString toFile;
    QStringList argvs =  QCoreApplication::arguments();
    argvs.removeFirst();

    for (int i = 0; i < argvs.size();) {
        QString s = argvs[i];
        if (s[0] != '-') {
            ++i;
            continue;
        }

        switch (s[1].unicode()) {
            case 'c':
                if (argvs.size() - i < 2)
                    usage();
                fromFile = argvs.takeAt(i + 1);
                fromFile = fromFile.trimmed();
                break;
            case 'o':
                if (argvs.size() - i < 2)
                    usage();
                toFile = argvs.takeAt(i + 1);
                toFile = toFile.trimmed();
                break;
        case 'h':
        case 'v':
            usage();
            break;
        default:
            usage();
        }

        argvs.removeAt(i);
    }

    //---------------------------------------------------------
    //   convert ove file to midi file
    //---------------------------------------------------------
    try {
        CoreData::createInstance();
        CoreData::get()->init();

        OveConvertor c;
        c.convert(fromFile, toFile);

        CoreData::get()->exit();
        CoreData::destroyInstance();
    } catch (...) {

    }

    return 0;
}
