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
#include <QCoreApplication>
#include <stdio.h>
#include <stdlib.h>
#include "CoreData.h"
#include "ConvertWindow.h"

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(ove2xml);

    QApplication app(argc, argv);

    // set local file system codc
    QTextCodec* tc = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(tc);

    //---------------------------------------------------------
    //   convert ove file to midi file
    //---------------------------------------------------------
    try {
        CoreData::createInstance();
        CoreData::get()->init();

        ConvertWindow convertWindow;
        convertWindow.show();
        app.exec();

        CoreData::get()->exit();
        CoreData::destroyInstance();
    } catch (...) {
    }

    return 0;
}
