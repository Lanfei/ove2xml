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

#ifndef LOAD_OVE_THREAD_H
#define LOAD_OVE_THREAD_H

#include <QThread>

namespace OVE {
class OveSong;
class IOveNotify;
}

// load ove file to OveData
class LoadOveThread: public QThread {
	Q_OBJECT

public:
	LoadOveThread(QObject * parent = 0);
	virtual ~LoadOveThread();

public:
	void setOveFile(const QString& filePath);
	void setOveSong(OVE::OveSong* ove);

signals:
	void loadInfoSignal(const QStringList& infos);
	void loadProgressSignal(int step, int totalStep);
	void loadErrorSignal();

private:
	friend class OveLoaderNotify;
	void oveLoadInfo(const QString& info);
	void oveLoadError();
	void oveLoadPosition(int currentMeasure, int totalMeasure, int currentTrack, int totalTrack);

	virtual void run();
	void messageOutString(const QString& str);
	void messageOutStrings(const QStringList& strs);

private:
	OVE::IOveNotify* oveListener_;
	QString filePath_;
	OVE::OveSong* ove_;
};

#endif
