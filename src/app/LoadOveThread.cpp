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

#include "LoadOveThread.h"

#include <QFile>
#include <QStringList>
#include "ove.h"

class OveLoaderNotify: public OVE::IOveNotify {
public:
	OveLoaderNotify(LoadOveThread* receiver) {
		receiver_ = receiver;
	}

	virtual ~OveLoaderNotify() {}

public:
	virtual void loadInfo(const QString& info) {
		receiver_->oveLoadInfo(info);
	}

	virtual void loadError() {
		receiver_->oveLoadError();
	}

	virtual void loadPosition(int currentMeasure, int totalMeasure, int currentTrack, int totalTrack) {
		receiver_->oveLoadPosition(currentMeasure, totalMeasure, currentTrack, totalTrack);
	}

private:
	LoadOveThread* receiver_;
};

///////////////////////////////////////////////////////////////////////////////
LoadOveThread::LoadOveThread(QObject * parent) :
	QThread(parent) {
	ove_ = NULL;
	oveListener_ = new OveLoaderNotify(this);
	filePath_ = QString();
}

LoadOveThread::~LoadOveThread() {
}

void LoadOveThread::setOveFile(const QString& filePath) {
	filePath_ = filePath;
}

void LoadOveThread::setOveSong(OVE::OveSong* ove) {
	ove_ = ove;
}

void LoadOveThread::oveLoadInfo(const QString& info) {
	messageOutString(info);
}

void LoadOveThread::oveLoadError() {
	emit loadErrorSignal();
}

void LoadOveThread::oveLoadPosition(int currentMeasure, int totalMeasure, int currentTrack, int totalTrack) {
	double step = double(totalMeasure * currentTrack + currentMeasure) / (double) (totalMeasure * totalTrack);

	emit loadProgressSignal(int(step * 50.0), 100);
}

void LoadOveThread::run() {
	if (ove_ == NULL)
		return;

	QStringList strs;
	strs << QString::fromLocal8Bit("////////////////////////////////////////////////////////////////////////////////////////");
	strs << QString::fromLocal8Bit("Loading... : ") + filePath_;
	messageOutStrings(strs);

	QFile oveFile(filePath_);

	if (!oveFile.open(QFile::ReadOnly)) {
		messageOutString(QString("can't read file!"));

		return;
	}

	QByteArray buffer = oveFile.readAll();

	oveFile.close();
	ove_->clear();

	// ove -> OveData

	OVE::IOVEStreamLoader* oveLoader = OVE::createOveStreamLoader();

	oveLoader->setOve(ove_);
	oveLoader->setFileStream((unsigned char*) buffer.data(), buffer.size());
	oveLoader->setNotify(oveListener_);
	bool result = oveLoader->load();
	oveLoader->release();

	if (!result) {
		return;
	}
}

void LoadOveThread::messageOutString(const QString& str) {
	QStringList strs;
	strs += str;

	emit loadInfoSignal(strs);
}

void LoadOveThread::messageOutStrings(const QStringList& strs) {
	emit loadInfoSignal(strs);
}
