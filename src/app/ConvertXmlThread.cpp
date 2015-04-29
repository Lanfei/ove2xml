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

#include "ConvertXmlThread.h"

#include <QDate>
#include <QFileInfo>
#include <QStringList>
#include "XmlSerialize.h"

class XmlSerializeNotify: public IXmlNotify {
public:
	XmlSerializeNotify(ConvertXmlThread* receiver) {
		receiver_ = receiver;
	}
	virtual ~XmlSerializeNotify() {}

private:
	ConvertXmlThread* receiver_;

public:
	virtual void notify_convert_pos(int measure, int totalMeasure, int part, int totalPart) {
		receiver_->xmlConvertNotify(measure, totalMeasure, part, totalPart);
	}
	;
};

///////////////////////////////////////////////////////////////////////////////
ConvertXmlThread::ConvertXmlThread(QObject* parent) :
	QThread(parent) {
	xmlListener_ = new XmlSerializeNotify(this);
	filePath_ = QString();
}

ConvertXmlThread::~ConvertXmlThread() {
}

void ConvertXmlThread::setOveData(OVE::OveSong* ove) {
	ove_ = ove;
}

void ConvertXmlThread::setOveFile(const QString &fileName) {
	filePath_ = fileName;
}

QString ConvertXmlThread::getXmlFilePath() {
	QFileInfo info(filePath_);
	QString midiFile = info.absolutePath() + "/" + info.completeBaseName() + ".xml";
	//	QString midiFile = QString("test.xml") ;

	return midiFile;
}

void ConvertXmlThread::run() {
	if (ove_ == 0)
		return;

	emit convertProgressSignal(50, 100);

	// OveData -> xml
	QString encodeDate = QDate::currentDate().toString(Qt::ISODate);
	XML::XmlSerialize xmlSerialize;
	xmlSerialize.setOveSong(ove_);
	xmlSerialize.setDate(encodeDate);
	xmlSerialize.setNotify(xmlListener_);

	QString filePath = getXmlFilePath();

	bool result = xmlSerialize.save(filePath);

	QString str = result ? QString("Saved as " + filePath) : QString("Error when creating xml file!");

	messageOutString(str);
}

void ConvertXmlThread::xmlConvertNotify(int measure, int totalMeasure, int part, int totalPart) {
	double step = double(totalMeasure * part + measure) / (double) (totalMeasure * totalPart);

	emit convertProgressSignal(int(step * 50.0) + 50, 100);
}

void ConvertXmlThread::messageOutString(const QString& str) {
	QStringList strs;
	strs += str;

	emit convertInfoSignal(strs);
}

void ConvertXmlThread::messageOutStrings(const QStringList& strs) {
	emit convertInfoSignal(strs);
}

QPair<int, int> ConvertXmlThread::getProgress(int type) {
	int step = 0;
	int totalStep = 100;

	if (type == 1) //ove
	{
		step = 0;
		totalStep = 100;
	} else if (type == 2) {
		step = 40;
		totalStep = 100;
	} else if (type == 3) {
		step = 50;
		totalStep = 100;
	}

	emit convertProgressSignal(step, totalStep);

	return qMakePair(step, totalStep);
}
