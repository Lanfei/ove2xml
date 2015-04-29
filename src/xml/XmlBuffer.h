//=============================================================================
//  Overture Document To MusicXml Library
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

#ifndef XML_BUFFER_H
#define XML_BUFFER_H

#include <QList>
#include <QString>
#include "ove.h"

using namespace OVE;
class IXmlNotify;

namespace XML {

class XmlBuffer {
public:
	XmlBuffer();
	~XmlBuffer();

public:
	void setOve(OveSong* ove);
	OveSong* getOve() const;

	void organize();

	void setEncodeDate(const QString& date);
	QString getEncodeDate() const;

	void setNotify(IXmlNotify* notify);
	IXmlNotify* getNotify() const;

	// layout
	double getScalingMillimeters() const;
	int getLayoutTenths(int unit) const;

	enum MusicXMlVersion {
		Xml_1_0 = 0, Xml_1_1, Xml_2_0
	};
	void setXmlVersion(MusicXMlVersion version = Xml_1_1);
	MusicXMlVersion getXmlVersion() const;

	enum FinaleVersion {
		Finale_2007 = 0, Finale_2008, Finale_2009
	};
	void setFinaleVersion(FinaleVersion version = Finale_2008);
	FinaleVersion getFinaleVersion() const;

	// slur, 
	int calculateStartStopNumber(MusicData* ptr, bool start);
	void clearStartStopDatas();

private:
	IXmlNotify* notify_;
	OveSong* ove_;
	QString encodeDate_;
	MusicXMlVersion xmlVersion_;
	FinaleVersion finaleVersion_;
	QList<QPair<MusicData*, int> > nonStopedDatas_; // for start-stop MusicData
};

}

#endif
