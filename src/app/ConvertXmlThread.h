//=============================================================================
//  Ove2Xml
//  Overture Document To MusicXml Document Convertor
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

#ifndef CONVERT_XML_THREAD_H
#define CONVERT_XML_THREAD_H

#include <QThread>
#include "IXmlNotify.h"
#include "ove.h"

// convert OveData to MusicXml
class ConvertXmlThread: public QThread {
	Q_OBJECT

public:
	ConvertXmlThread(QObject * parent = 0);
	virtual ~ConvertXmlThread();

	// ove -> OveData -> xml
public:
	void setOveData(OVE::OveSong* ove);
	void setOveFile(const QString &fileName);

signals:
	void convertInfoSignal(const QStringList& infos);
	void convertProgressSignal(int step, int totalStep);

private:
	friend class XmlSerializeNotify;
	void xmlConvertNotify(int measure, int totalMeasure, int part, int totalPart);

private:
	void run();

	QString getXmlFilePath();

	void messageOutString(const QString& str);
	void messageOutStrings(const QStringList& strs);

	QPair<int, int> getProgress(int type);

private:
	IXmlNotify* xmlListener_;
	QString filePath_;
	OVE::OveSong* ove_;
};

#endif
