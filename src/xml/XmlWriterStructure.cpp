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

#include "XmlWriterStructure.h"

#include "XmlBuffer.h"
#include "XmlWriterMeasure.h"
#include "XmlWriterFactory.h"
#include "IXmlNotify.h"

namespace XML {

QString partNumToName(int part) {
	QString ss = QString();

	ss += "P";
	ss += QString::number(part);

	return ss;
}

QString partNumToInstrument(int part) {
	QString ss = QString();

	ss += partNumToName(part);
	ss += "-I";
	ss += QString::number(part);

	return ss;
}

inline QString numToName(int num) {
	QString ss = QString::number(num);
	return ss;
}

int getLeaseCommonMultiple(int a, int b) {
	int m, n;
	int q;

	Q_ASSERT(a > 0 && b > 0);

	m = qMax(a, b);
	n = qMin(a, b);

	q = m % n;

	while (q != 0) {
		m = n;
		n = q;
		q = m % n;
	}

	return a * b / n;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ScorePartwiseWriter::ScorePartwiseWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
    setIsRoot(true);
	setName("score-partwise");
}

void ScorePartwiseWriter::createChildren() {
	QList<QString> titles = buffer()->getOve()->getTitles();

    if (!titles.isEmpty() && !titles[0].isEmpty()) {
		MovementTitleWriterPtr movement = XmlWriterFactory::createMovementTitle(buffer());
		children_.push_back(movement);
	}

	IndentificationWriterPtr indentification = XmlWriterFactory::createIndentification(buffer());
	children_.push_back(indentification);

	DefaultsWriterPtr defaults = XmlWriterFactory::createDefaults(buffer());
	children_.push_back(defaults);

	PartlistWriterPtr partlist = XmlWriterFactory::createPartlist(buffer());
	children_.push_back(partlist);

	for (int i = 0; i < buffer()->getOve()->getPartCount(); ++i) {
		POS pos;
		PartWriterPtr part = XmlWriterFactory::createPart(buffer());

		pos.stave_ = i;
		part->setPos(pos);

		children_.push_back(part);
	}
}

//////////////////////////////////////////////////////////////////////////

MovementTitleWriter::MovementTitleWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("movement-title");
}

void MovementTitleWriter::collect() {
	QList<QString> titles = buffer()->getOve()->getTitles();
    if (!titles.isEmpty()) {
        value_ = titles[0];
	}
}

//////////////////////////////////////////////////////////////////////////

IndentificationWriter::IndentificationWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("identification");
}

void IndentificationWriter::createChildren() {
	WriterPtr child;
	QList<QString> writers = buffer()->getOve()->getWriters();

    if (!writers.isEmpty()) {
        child = XmlWriterFactory::createWriter("creator", buffer()->getOve()->getWriters()[0], strType, "composer");
		children_.push_back(child);
	}

	if (writers.size() > 1) {
        child = XmlWriterFactory::createWriter("creator", buffer()->getOve()->getWriters()[1], strType, "lyricist");
		children_.push_back(child);
	}

    if (!buffer()->getOve()->getCopyrights().isEmpty()) {
        child = XmlWriterFactory::createWriter("rights", buffer()->getOve()->getCopyrights()[0]);
		children_.push_back(child);
	}

	EncodingWriterPtr encoding = XmlWriterFactory::createEncoding(buffer());
	children_.push_back(encoding);
}

//////////////////////////////////////////////////////////////////////////

DefaultsWriter::DefaultsWriter(XmlBuffer* xmlBuffer) : Writer(xmlBuffer) {
	setName("defaults");
}

void DefaultsWriter::createChildren() {
	ScalingWriterPtr scaling = XmlWriterFactory::createScaling(buffer());
	children_.push_back(scaling);

    PageLayoutWriterPtr pageLayout = XmlWriterFactory::createPageLayout(buffer());
	children_.push_back(pageLayout);
}

//////////////////////////////////////////////////////////////////////////

EncodingWriter::EncodingWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("encoding");
}

void EncodingWriter::createChildren() {
	WriterPtr child;

    child = XmlWriterFactory::createWriter("software", "ove2xml");
	children_.push_back(child);

    child = XmlWriterFactory::createWriter("encoder", "Rui Fan (vanferry@gmail.com)");
	children_.push_back(child);

	if (!buffer()->getEncodeDate().isEmpty()) {
        child = XmlWriterFactory::createWriter("encoding-date", buffer()->getEncodeDate());
		children_.push_back(child);
	}
}

//////////////////////////////////////////////////////////////////////////

ScalingWriter::ScalingWriter(XmlBuffer* xmlBuffer) : Writer(xmlBuffer) {
	setName("scaling");
}

void ScalingWriter::createChildren() {
	WriterPtr child;

	//child = XmlWriterFactory::createWriter("millimeters", "7.2319")) ;
	child = XmlWriterFactory::createWriter("millimeters", "8.0000");
	children_.push_back(child);

	child = XmlWriterFactory::createWriter("tenths", numToName(40));
	children_.push_back(child);
}

//////////////////////////////////////////////////////////////////////////

PageLayoutWriter::PageLayoutWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("page-layout");
}

void PageLayoutWriter::createChildren() {
	WriterPtr child;

	if (buffer()->getOve()->getPageCount() > 0) {
		Page* page = buffer()->getOve()->getPage(0);

        child = XmlWriterFactory::createWriter("page-height", numToName(buffer()->getLayoutTenths(page->getPageHeight())));
		children_.push_back(child);

        child = XmlWriterFactory::createWriter("page-width", numToName(buffer()->getLayoutTenths(page->getPageWidth())));
		children_.push_back(child);

        PageMarginsWriterPtr marginWriter = XmlWriterFactory::createPageMargins(buffer());
		marginWriter->setPage(page);
		children_.push_back(marginWriter);
	}
}

//////////////////////////////////////////////////////////////////////////

PageMarginsWriter::PageMarginsWriter(XmlBuffer* xmlBuffer) : Writer(xmlBuffer) {
	setName("page-margins");
}

void PageMarginsWriter::createChildren() {
	WriterPtr child;

    child = XmlWriterFactory::createWriter("left-margin", numToName(buffer()->getLayoutTenths(page_->getLeftMargin())));
	children_.push_back(child);

    child = XmlWriterFactory::createWriter("right-margin", numToName(buffer()->getLayoutTenths(page_->getRightMargin())));
	children_.push_back(child);

    child = XmlWriterFactory::createWriter("top-margin", numToName(buffer()->getLayoutTenths(page_->getTopMargin())));
	children_.push_back(child);

    child = XmlWriterFactory::createWriter("bottom-margin", numToName(buffer()->getLayoutTenths(page_->getBottomMargin())));
	children_.push_back(child);
}

void PageMarginsWriter::setPage(Page* page) {
	page_ = page;
}

//////////////////////////////////////////////////////////////////////////

PartlistWriter::PartlistWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("part-list");
}

void PartlistWriter::createChildren() {
	for (int i = 0; i < buffer()->getOve()->getPartCount(); ++i) {
		POS pos;
		ScorePartWriterPtr child = XmlWriterFactory::createScorePart(buffer());

		pos.stave_ = i;
		child->setPos(pos);

		children_.push_back(child);
	}
}

//////////////////////////////////////////////////////////////////////////

ScorePartWriter::ScorePartWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("score-part");
}

void ScorePartWriter::collect() {
	attrs_["id"] = partNumToName(pos().stave_ + 1);
}

void ScorePartWriter::createChildren() {
	//Track* track = static_cast<Track*> (buffer()->get_ove()->getTrack(pos().stave_, 0));

	//if(!track->get_name().isEmpty())
	{
		//child = XmlWriterFactory::createWriter("part-name", track->get_name())) ;
		WriterPtr child = XmlWriterFactory::createWriter("part-name", "MusicXML Part");
		children_.push_back(child);
	}

	ScoreInstrumentWriterPtr scoreInstrument = XmlWriterFactory::createScoreInstrument(buffer());
	scoreInstrument->setPos(pos());
	children_.push_back(scoreInstrument);

	MidiInstrumentWriterPtr midiInstrument = XmlWriterFactory::createMidiInstrument(buffer());
	midiInstrument->setPos(pos());
	children_.push_back(midiInstrument);
}

//////////////////////////////////////////////////////////////////////////
ScoreInstrumentWriter::ScoreInstrumentWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("score-instrument");
}

void ScoreInstrumentWriter::collect() {
	attrs_["id"] = partNumToInstrument(pos().stave_ + 1);
}

void ScoreInstrumentWriter::createChildren() {
	WriterPtr child;
	Track* track = buffer()->getOve()->getTrack(pos().stave_, 0);

    //if( !track->get_name().isEmpty() )
	{
		QString str = track->getName();
		if (str.isEmpty()) {
			str = "Grand Piano";
		}

		child = XmlWriterFactory::createWriter("instrument-name", str);
		children_.push_back(child);
	}
}

//////////////////////////////////////////////////////////////////////////

MidiInstrumentWriter::MidiInstrumentWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("midi-instrument");
}

void MidiInstrumentWriter::collect() {
	attrs_["id"] = partNumToInstrument(pos().stave_ + 1);
}

void MidiInstrumentWriter::createChildren() {
	Track* track = static_cast<Track*> (buffer()->getOve()->getTrack(pos().stave_, 0));
	WriterPtr child;

	child = XmlWriterFactory::createWriter("midi-channel", numToName(track->getChannel() + 1));
	children_.push_back(child);

	child = XmlWriterFactory::createWriter("midi-program", numToName(track->getPatch() + 1));
	children_.push_back(child);
}

//////////////////////////////////////////////////////////////////////////

PartWriter::PartWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("part");
	createPrefixComment_ = true;
}

void PartWriter::collect() {
	attrs_["id"] = partNumToName(pos().stave_ + 1);

	if (pos().stave_ == buffer()->getOve()->getPartCount() - 1) {
		createSuffixComment_ = true;
	}
}

void PartWriter::createChildren() {
	int division = getPartDivision();
	int partCount = buffer()->getOve()->getPartCount();
	int measureCount = buffer()->getOve()->getTrackBarCount();
	int measureNumber = 0;

	for (int i = 0; i < measureCount; ++i) {
		POS p;
		MeasureWriterPtr child = XmlWriterFactory::createMeasure(buffer());
		Measure* measure = buffer()->getOve()->getMeasure(i);

		p.stave_ = pos().stave_;
		p.bar_ = i;

		child->setPos(p);
		child->setDivision(division);

		if (!measure->getIsPickup()) {
			++measureNumber;
		}

		child->setNumber(measureNumber);

		children_.push_back(child);

		IXmlNotify* notify = buffer()->getNotify();
		if (notify != NULL) {
			notify->notify_convert_pos(i, measureCount, pos().stave_, partCount);
		}
	}
}

void PartWriter::onBeforeProcess() {
	// tie/slur/tuplet/glissando, non of them can cross part
	buffer()->clearStartStopDatas();
}

int PartWriter::getPartDivision() {
	int division(1);

	OveSong* ove = buffer()->getOve();
	int part = pos().stave_;

	for (int i = 0; i < ove->getStaffCount(part); ++i) {
		for (int j = 0; j < ove->getPartBarCount(); ++j) {
			MeasureData* bar_ptr = ove->getMeasureData(part, i, j);
			const QList<NoteContainer*>& containers = bar_ptr->getNoteContainers();

			division = getLeaseCommonMultiple(division, calculateDivision(containers));
		}
	}

	return division;
}

int PartWriter::calculateDivision(const QList<NoteContainer*>& containers) {
	int len(NoteDuration_4);
	int division(1);//lease common multiple
	QMap<int, int> tuplets;//first��tuplet��second��base note length
	QMap<int, int>::iterator it;

	for (int i = 0; i < containers.size(); ++i) {
		int note_len;
		int tuplet;

		note_len = containers[i]->getDuration();

		for (int j = 0; j < containers[i]->getDot(); ++j) {
			note_len /= 2;
		}

		tuplet = containers[i]->getTuplet();

		if (tuplet > 0) {
			if (tuplets.find(tuplet) == tuplets.end()) {
				tuplets[tuplet] = note_len;//Ҳ���׸�������base����
			} else {
				tuplets[tuplet] = qMin(tuplets[tuplet], note_len);
			}
		} else {
			len = qMin(len, note_len);
		}
	}

	division = NoteDuration_4 / len;

	for (it = tuplets.begin(); it != tuplets.end(); ++it) {

		int div(NoteDuration_4 / it.value());

		if (div > 0) {
			int lcm = getLeaseCommonMultiple(it.key(), div);

			division = getLeaseCommonMultiple(lcm, division);
		}
	}

	return division;
}

}
