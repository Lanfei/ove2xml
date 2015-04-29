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

#include "XmlBuffer.h"

namespace XML {

const double TENTHS = 8.0000;

XmlBuffer::XmlBuffer() :
	notify_(NULL), xmlVersion_(Xml_1_1), finaleVersion_(Finale_2008) {
}

XmlBuffer::~XmlBuffer() {
	notify_ = NULL;
}

void XmlBuffer::setOve(OveSong* ove) {
	ove_ = ove;
}

OveSong* XmlBuffer::getOve() const {
	return ove_;
}

int getVoiceOffset(const QList<int>& voices, int staff) {
	int voiceOffset = 0;
	for(int i=0; i<staff && i<voices.size(); ++i) {
		voiceOffset += voices[i];
	}

	return voiceOffset;
}

void XmlBuffer::organize() {
	int i;
	int j;
	int k;
	int l;
	int trackBarCount = ove_->getTrackBarCount();

	for(i=0; i<ove_->getPartCount(); ++i) {
		int partStaffCount = ove_->getStaffCount(i);
		QList<int> voices;

		for(j=0; j<partStaffCount; ++j) {
			int maxVoice = 0;
			for(k=0; k<trackBarCount; ++k) {
				//Measure* measure = ove_->getMeasure(k);
				MeasureData* measureData = ove_->getMeasureData(i, j, k);
				QList<NoteContainer*> containers = measureData->getNoteContainers();

				for(l=0; l<containers.size(); ++l){
					maxVoice = qMax(maxVoice, (int)containers[l]->getVoice());
				}
			}

            voices.push_back(maxVoice+1);
		}

		for(j=0; j<partStaffCount; ++j) {
			for(k=0; k<trackBarCount; ++k) {
				//Measure* measure = ove_->getMeasure(k);
				MeasureData* measureData = ove_->getMeasureData(i, j, k);
				QList<NoteContainer*> containers = measureData->getNoteContainers();

				for(l=0; l<containers.size(); ++l){
					int v = containers[l]->getVoice() + getVoiceOffset(voices, j);
					containers[l]->setXmlVoice(v);
				}
			}
		}
	}
}

void XmlBuffer::setNotify(IXmlNotify* notify) {
	notify_ = notify;
}

IXmlNotify* XmlBuffer::getNotify() const {
	return notify_;
}

double XmlBuffer::getScalingMillimeters() const {
	return TENTHS;
}

int XmlBuffer::getLayoutTenths(int unit) const {
	int millimeter = int((((double) unit * 25.4) / 14.0) / 24.0);
	return millimeter * 5; // 5 == 40 / TENTHS(8)
}

void XmlBuffer::setEncodeDate(const QString& date) {
	encodeDate_ = date;
}

QString XmlBuffer::getEncodeDate() const {
	return encodeDate_;
}

void XmlBuffer::setXmlVersion(MusicXMlVersion version) {
	xmlVersion_ = version;
}

XmlBuffer::MusicXMlVersion XmlBuffer::getXmlVersion() const {
	return xmlVersion_;
}

void XmlBuffer::setFinaleVersion(FinaleVersion version) {
	finaleVersion_ = version;
}

XmlBuffer::FinaleVersion XmlBuffer::getFinaleVersion() const {
	return finaleVersion_;
}

int XmlBuffer::calculateStartStopNumber(MusicData* ptr, bool start) {
	int i;
	MusicDataType type = ptr->getMusicDataType();
	int number = 1;

	if (start) {
		for (i = 0; i < nonStopedDatas_.size(); ++i) {
			if (nonStopedDatas_[i].first->getMusicDataType() == type) {
				++number;
			}
		}

		nonStopedDatas_.push_back(qMakePair(ptr, number));
	} else {
		// stop
		QList<QPair<MusicData*, int> > nonStopedDatas;

		for (i = 0; i < nonStopedDatas_.size(); ++i) {
			if (ptr == nonStopedDatas_[i].first) {
				number = nonStopedDatas_[i].second;
			} else {
				nonStopedDatas.push_back(nonStopedDatas_[i]);
			}
		}

		nonStopedDatas_ = nonStopedDatas;
	}

	if (number > 6) {
		number = 6;
	}

	Q_ASSERT(number > 0 && number <= 6);

	return number;
}

void XmlBuffer::clearStartStopDatas() {
	nonStopedDatas_.clear();
}

}
