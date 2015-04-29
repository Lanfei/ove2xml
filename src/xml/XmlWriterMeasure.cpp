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

#include "XmlWriterMeasure.h"

#include "XmlBuffer.h"
#include "XmlWriterPtr.h"
#include "XmlWriterFactory.h"

namespace XML {

inline int getMaxPower2(int n) {
	unsigned int i;
	int mu(2);

	for (i = 0; i < 16; ++i) {
		if (n & (1 << i)) {
			mu = i;
		}
	}

	return pow((float) 2, mu);
}

inline QString numToName(int num) {
	return QString::number(num);
}

inline QString note_2_name(int note) {
	int tone(note % TWELVE_TONE);
	QMap<int, QString> note_map;

	note_map[0] = QString("C");
	note_map[1] = QString("D");
	note_map[2] = QString("D");
	note_map[3] = QString("E");
	note_map[4] = QString("E");
	note_map[5] = QString("F");
	note_map[6] = QString("F");
	note_map[7] = QString("G");
	note_map[8] = QString("G");
	note_map[9] = QString("A");
	note_map[10] = QString("B");
	note_map[11] = QString("B");

	return note_map[tone];
}

QString NoteTypeToString(NoteType type) {
	QString str = QString();

	switch (type) {
	case Note_DoubleWhole:
		str = "breve";
		break;
	case Note_Whole:
		str = "whole";
		break;
	case Note_Half:
		str = "half";
		break;
	case Note_Quarter:
		str = "quarter";
		break;
	case Note_Eight:
		str = "eighth";
		break;
	case Note_Sixteen:
		str = "16th";
		break;
	case Note_32:
		str = "32nd";
		break;
	case Note_64:
		str = "64th";
		break;
	case Note_128:
		str = "128th";
		break;
	case Note_256:
		str = "256th";
		break;
	default:
		Q_ASSERT(false);
		break;
	}

	return str;
}

class CompareMusicData {
public:
	bool operator()(MusicData* data1, MusicData* data2) {
		if (data1->getVoice() != data2->getVoice()) {
			return data1->getVoice() < data2->getVoice();
		}

		return data1->start()->getOffset() < data2->start()->getOffset();
	}
};

//////////////////////////////////////////////////////////////////////////

MeasureWriter::MeasureWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), division_(1), number_(0) {
	setName("measure");
	setCreatePrefixComment(true);
}

void MeasureWriter::collect() {
	int partStaffCount = buffer()->getOve()->getStaffCount(pos().stave_);
	for (int i = 0; i < partStaffCount; ++i) {
		measureDatas_.push_back(buffer()->getOve()->getMeasureData(pos().stave_, i, pos().bar_));
	}

	measure_ = buffer()->getOve()->getMeasure(pos().bar_);

	attrs_[strNumber] = numToName(number_);

	if (measure_->getIsPickup()) {
		attrs_["implicit"] = "yes";
	}

	if (pos().bar_ == 0) {
		createPrefixComment_ = false;
	}
}

void MeasureWriter::createChildren() {
	// left barline
	createBarline(true);

	// attributes
	createAttributes();

	// sound tempo
	createSoundTempo();

	// music data -> notations, etc,.
	createMeasuresMusicDatas();

	// right barline
	createBarline(false);
}

void MeasureWriter::createMeasuresMusicDatas() {
	buildClefs();

	for (int i = 0; i < measureDatas_.size(); ++i) {
		MeasureData* measureData = measureDatas_[i];

		if (i > 0) {
			createBarBackup();
		}

		createMeasureMusicDatas(measureData, i);
	}
}

bool MeasureWriter::canCreateDirection(MusicData* ptr) {
	MusicDataType type = ptr->getMusicDataType();

	switch (type) {
	case MusicData_Tempo: {
		Tempo* tempo = static_cast<Tempo*> (ptr);
		if (tempo->getShowMark() == false) {
			return false;
		}

		break;
	}
	case MusicData_OctaveShift_EndPoint: {
		OctaveShiftEndPoint* octave = static_cast<OctaveShiftEndPoint*> (ptr);
		OctaveShiftPosition position = octave->getOctaveShiftPosition();
		if (position == OctavePosition_Continue) {
			return false;
		}

		break;
	}
	case MusicData_Decorator: {
		Decorator* decorator = static_cast<Decorator*> (ptr);
		Decorator::DecoratorType dType = decorator->getDecoratorType();
		ArticulationType artType = decorator->getArticulationType();

		if (dType != Decorator::Decorator_Articulation) {
			return false;
		}

		if (!(artType == Articulation_Pedal_Down || artType
				== Articulation_Pedal_Up)) {
			return false;
		}

		break;
	}
	case MusicData_Pedal:
	case MusicData_Harp_Pedal: {
		return false;
		break;
	}
	default:
		break;
	}

	return true;
}

void MeasureWriter::createMeasureMusicDatas(MeasureData* bar, int staff) {
	int i;
	int j;
	QList<NoteContainer*> containers = bar->getNoteContainers();

	// empty bar
    if (containers.isEmpty()) {
		createEmptyBarRest(staff, staff);
	}

	// get max voice
	unsigned int maxVoice = 0;
	for (i = 0; i < containers.size(); ++i) {
		if (containers[i]->getVoice() > maxVoice) {
			maxVoice = containers[i]->getVoice();
		}
	}

	// update by voice
	QList<MusicData*> elements = bar->getMusicDatas(MusicData_None);

	for (i = 0; i < elements.size(); ++i) {
		if (elements[i]->getMusicDataType() == MusicData_Note_Container) {
			//NoteContainer* ptr = static_cast<NoteContainer*> (elements[i]);

			continue;
		}

		unsigned int voice = maxVoice;

		for (j = 0; j < containers.size(); ++j) {
			if (elements[i]->start()->getOffset() == containers[j]->start()->getOffset() &&
				containers[j]->getVoice() < voice) {
				voice = containers[j]->getVoice();
			}
		}

		elements[i]->setVoice(voice);
	}

	// sort by voice & unit
	qSort(elements.begin(), elements.end(), CompareMusicData());

	// create xml
	for (i = 0; i < elements.size(); ++i) {
		if (i > 0 && elements[i]->getVoice() != elements[i - 1]->getVoice()) {
			createBarBackup();

			if (elements[i]->start()->getOffset() > 0) {
				// create forward
			}
		}

		MusicDataType type = elements[i]->getMusicDataType();

		switch (type) {
		// note
		case MusicData_Note_Container: {
			NoteContainer* container = static_cast<NoteContainer*> (elements[i]);

			createNotes(bar, container, staff);

			break;
		}
			// harmony
		case MusicData_Harmony: {
			Harmony* harmony = static_cast<Harmony*> (elements[i]);
			HarmonyWriterPtr writer = XmlWriterFactory::createHarmony(buffer());
			writer->setHarmony(harmony);

			children_.push_back(writer);

			break;
		}
			// attributes
		case MusicData_Clef: //С������;���ױ�
		{
			Clef* clef = static_cast<Clef*> (elements[i]);
			AttributeWriterPtr attr = XmlWriterFactory::createAttribute(buffer());

			attr->setPos(pos());
			attr->addClef(clef->getClefType(), staff, createStaff());
			attr->setCreateStaves(measureDatas_.size() > 1, measureDatas_.size());

			children_.push_back(attr);

			break;
		}
			// direction
		case MusicData_Dynamics:
		case MusicData_OctaveShift_EndPoint:
		case MusicData_Expressions:
		case MusicData_Pedal:
		case MusicData_Harp_Pedal:
		case MusicData_Text:// Text_Rehearsal, Text_SystemText, Text_MeasureText
		case MusicData_Tempo://<other-direction>quarter note</other-direction> then <sound tempo="100"/>
		case MusicData_Repeat://coda, segno, and text like Fine (words)
		case MusicData_Wedge_EndPoint: {
			if (canCreateDirection(elements[i])) {
				DirectionWriterPtr direction = XmlWriterFactory::createDirection(buffer());
				direction->setPos(pos());
				direction->addDirection(elements[i]);

				children_.push_back(direction);
			}

			break;
		}
		case MusicData_KuoHao:// parentheses, bracket, brace
		{
			//KuoHao* kuoHao = static_cast<KuoHao*> (elements[i]);
			break;
		}
		case MusicData_Bar_End: {
			break;
		}
		default:
			break;
		}
	}
}

void MeasureWriter::createBarBackup() {
	BarBackupWriterPtr writer = XmlWriterFactory::createBarBackup(buffer());

	writer->setPos(pos());
	writer->setDivision(division_);
	writer->setDuration(getBarDuration(pos().bar_, division_));

	children_.push_back(writer);
}

void MeasureWriter::createEmptyBarRest(unsigned int voice, int staff) {
	BarRestWriterPtr writer = XmlWriterFactory::createBarRest(buffer());

	writer->setVoice(voice);

	if (createStaff()) {
		writer->setStaff(staff);
	}

	writer->setDuration(getBarDuration(pos().bar_, division_));

	children_.push_back(writer);
}

void MeasureWriter::createNotes(MeasureData* measureData, NoteContainer* container, int staff) {
	int i;

	// direction -> direction-type -> pedal
	QList<Articulation*> arts = container->getArticulations();

	for (i = 0; i < arts.size(); ++i) {
		if (arts[i]->getXmlType() == Articulation::Xml_Direction
				&& (arts[i]->getArtType() == Articulation_Pedal_Down
						|| arts[i]->getArtType() == Articulation_Pedal_Up)) {
			DirectionWriterPtr direction = XmlWriterFactory::createDirection(buffer());
			Decorator* pedal = new Decorator;

			pedal->setTick(container->getTick());
			pedal->start()->setOffset(container->start()->getOffset());
			pedal->setColor(container->getColor());
			pedal->setDecoratorType(Decorator::Decorator_Articulation);
			pedal->setArticulationType(arts[i]->getArtType());

			direction->setPos(pos());
			direction->addDirection(pedal);

			children_.push_back(direction);
		}
	}

	// note
	if (container->getIsRaw() || !container->getShow()) {
		return;
	}

	QList<Note*> notes = container->getNotesRests();

	for (i = 0; i < notes.size(); ++i) {
		if (notes[i]->getShow()) {
			NoteWriterPtr noteWriter = XmlWriterFactory::createNote(buffer());
			ClefType clefType = checkClefType(notes[i]->getOffsetStaff() + staff, container->start()->getOffset());

			noteWriter->setPos(pos());
			noteWriter->setMeasure(measure_);
			noteWriter->setMeasureData(measureData);
			noteWriter->setContainer(container);
			noteWriter->setNote(notes[i]);
			noteWriter->setDivision(division_);
			noteWriter->setCreateChord(notes.size() > 1 && i > 0);
			noteWriter->setCreateStaff(createStaff(), staff);
			noteWriter->setClefType(clefType);

			children_.push_back(noteWriter);
		}
	}
}

void MeasureWriter::buildClefs() {
	clefss_.clear();

	for (int i = 0; i < measureDatas_.size(); ++i) {
		QList<MusicData*> datas;
		QList<MusicData*> measureClefs = measureDatas_[i]->getMusicDatas(
				MusicData_Clef);
		QList<Clef*> clefs;

		datas.push_back(measureDatas_[i]->getClef());
		datas += measureClefs;

		for (int j = 0; j < datas.size(); ++j) {
			Clef* ptr = static_cast<Clef*> (datas[j]);
			clefs.push_back(ptr);
		}

		clefss_.push_back(clefs);
	}
}

ClefType MeasureWriter::checkClefType(int staff, int unit) {
	ClefType type = Clef_Treble;

	if (staff >= 0 && staff < clefss_.size()) {
		for (int i = 0; i < clefss_[staff].size(); ++i) {
			Clef* clef = clefss_[staff][i];

			if (i == 0) {
				type = clef->getClefType();
			}

			if (clef->start()->getOffset() > unit) {
				break;
			}

			type = clef->getClefType();
		}
	}

	return type;
}

bool MeasureWriter::createStaff() const {
	return measureDatas_.size() > 1;
}

void MeasureWriter::createBarline(bool left_barline) {
    if (measureDatas_.isEmpty()) {
		return;
	}

	int i;
	MeasureData* measureData = measureDatas_[0];
	BarlineType barlineType = left_barline ? measure_->getLeftBarline()
			: measure_->getRightBarline();
	QList<MusicData*> repeats = measureData->getMusicDatas(MusicData_Repeat);
	bool create = false; // ���������ٴ���

	BarlineWriterPtr barlineWriter = XmlWriterFactory::createBarline(buffer());

	barlineWriter->setLeftBarline(left_barline);

	// bar-style
	if (barlineType != Barline_Default) {
		barlineWriter->setBarlineType(barlineType);
		create = true;
	}

	// endings
	QList<MusicData*> endings = measureData->getCrossMeasureElements(
			MusicData_Numeric_Ending, MeasureData::PairType_All);
	for (i = 0; i < endings.size(); ++i) {
		bool start;
		QList<int> nums;
		NumericEnding* ending = static_cast<NumericEnding*> (endings[i]);

		if (left_barline && measure_->getBarNumber()->getIndex()
				== ending->start()->getMeasure()) {
			start = true;
			nums = ending->getNumbers();

			barlineWriter->setNumericEnding(start, nums);
			create = true;
		}

		if (!left_barline && measure_->getBarNumber()->getIndex()
				== ending->start()->getMeasure() + ending->stop()->getMeasure()) {
			start = false;
			nums = ending->getNumbers();

			barlineWriter->setNumericEnding(start, nums);
			create = true;
		}
	}

	// repeat
	for (i = 0; i < repeats.size(); ++i) {
		RepeatSymbol* repeat = static_cast<RepeatSymbol*> (repeats[i]);
		barlineWriter->setRepeatType(repeat->getRepeatType());
		create = true;
	}

	// right barline of last measure
	if (!create && pos().bar_ == buffer()->getOve()->getMeasureCount() - 1
			&& !left_barline) {
		barlineWriter->setBarlineType(Barline_Final);
		create = true;
	}

	if (create) {
		children_.push_back(barlineWriter);
	}
}

void MeasureWriter::createSoundTempo(void) {
    if (measureDatas_.isEmpty()) {
		return;
	}
	if (pos().bar_ > 0) {
		return;
	}

	MeasureData* measureData = measureDatas_[0];
	QList<MusicData*> tempos = measureData->getMusicDatas(MusicData_Tempo);

	for (int i = 0; i < tempos.size(); ++i) {
		Tempo* tempo = static_cast<Tempo*> (tempos[i]);

		if (tempo->getShowMark()) {
			SoundWriterPtr sound = XmlWriterFactory::createSound(buffer());

			sound->setTempo(tempo);

			children_.push_back(sound);
		}
	}
}

void MeasureWriter::createAttributes() {
    if (measureDatas_.isEmpty()) {
		return;
	}

	int i;
	bool fdivision = false;
	bool fkey = false;
	bool ftime = false;
	bool fclef = false;
	bool fStaves = false;
	bool fRepeat = false;
	int measureID = measure_->getBarNumber()->getIndex();
	AttributeWriterPtr attr = XmlWriterFactory::createAttribute(buffer());
	Key* keyPtr = measureDatas_[0]->getKey();

	attr->setPos(pos());

	if (pos().bar_ == 0) {
		fdivision = true;
		fkey = true;
		ftime = true;
		fclef = true;

		if (measureDatas_.size() > 1) {
			fStaves = true;
		}
	} else {
		if (keyPtr->getKey() != keyPtr->getPreviousKey()) {
			fkey = true;
		}

		TimeSignature* ts1 =
				buffer()->getOve()->getMeasure(pos().bar_ - 1)->getTime();
		TimeSignature* ts2 =
				buffer()->getOve()->getMeasure(pos().bar_)->getTime();

		if (!(ts1->getNumerator() == ts2->getNumerator()
				&& ts1->getDenominator() == ts2->getDenominator()
				&& ts1->getIsSymbol() == ts2->getIsSymbol())) {
			ftime = true;
		}
	}

	// measure repeat
	QList<MusicData*> repeats = measureDatas_[0]->getCrossMeasureElements(
			MusicData_Measure_Repeat, MeasureData::PairType_All);

	// stop
	for (i = 0; i < repeats.size(); ++i) {
		bool stop = (measureID != repeats[i]->start()->getMeasure());

		if (stop) {
			attr->addMeasureStyle(repeats[i], false);
			fRepeat = true;
		}
	}

	// start
	for (i = 0; i < repeats.size(); ++i) {
		bool start = (measureID == repeats[i]->start()->getMeasure());

		if (start) {
			attr->addMeasureStyle(repeats[i], true);
			fRepeat = true;
		}
	}

	if (fdivision || fkey || ftime || fclef || fStaves || fRepeat) {

		attr->setPos(pos());
		attr->setCreateDivision(fdivision, division_);
		attr->setCreateKey(fkey, keyPtr);
		attr->setCreateTime(ftime);

		if (fclef) {
			for (i = 0; i < measureDatas_.size(); ++i) {
				ClefType type = measureDatas_[i]->getClef()->getClefType();

				attr->addClef(type, i, measureDatas_.size() > 1);
			}
		}

		attr->setCreateStaves(fStaves, measureDatas_.size());

		children_.push_back(attr);
	} else {
		if (measure_->getMultiMeasureRestCount() > 0) {
			children_.push_back(attr);
		}
	}
}

int MeasureWriter::getBarDuration(int bar, int division) {
	TimeSignature* ts = buffer()->getOve()->getMeasure(bar)->getTime();
	int duration = 4 * 1.0 / ts->getDenominator() * ts->getNumerator()
			* division;

	return duration;
}

void MeasureWriter::setNumber(int number) {
	number_ = number;
}

void MeasureWriter::setDivision(int division) {
	division_ = division;
}

//////////////////////////////////////////////////////////////////////////

AttributeWriter::AttributeWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), createDivision_(false), division_(1),
			createKey_(false), createTime_(false), staff_(0), staves_(1) {
	setName("attributes");
}

void AttributeWriter::createChildren() {
	int i;
	WriterPtr child;

	if (createDivision_) {
		child = XmlWriterFactory::createWriter("divisions", numToName(
				division_));
		children_.push_back(child);
	}

	if (createKey_) {
		KeyWriterPtr keyWriter = XmlWriterFactory::createKey(buffer());
		keyWriter->setPos(pos());
		keyWriter->setKey(keyPtr_);
		children_.push_back(keyWriter);
	}

	if (createTime_) {
		TimeWriterPtr timeWriter = XmlWriterFactory::createTime(buffer());
		timeWriter->setPos(pos());
		children_.push_back(timeWriter);
	}

	if (createStaves_) {
		child = XmlWriterFactory::createWriter("staves", numToName(staves_));
		children_.push_back(child);
	}

	for (i = 0; i < clefs_.size(); ++i) {
		ClefWriterPtr clefWriter = XmlWriterFactory::createClef(buffer());

		clefWriter->setClefNumber(clefs_[i].staff_, clefs_[i].createNumber_);
		clefWriter->setClefType(clefs_[i].type_);

		children_.push_back(clefWriter);
	}

	for (i = 0; i < measureStyles_.size(); ++i) {
		if (measureStyles_[i].first->getMusicDataType()
				== MusicData_Measure_Repeat) {
			MeasureRepeat* repeat =
					static_cast<MeasureRepeat*> (measureStyles_[i].first);
			MeasureStyleWriterPtr msWriter =
					XmlWriterFactory::createMeasureStyle(buffer());

			msWriter->setRepeat(repeat->getSingleRepeat() ? 1 : 2,
					measureStyles_[i].second);

			children_.push_back(msWriter);
		}
	}

	{
		Measure* measurePtr = buffer()->getOve()->getMeasure(pos().bar_);
		if (measurePtr->getMultiMeasureRestCount() > 0) {
			MeasureStyleWriterPtr msWriter =
					XmlWriterFactory::createMeasureStyle(buffer());
			msWriter->setMultiRestCount(
					measurePtr->getMultiMeasureRestCount());

			children_.push_back(msWriter);
		}
	}
}

void AttributeWriter::setCreateDivision(bool create, int division) {
	createDivision_ = create;
	division_ = division;
}

void AttributeWriter::setCreateKey(bool create, Key* keyPtr) {
	createKey_ = create;
	keyPtr_ = keyPtr;
}

void AttributeWriter::setCreateTime(bool create) {
	createTime_ = create;
}

void AttributeWriter::setCreateStaves(bool create, int staves) {
	createStaves_ = create;
	staves_ = staves;
}

void AttributeWriter::setCreateClef(bool create, int staff) {
	createClef_ = create;
	staff_ = staff;
}

void AttributeWriter::addClef(ClefType clefType, int staff, bool createNumber) {
	ClefNode node;
	node.type_ = clefType;
	node.staff_ = staff;
	node.createNumber_ = createNumber;
	clefs_.push_back(node);
}

void AttributeWriter::addMeasureStyle(MusicData* ptr, bool start) {
	measureStyles_.push_back(qMakePair(ptr, start));
}

//////////////////////////////////////////////////////////////////////////

KeyWriter::KeyWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("key");
}

void KeyWriter::setKey(Key* ptr) {
	key_ = ptr;
}

void KeyWriter::createChildren() {
	WriterPtr child;
	int fifths = key_->getKey(); // [-7, +7]
	int cancel = key_->getSymbolCount() - fifths;

	if (cancel != 0 && pos().bar_ != 0) {
		child = XmlWriterFactory::createWriter("cancel", numToName(cancel));
		children_.push_back(child);
	}

	child = XmlWriterFactory::createWriter("fifths", numToName(fifths));
	children_.push_back(child);

	QString mode = (true) ? "major" : "minor";

	child = XmlWriterFactory::createWriter("mode", mode);
	children_.push_back(child);
}

//////////////////////////////////////////////////////////////////////////

TimeWriter::TimeWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("time");
}

void TimeWriter::createChildren() {
	WriterPtr child;
	TimeSignature* time = buffer()->getOve()->getMeasure(pos().bar_)->getTime();
	int num = time->getNumerator();
	int den = time->getDenominator();

	child = XmlWriterFactory::createWriter("beats", numToName(num));
	children_.push_back(child);

	child = XmlWriterFactory::createWriter("beat-type", numToName(den));
	children_.push_back(child);

	if (time->getIsSymbol()) {
		QString value = QString();

		if (num == 4 && den == 4) {
			value = "common";
		} else if (num == 2 && den == 2) {
			value = "cut";
		}

		if (value != QString()) {
			attrs_["symbol"] = value;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

ClefWriter::ClefWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), createNumber_(false), number_(0), type_(Clef_Treble) {
	setName("clef");
}

void ClefWriter::collect() {
	if (createNumber_) {
		attrs_[strNumber] = numToName(number_ + 1);
	}
}

void ClefWriter::createChildren() {
	WriterPtr child;
	QString sign("none");
	int line(2);
	int octave(0);

	switch (type_) {
	case Clef_Treble: {
		sign = "G";
		line = 2;
		break;
	}
	case Clef_Bass: {
		sign = "F";
		line = 4;
		break;
	}
	case Clef_Alto: {
		sign = "C";
		line = 3;
		break;
	}
	case Clef_UpAlto: {
		sign = "C";
		line = 4;
		break;
	}
	case Clef_DownDownAlto: {
		sign = "C";
		line = 1;
		break;
	}
	case Clef_DownAlto: {
		sign = "C";
		line = 2;
		break;
	}
	case Clef_UpUpAlto: {
		sign = "C";
		line = 5;
		break;
	}
	case Clef_Treble8va: {
		sign = "G";
		line = 2;
		octave = 1;
		break;
	}
	case Clef_Bass8va: {
		sign = "F";
		line = 4;
		octave = 1;
		break;
	}
	case Clef_Treble8vb: {
		sign = "G";
		line = 2;
		octave = -1;
		break;
	}
	case Clef_Bass8vb: {
		sign = "F";
		line = 4;
		octave = -1;
		break;
	}
	case Clef_Percussion1:
	case Clef_Percussion2: {
		sign = "percussion";
		line = 0;
		break;
	}
	case Clef_TAB: {
		sign = "TAB";
		line = 5;
		break;
	}
	default:
		//Q_ASSERT(false) ;
		break;
	}

	child = XmlWriterFactory::createWriter("sign", sign);
	children_.push_back(child);

	if (line != 0) {
		child = XmlWriterFactory::createWriter("line", numToName(line));
		children_.push_back(child);
	}

	if (octave != 0) {
		child = XmlWriterFactory::createWriter("clef-octave-change",
				numToName(octave));
		children_.push_back(child);
	}
}

void ClefWriter::setClefNumber(int number, bool create) {
	createNumber_ = create;
	number_ = number;
}

void ClefWriter::setClefType(ClefType type) {
	type_ = type;
}

//////////////////////////////////////////////////////////////////////////
MeasureStyleWriter::MeasureStyleWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), setRepeat_(false), start_(true), styleValue_(1),
			multiRestCount_(0) {
	setName("measure-style");
}

void MeasureStyleWriter::createChildren() {
	WriterPtr child;

	if (setRepeat_) {
		QString value = start_ ? numToName(styleValue_) : QString();

		child = XmlWriterFactory::createWriter("measure-repeat", value,
				strType, strStart);
		children_.push_back(child);
	}

	if (multiRestCount_ > 0) {
		child = XmlWriterFactory::createWriter("multiple-rest", numToName(
				multiRestCount_));
		children_.push_back(child);
	}
}

void MeasureStyleWriter::setRepeat(int styleValue, bool start) {
	setRepeat_ = true;
	styleValue_ = styleValue;
	start_ = start;
}

void MeasureStyleWriter::setMultiRestCount(int count) {
	multiRestCount_ = count;
}

//////////////////////////////////////////////////////////////////////////

NoteWriter::NoteWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), division_(1), startTuplet_(false), createStaff_(false),
			createChord_(false), staff_(0), clefType_(Clef_Treble) {
	setName("note");
}

void NoteWriter::collect() {
	if (note_->getOnVelocity() != 0) {
		if (buffer()->getFinaleVersion() != XmlBuffer::Finale_2008) {
			//attrs_["dynamics"] = num_2_name(note_ptr_->get_on_velocity()) ;
			//attrs_["end-dynamics"] = num_2_name(note_ptr_->get_off_velocity()) ;
		}
	}
}

void NoteWriter::createChildren() {
	// grace
	createGrace();

	// cue
	createCue();

	// full note
	createFullNote();

	// duration
	createDuration();

	// tie
	createTie();

	// voice
	createVoice();

	// type
	createTypeWriter();

	// dot
	createDot();

	// accidental
	createAccidental();

	// time-modification
	createTimeModification();

	// stem
	createStem();

	// note head
	createNotehead();

	// staff
	createStaff();

	// beam
	createBeam();

	// notations
	createNotations();

	// lyric
	createLyric();
}

void NoteWriter::createGrace() {
	if (container_->getIsGrace()) {
		WriterPtr child = XmlWriterFactory::createWriter("grace");
		children_.push_back(child);
	}
}

void NoteWriter::createCue() {
	if (container_->getIsCue()) {
		WriterPtr child = XmlWriterFactory::createWriter("cue");
		children_.push_back(child);
	}
}

void NoteWriter::createFullNote() {
	WriterPtr child;

	// chord
	if (createChord_) {
		child = XmlWriterFactory::createWriter("chord", QString());
		children_.push_back(child);
	}

	// rest
	if (container_->getIsRest()) {
		RestWriterPtr rest = XmlWriterFactory::createRest(buffer());
		rest->setNote(note_);
		rest->setNoteType(container_->getNoteType());
		rest->setClefType(clefType_);
		children_.push_back(rest);
	}
	// pitch
	else {
		PitchWriterPtr pitch = XmlWriterFactory::createPitch(buffer());

		pitch->setLineNote(note_->getLine(), note_->getNote());
		pitch->setAccidental(note_->getAccidental());
		pitch->setClefType(clefType_);

		children_.push_back(pitch);
	}

	// ����unpitched
}

void NoteWriter::createDuration() {
	int length = container_->getDuration();
	int tuplet = container_->getTuplet();

	if (container_->getIsGrace()) {
		return;
	}

	if (tuplet > 0) {
		length = length * getMaxPower2(tuplet) / tuplet;
	}

	int duration = length * division_ / NoteDuration_128 / 32;

	WriterPtr child = XmlWriterFactory::createWriter("duration", numToName(
			duration));
	children_.push_back(child);
}

void NoteWriter::createTie() {
	int i;
	QList<MusicData*> ties = measureData_->getCrossMeasureElements(MusicData_Tie, MeasureData::PairType_All);
	int barIndex = measure_->getBarNumber()->getIndex();

	if (container_->getIsCue()) {
		return;
	}

	// tie ��stop��start
	for (i = 0; i < ties.size(); ++i) {
		Tie* tie = static_cast<Tie*> (ties[i]);

		if (tie->getNote() != (int)note_->getNote()) {
			continue;
		}

		if (tie->getRightLine()->getLine() == note_->getLine()
				&& tie->stop()->getOffset() == container_->start()->getOffset()
				&& ((tie->stop()->getMeasure() == 0
						&& tie->start()->getMeasure() == barIndex)
						|| (tie->stop()->getMeasure() > 0
								&& tie->start()->getMeasure()
										+ tie->stop()->getMeasure() == barIndex))) {
			WriterPtr child = XmlWriterFactory::createWriter("tie", QString(), strType, strStop);
			children_.push_back(child);

			ties_.push_back(qMakePair((MusicData*) tie, false));
		}

		if (tie->getLeftLine()->getLine() == note_->getLine()
				&& tie->start()->getOffset() == container_->start()->getOffset()
				&& tie->start()->getMeasure() == barIndex) {

			WriterPtr child = XmlWriterFactory::createWriter("tie", QString(), strType, strStart);
			children_.push_back(child);

			ties_.push_back(qMakePair((MusicData*) tie, true));
		}
	}
}

void NoteWriter::createVoice() {
	//WriterPtr child = XmlWriterFactory::createWriter("voice", numToName(container_->getVoice() + 1));
	WriterPtr child = XmlWriterFactory::createWriter("voice", numToName(container_->getXmlVoice() + 1));

	children_.push_back(child);
}

void NoteWriter::createTypeWriter() {
	QString str = NoteTypeToString(container_->getNoteType());
	WriterPtr child = XmlWriterFactory::createWriter(strType, str);

	children_.push_back(child);
}

void NoteWriter::createDot() {
	for (int i = 0; i < container_->getDot(); ++i) {
		WriterPtr child = XmlWriterFactory::createWriter("dot", QString());
		children_.push_back(child);
	}
}

void NoteWriter::createAccidental() {
	if (container_->getIsRest()) {
		return;
	}

	QString str = QString();
	QString att = QString();
	QString attValue = QString();

	AccidentalType acc = Accidental_Normal;
	if (note_->getShowAccidental() && note_->getAccidental()
			!= Accidental_Normal) {
		acc = note_->getAccidental();
	}

	switch (acc) {
	case Accidental_Normal: {
		break;
	}
	case Accidental_Sharp: {
		str = "sharp";
		break;
	}
	case Accidental_Flat: {
		str = "flat";
		break;
	}
	case Accidental_Natural: {
		str = "natural";
		break;
	}
	case Accidental_DoubleSharp: {
		str = "double-sharp";
		break;
	}
	case Accidental_DoubleFlat: {
		str = "double-flat";
		break;
	}
	case Accidental_Sharp_Caution: {
		str = "sharp";
		att = "cautionary";
		attValue = "yes";
		break;
	}
	case Accidental_Flat_Caution: {
		str = "flat";
		att = "cautionary";
		attValue = "yes";
		break;
	}
	case Accidental_Natural_Caution: {
		str = "natural";
		att = "cautionary";
		attValue = "yes";
		break;
	}
	case Accidental_DoubleSharp_Caution: {
		str = "double-sharp";
		att = "cautionary";
		attValue = "yes";
		break;
	}
	case Accidental_DoubleFlat_Caution: {
		str = "double-flat";
		att = "cautionary";
		attValue = "yes";
		break;
	}
	default: {
		break;
	}
	}

	if (str != QString()) {
		WriterPtr child = XmlWriterFactory::createWriter("accidental", str,
				att, attValue);
		children_.push_back(child);
	}
}

void NoteWriter::createTimeModification() {
	if (container_->getTuplet() > 0) {
		TimeModificationWriterPtr tm = XmlWriterFactory::createTimeModification(buffer());
		tm->setTupletSpace(container_->getTuplet(), container_->getSpace());
		children_.push_back(tm);
	}
}

void NoteWriter::createStem() {
	if (!container_->getIsRest() && container_->getNoteType() > Note_Whole) {
		QString stem_value = container_->getStemUp() ? "up" : "down";
		WriterPtr child = XmlWriterFactory::createWriter("stem", stem_value);
		children_.push_back(child);
	}
}

void NoteWriter::createNotehead() {
	QString str = QString();

	switch (note_->getHeadType()) {
	case NoteHead_Standard: {
		break;
	}
	case NoteHead_Invisible: {
		str = "none";
		break;
	}
	case NoteHead_Rhythmic_Slash: {
		str = "slash";
		break;
	}
	case NoteHead_Percussion: {
		str = "";
		break;
	}
	case NoteHead_Closed_Rhythm: {
		str = "";
		break;
	}
	case NoteHead_Open_Rhythm: {
		str = "";
		break;
	}
	case NoteHead_Closed_Slash: {
		str = "slashed";
		break;
	}
	case NoteHead_Open_Slash: {
		str = "";
		break;
	}
	case NoteHead_Closed_Do: {
		str = "do";
		break;
	}
	case NoteHead_Open_Do: {
		str = "do";
		break;
	}
	case NoteHead_Closed_Re: {
		str = "re";
		break;
	}
	case NoteHead_Open_Re: {
		str = "re";
		break;
	}
	case NoteHead_Closed_Mi: {
		str = "mi";
		break;
	}
	case NoteHead_Open_Mi: {
		str = "mi";
		break;
	}
	case NoteHead_Closed_Fa: {
		str = "fa";
		break;
	}
	case NoteHead_Open_Fa: {
		str = "fa";
		break;
	}
	case NoteHead_Closed_Sol: {
		str = "so";
		break;
	}
	case NoteHead_Open_Sol: {
		str = "so";
		break;
	}
	case NoteHead_Closed_La: {
		str = "la";
		break;
	}
	case NoteHead_Open_La: {
		str = "la";
		break;
	}
	case NoteHead_Closed_Ti: {
		str = "ti";
		break;
	}
	case NoteHead_Open_Ti: {
		str = "ti";
		break;
	}
	default: {
		break;
	}
	}

	if (str != QString()) {
		WriterPtr child = XmlWriterFactory::createWriter("notehead", str);
		children_.push_back(child);
	}
}

void NoteWriter::createStaff() {
	if (createStaff_) {
		WriterPtr child = XmlWriterFactory::createWriter("staff", numToName(staff_ + note_->getOffsetStaff() + 1));
		//WriterPtr child = XmlWriterFactory::createWriter("staff", num_2_name(staff_+1))) ;
		children_.push_back(child);
	}
}

void NoteWriter::createNotationsPairElements(const QList<MusicData*>& pairs) {
	if (createChord_) {
		return;
	}

	MeasurePos containerStart = container_->start()->shiftMeasure(0);
	MeasurePos containerEnd = container_->stop()->shiftMeasure(container_->start()->getMeasure());

	for (int i = 0; i < pairs.size(); ++i) {
		bool create = false;
		bool start = true;
		MeasurePos pairStart = pairs[i]->start()->shiftMeasure(0);
		MeasurePos pairEnd = pairs[i]->stop()->shiftMeasure(pairs[i]->start()->getMeasure());

		if (pairStart == containerStart) {
			create = true;
			start = true;
		}

		if (pairEnd == containerStart) {
			create = true;
			start = false;
		}

		if (create) {
			NotationsWriterPtr notations = XmlWriterFactory::createNotations(buffer());
			notations->addDataAndStart(pairs[i], start);
			children_.push_back(notations);
		}
	}
}

void NoteWriter::createBeam() {
	QList<MusicData*> beams = measureData_->getCrossMeasureElements(MusicData_Beam, MeasureData::PairType_All);
	MeasurePos containerStart = container_->start()->shiftMeasure(0);
	MeasurePos containerEnd = container_->stop()->shiftMeasure(container_->start()->getMeasure());
	int beamCnt = 0;

	for (int i = 0; i < beams.size(); ++i) {
		Beam* beam = static_cast<Beam*> (beams[i]);
		MeasurePos beamStart = beam->start()->shiftMeasure(0);
		MeasurePos beamEnd = beam->stop()->shiftMeasure(beam->start()->getMeasure());

		if (beamStart <= containerStart && beamEnd >= containerStart) {
			const QList<QPair<MeasurePos, MeasurePos> > lines =	beam->getLines();

			for (int j = 0; j < lines.size(); ++j) {
				MeasurePos lineStart = lines[j].first.shiftMeasure(beam->start()->getMeasure());
				MeasurePos lineEnd = lines[j].second.shiftMeasure(beam->start()->getMeasure());

				if (lineStart <= containerStart && lineEnd >= containerStart) {
					QString str = QString();

					if (lineStart != lineEnd) {
						if (lineStart == containerStart) {
							str = "begin";
						}

						if (lineStart < containerStart && lineEnd
								> containerStart) {
							str = "continue";
						}

						if (lineEnd == containerStart) {
							str = "end";
						}
					} else // lineStart == lineEnd
					{
						if (lineStart == containerStart) {
							str = "forward hook";
						}

						if (lineEnd == containerEnd) {
							str = "backward hook";
						}
					}

					if (str != QString()) {
						++beamCnt;
						WriterPtr child = XmlWriterFactory::createWriter("beam", str, strNumber, numToName(beamCnt));
						children_.push_back(child);
					}
				}
			}
		}
	}
}

void NoteWriter::createNotations() {
	createNotationsTied(); //
	createNotationsSlur(); //
	createNotationsTuplet(); //
	createNotationsGlissando(); //
	createNotationsSlide();
	createNotationsOrnaments();
	createNotationsTechnical();
	createNotationsArticulations();
	createNotationsDynamics();
	createNotationsFermata();
	createNotationsArpeggiate();
}

void NoteWriter::createLyric() {
	if (container_->getIsRest()) {
		return;
	}

	QList<MusicData*> lyrics = measureData_->getMusicDatas(MusicData_Lyric);

	for (int i = 0; i < lyrics.size(); ++i) {
		Lyric* lyric = static_cast<Lyric*> (lyrics[i]);

		if (lyric->start()->getOffset() == container_->start()->getOffset()
				&& lyric->getVoice() == container_->getVoice()
                && !lyric->getLyric().isEmpty()) {
			LyricWriterPtr writer = XmlWriterFactory::createLyric(buffer());

			writer->setText(lyric->getLyric());
			writer->setVerse(lyric->getVerse());

			children_.push_back(writer);
		}
	}
}

void NoteWriter::createNotationsTied() {
	for (int i = 0; i < ties_.size(); ++i) {
		NotationsWriterPtr notations = XmlWriterFactory::createNotations(buffer());

		notations->addDataAndStart(ties_[i].first, ties_[i].second);
		children_.push_back(notations);
	}
}

void NoteWriter::createNotationsSlur() {
	QList<MusicData*> slurs = measureData_->getCrossMeasureElements(MusicData_Slur, MeasureData::PairType_All);
	createNotationsPairElements(slurs);
}

void NoteWriter::createNotationsTuplet() {
	QList<MusicData*> tuplets = measureData_->getCrossMeasureElements(MusicData_Tuplet, MeasureData::PairType_All);
	createNotationsPairElements(tuplets);
}

void NoteWriter::createNotationsGlissando() {
	QList<MusicData*> glissandos = measureData_->getCrossMeasureElements(MusicData_Glissando, MeasureData::PairType_All);
	createNotationsPairElements(glissandos);
}

void NoteWriter::createNotationsSlide() {
}

void NoteWriter::createNotationsOrnaments() {
}

void NoteWriter::createNotationsTechnical() {
}

void NoteWriter::createNotationsArticulations() {
	int i;
	QList<Articulation*> arts = container_->getArticulations();

	// ornaments
	bool createOrnament = false;
	NotationsWriterPtr ornaWriter = XmlWriterFactory::createNotations(buffer());

	for (i = 0; i < arts.size(); ++i) {
		if (arts[i]->getXmlType() == Articulation::Xml_Ornament) {
			ornaWriter->addOrnament(arts[i]);
			createOrnament = true;
		}
	}

	if (createOrnament) {
		children_.push_back(ornaWriter);
	}

	// technical
	bool createTechnical = false;
	NotationsWriterPtr technicalWriter = XmlWriterFactory::createNotations(
			buffer());

	for (i = 0; i < arts.size(); ++i) {
		if (arts[i]->getXmlType() == Articulation::Xml_Technical) {
			technicalWriter->addTechnical(arts[i]);
			createTechnical = true;
		}
	}

	if (createTechnical) {
		children_.push_back(technicalWriter);
	}

	// articulations
	bool createArticulation = false;
	NotationsWriterPtr artWriter = XmlWriterFactory::createNotations(buffer());

	for (i = 0; i < arts.size(); ++i) {
		if (arts[i]->getXmlType() == Articulation::Xml_Articulation) {
			artWriter->addArticulation(arts[i]);
			createArticulation = true;
		}
	}

	if (createArticulation) {
		children_.push_back(artWriter);
	}

	// fermata
	bool createFermata = false;
	NotationsWriterPtr ferWriter = XmlWriterFactory::createNotations(buffer());

	for (i = 0; i < arts.size(); ++i) {
		if (arts[i]->getXmlType() == Articulation::Xml_Fermata) {
			ferWriter->addFermata(arts[i]);
			createFermata = true;
		}
	}

	if (createFermata == true) {
		children_.push_back(ferWriter);
	}

	// arpeggiate
	bool createArpeggiate = false;
	NotationsWriterPtr arpWriter = XmlWriterFactory::createNotations(buffer());

	for (i = 0; i < arts.size(); ++i) {
		if (arts[i]->getXmlType() == Articulation::Xml_Arpeggiate) {
			arpWriter->addArpeggiate(arts[i]);
			createArpeggiate = true;
		}
	}

	if (createArpeggiate == true) {
		children_.push_back(arpWriter);
	}
}

void NoteWriter::createNotationsDynamics() {
}

void NoteWriter::createNotationsFermata() {
}

void NoteWriter::createNotationsArpeggiate() {
}

void NoteWriter::setMeasure(Measure* measure) {
	measure_ = measure;
}

void NoteWriter::setMeasureData(MeasureData* measureData) {
	measureData_ = measureData;
}

void NoteWriter::setContainer(NoteContainer* container) {
	container_ = container;
}

void NoteWriter::setNote(Note* note_ptr) {
	note_ = note_ptr;
}

void NoteWriter::setDivision(int division) {
	division_ = division;
}

void NoteWriter::setCreateChord(bool create) {
	createChord_ = create;
}

void NoteWriter::setStartTuplet(bool start) {
	startTuplet_ = start;
}

void NoteWriter::setCreateStaff(bool create, int staff) {
	createStaff_ = create;
	staff_ = staff;
}

void NoteWriter::setClefType(ClefType type) {
	clefType_ = type;
}

//////////////////////////////////////////////////////////////////////////
BarRestWriter::BarRestWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), duration_(4), voice_(0), staff_(0), createStaff_(false) {
	setName("note");
}

void BarRestWriter::createChildren() {
	WriterPtr writer;

	writer = XmlWriterFactory::createWriter("rest");
	children_.push_back(writer);

	writer = XmlWriterFactory::createWriter("duration", numToName(duration_));
	children_.push_back(writer);

	writer = XmlWriterFactory::createWriter("voice", numToName(voice_ + 1));
	children_.push_back(writer);

	if (createStaff_) {
		writer = XmlWriterFactory::createWriter("staff", numToName(staff_ + 1));
		children_.push_back(writer);
	}
}

void BarRestWriter::setDuration(int duration) {
	duration_ = duration;
}

void BarRestWriter::setVoice(unsigned int voice) {
	voice_ = voice;
}

void BarRestWriter::setStaff(int staff) {
	staff_ = staff;
	createStaff_ = true;
}

//////////////////////////////////////////////////////////////////////////
HarmonyWriter::HarmonyWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("harmony");
}

void HarmonyWriter::collect() {
	kind_ = harmonyTypeToKind(harmony_->getHarmonyType());
	degrees_ = harmonyTypeToDegrees(harmony_->getHarmonyType());
}

void HarmonyWriter::createChildren() {
	WriterPtr child;
	RootWriterPtr root = XmlWriterFactory::createRoot(buffer());
	QPair<unsigned int, int> root_note;

	root_note.first = harmony_->getRoot();
	root_note.second = 0;
	root->setRoot(root_note.first);
	root->setAlter(root_note.second);
	children_.push_back(root);

	child = XmlWriterFactory::createWriter("kind", kind_);
	children_.push_back(child);

	if (harmony_->getBass() != INVALID_NOTE) {
		QPair<unsigned int, int> bass_note;

		bass_note.first = harmony_->getBass();
		bass_note.second = 0;

		BassWriterPtr bass = XmlWriterFactory::createBass(buffer());
		bass->setBass(bass_note.first);
		bass->setAlter(bass_note.second);

		children_.push_back(bass);
	}

	for (int i = 0; i < degrees_.size(); ++i) {
		DegreeWriterPtr degree = XmlWriterFactory::createDegree(buffer());
		degree->setValue(degrees_[i].value_);
		degree->setAlter(degrees_[i].alter_);
		degree->setType(degrees_[i].type_);

		children_.push_back(degree);
	}
}

void HarmonyWriter::setHarmony(Harmony* harmony_ptr) {
	harmony_ = harmony_ptr;
}

QString HarmonyWriter::harmonyTypeToKind(HarmonyType type) {
	QMap<unsigned int, QString> harmony_map;

	// 	Harmony_Minor7b5		= 0x0449,
	// 	Harmony_Sus7			= 0x04A1,
	// 	Harmony_Dim7			= 0x0249,
	// 	Harmony_Aug			= 0x0111,
	// 	Harmony_Aug7			= 0x0511,
	// 	Harmony_Min9b5		= 0x044D,
	// 	Harmony_Dominant7s9	= 0x0499,
	// 	Harmony_Dominant13	= 0x0615,
	// 	Harmony_Min6			= 0x0289,
	// 	Harmony_Maj6			= 0x0291,

	harmony_map[Harmony_maj] = "major";
	harmony_map[Harmony_maj7] = "major-seventh";
	harmony_map[Harmony_6] = "major-sixth";
	harmony_map[Harmony_aug] = "augmented";
	harmony_map[Harmony_7] = "dominant";
	harmony_map[Harmony_9] = "dominant-ninth";
	harmony_map[Harmony_min] = "minor";
	harmony_map[Harmony_min7] = "minor-seventh";
	harmony_map[Harmony_min6] = "minor-sixth";
	//harmony_map[Harmony_MINMAJ7] = "major-minor" ;
	harmony_map[Harmony_min7b5] = "minor-seventh";
	harmony_map[Harmony_sus4] = "suspended-fourth";
	harmony_map[Harmony_dim] = "diminished";

	// here are list not default listed in MusicXML
	// 	harmony_map[Harmony_ADD9] = "major" ;
	// 	harmony_map[Harmony_MADD9] = "minor" ;
	// 	harmony_map[Harmony_7FLAT9] = "dominant" ;
	// 	harmony_map[Harmony_7SHARP9] = "dominant" ;
	// 	harmony_map[Harmony_7ADD13] = "dominant" ;
	// 	harmony_map[Harmony_7FLAT13] = "dominant" ;
	// 	harmony_map[Harmony_7SHARP5] = "dominant" ;
	// 	harmony_map[Harmony_7SHARP11] = "dominant" ;
	// 	harmony_map[Harmony_7SUS4] = "suspended-fourth" ;
	// 	harmony_map[Harmony_9SUS4] = "suspended-fourth" ;
	// 	harmony_map[Harmony_7FLAT5] = "dominant" ;

	return harmony_map[type];
}

QList<HarmonyWriter::DegreeNode> HarmonyWriter::harmonyTypeToDegrees(HarmonyType type) {
	QList<DegreeNode> degrees;
	DegreeNode degree;

	// 	Harmony_Dim7			= 0x0249,
	// 	Harmony_Aug7			= 0x0511,
	// 	Harmony_Min9b5		= 0x044D,
	// 	Harmony_Dominant7s9	= 0x0499,
	// 	Harmony_Dominant13	= 0x0615,

	switch (type) {
	case Harmony_maj:
	case Harmony_6:
	case Harmony_7:
	case Harmony_9:
	case Harmony_min:
	case Harmony_min7:
	case Harmony_min6:
	case Harmony_sus4:
	case Harmony_aug:
	case Harmony_dim: {
		break;
	}
	case Harmony_min7b5:
		//	case CHORD_7FLAT5 :
	{
		degree.value_ = 5;
		degree.alter_ = -1;
		degree.type_ = "alter";

		degrees.push_back(degree);

		break;
	}
		/*	case Harmony_ADD9 :
		 case Harmony_MADD9 :
		 {
		 degree.value_ = 9 ;
		 degree.alter_ = 0 ;
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 break ;
		 }
		 case Harmony_MIN7ADD11 :
		 {
		 degree.value_ = 9 ;
		 degree.alter_ = 0 ;
		 degree.type_ = "subtract" ;

		 degrees.push_back(degree) ;

		 break ;
		 }
		 case Harmony_7FLAT9 :
		 {
		 degree.value_ = 9 ;
		 degree.alter_ = -1 ;
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 break ;
		 }
		 case Harmony_7SHARP9 :
		 {
		 degree.value_ = 9 ;
		 degree.alter_ = 1 ;
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 break ;
		 }
		 case Harmony_7ADD13 :
		 {
		 degree.value_ = 13 ;
		 degree.alter_ = 0 ;
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 break ;
		 }
		 case Harmony_7FLAT13 :
		 {
		 degree.value_ = 13 ;
		 degree.alter_ = -1 ;
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 break ;
		 }
		 case Harmony_7SHARP5 :
		 {
		 degree.value_ = 5 ;
		 degree.alter_ = 1 ;
		 degree.type_ = "alter" ;

		 degrees.push_back(degree) ;

		 break ;
		 }
		 case Harmony_7SHARP11 :
		 {
		 degree.value_ = 11 ;
		 degree.alter_ = 1 ;
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 break ;
		 }*/
	case Harmony_7sus4: {
		degree.value_ = 7;
		degree.alter_ = 0; //������-1��Ҫ��finale����
		degree.type_ = "add";

		degrees.push_back(degree);

		break;
	}
		/*	case Harmony_9SUS4 :
		 {
		 degree.value_ = 7 ;
		 degree.alter_ = 0 ;	//������-1��Ҫ��finale����
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 degree.value_ = 9 ;
		 degree.alter_ = 0 ;
		 degree.type_ = "add" ;

		 degrees.push_back(degree) ;

		 break ;
		 }*/
	default:
		break;
	}

	return degrees;
}

//////////////////////////////////////////////////////////////////////////

BarBackupWriter::BarBackupWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), division_(1), duration_(0) {
	setName("backup");
}

void BarBackupWriter::createChildren() {
	WriterPtr child;

	child = XmlWriterFactory::createWriter("duration", numToName(duration_));
	children_.push_back(child);
}

void BarBackupWriter::setDivision(int division) {
	division_ = division;
}

void BarBackupWriter::setDuration(int duration) {
	duration_ = duration;
}

//////////////////////////////////////////////////////////////////////////

ForwardWriter::ForwardWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("forward");
}

void ForwardWriter::createChildren() {
	WriterPtr child;

	child = XmlWriterFactory::createWriter("duration", numToName(duration_));
	children_.push_back(child);
}

void ForwardWriter::setDuration(int duration) {
	duration_ = duration;
}

//////////////////////////////////////////////////////////////////////////

DirectionWriter::DirectionWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("direction");
}

void DirectionWriter::createChildren() {
	for (int i = 0; i < datas_.size(); ++i) {
		MusicData* data = datas_[i];

		attrs_[strPlacement] = data->getYOffset() >= 0 ? "below" : "above";

		DirectionTypeWriterPtr child = XmlWriterFactory::createDirectionType(
				buffer());
		child->addDirection(data, true);

		children_.push_back(child);

		if (data->getMusicDataType() == MusicData_Tempo) {
			Tempo* ptr = static_cast<Tempo*> (data);
			SoundWriterPtr sound = XmlWriterFactory::createSound(buffer());

			sound->setTempo(ptr);

			children_.push_back(sound);
		}
	}
}

void DirectionWriter::addDirection(MusicData* ptr) {
	datas_.push_back(ptr);
}

//////////////////////////////////////////////////////////////////////////
SoundWriter::SoundWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	tempo_ = 0;
	setName("sound");
}

void SoundWriter::setTempo(Tempo* ptr) {
	tempo_ = ptr;
}

void SoundWriter::collect() {
	if (tempo_ != 0) {
		attrs_["tempo"] = numToName(tempo_->getTypeTempo());
	}
}

//////////////////////////////////////////////////////////////////////////

BarlineWriter::BarlineWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), left_(true), barlineType_(Barline_Default), numericStart_(false), repeatType_(Repeat_Null) {
	setName("barline");
}

void BarlineWriter::collect() {
	attrs_["location"] = left_ ? "left" : "right";
}

void BarlineWriter::createChildren() {
	if (barlineType_ != Barline_Default) {
		QString strStyle = barlineToBarstyle(barlineType_, left_);
		WriterPtr child = XmlWriterFactory::createWriter("bar-style", strStyle);
		children_.push_back(child);
	}

    if (!numerics_.isEmpty()) {
		EndingWriterPtr ending = XmlWriterFactory::createEnding(buffer());
		ending->setStartStop(numericStart_);
		ending->setNumbers(numerics_);

		children_.push_back(ending);
	}

	if (barlineType_ == Barline_RepeatLeft || barlineType_
			== Barline_RepeatRight) {
		RepeatWriterPtr repeat = XmlWriterFactory::createRepeat(buffer());
		bool forward = (barlineType_ == Barline_RepeatLeft) ? true : false;

		repeat->setForward(forward);

		children_.push_back(repeat);
	}
}

void BarlineWriter::setLeftBarline(bool left) {
	left_ = left;
}

void BarlineWriter::setBarlineType(BarlineType barlineType) {
	barlineType_ = barlineType;
}

void BarlineWriter::setRepeatType(RepeatType repeatType) {
	repeatType_ = repeatType;
}

void BarlineWriter::setNumericEnding(bool start, const QList<int>& nums) {
	numerics_ = nums;
	numericStart_ = start;
}

QString BarlineWriter::barlineToBarstyle(BarlineType type, bool leftBarline) {
	QString bar_style = QString();

	switch (type) {
	case Barline_Default: {
		bar_style = "regular";
		break;
	}
	case Barline_Double: {
		bar_style = "light-light";
		break;
	}
	case Barline_Final: {
		bar_style = leftBarline ? "heavy-light" : "light-heavy";
		break;
	}
	case Barline_Null: {
		bar_style = "none";
		break;
	}
	case Barline_RepeatLeft: {
		bar_style = "heavy-light";
		break;
	}
	case Barline_RepeatRight: {
		bar_style = "light-heavy";
		break;
	}
	case Barline_Dashed: {
		bar_style = "dotted";
		break;
	}
	default:
		break;
	}

	return bar_style;
}

//////////////////////////////////////////////////////////////////////////
RestWriter::RestWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), noteType_(Note_Quarter), clefType_(Clef_Treble),
			createDisplay_(false) {
	setName("rest");
}

void RestWriter::setNote(Note* ptr) {
	note_ = ptr;
}

void RestWriter::setNoteType(NoteType type) {
	noteType_ = type;
}

void RestWriter::setClefType(ClefType type) {
	clefType_ = type;
}

void RestWriter::setCreateDisplay(bool create) {
	createDisplay_ = create;
}

void RestWriter::createChildren() {
	int defaultLine = restTypeToDefaultLine(noteType_);

	if (note_->getLine() == defaultLine) {
		return;
	}

}

int RestWriter::restTypeToDefaultLine(NoteType type) {
	int line = 0;

	switch (type) {
	case Note_DoubleWhole:
	case Note_Whole:
	case Note_Half:
	case Note_Quarter: {
		line = 0;
		break;
	}
	case Note_Eight: {
		line = 1;
		break;
	}
	case Note_Sixteen:
	case Note_32: {
		line = -1;
		break;
	}
	case Note_64: {
		line = -3;
		break;
	}
	case Note_128: {
		line = -4;
		break;
	}
	case Note_256: {
		line = -5;
		break;
	}
	default:
		break;
	}

	return line;
}

//////////////////////////////////////////////////////////////////////////
PitchWriter::PitchWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), line_(0), note_(60), clefType_(Clef_Treble) {
	setName("pitch");
}

void PitchWriter::createChildren() {
	WriterPtr child;

	ToneType clefMiddleTone;
	int clefMiddleOctave;

	getMiddleToneOctave(clefType_, clefMiddleTone, clefMiddleOctave);

	int absLine = (int) clefMiddleTone + clefMiddleOctave * OCTAVE + line_;
	int chromaticTone = absLine % OCTAVE;
	int octave = absLine / OCTAVE;
	int alter = accidentalToAlter(accidental_);

	child = XmlWriterFactory::createWriter("step", toneToString(chromaticTone));
	children_.push_back(child);

	if (alter != 0) {
		child = XmlWriterFactory::createWriter("alter", numToName(alter));
		children_.push_back(child);
	}

	child = XmlWriterFactory::createWriter("octave", numToName(octave));
	children_.push_back(child);
}

int PitchWriter::accidentalToAlter(AccidentalType type) {
	int alter = 0;

	switch (type) {
	case Accidental_Normal:
	case Accidental_Natural:
	case Accidental_Natural_Caution: {
		alter = 0;
		break;
	}
	case Accidental_Sharp:
	case Accidental_Sharp_Caution: {
		alter = 1;
		break;
	}
	case Accidental_Flat:
	case Accidental_Flat_Caution: {
		alter = -1;
		break;
	}
	case Accidental_DoubleSharp:
	case Accidental_DoubleSharp_Caution: {
		alter = 2;
		break;
	}
	case Accidental_DoubleFlat:
	case Accidental_DoubleFlat_Caution: {
		alter = -2;
		break;
	}
	default:
		break;
	}

	return alter;
}

void PitchWriter::getMiddleToneOctave(ClefType clef, ToneType& tone,
		int& octave) {
	tone = Tone_B;
	octave = 4;

	switch (clef) {
	case Clef_Treble: {
		tone = Tone_B;
		octave = 4;
		break;
	}
	case Clef_Treble8va: {
		tone = Tone_B;
		octave = 5;
		break;
	}
	case Clef_Treble8vb: {
		tone = Tone_B;
		octave = 3;
		break;
	}
	case Clef_Bass: {
		tone = Tone_D;
		octave = 3;
		break;
	}
	case Clef_Bass8va: {
		tone = Tone_D;
		octave = 4;
		break;
	}
	case Clef_Bass8vb: {
		tone = Tone_D;
		octave = 2;
		break;
	}
	case Clef_Alto: {
		tone = Tone_C;
		octave = 4;
		break;
	}
	case Clef_UpAlto: {
		tone = Tone_A;
		octave = 3;
		break;
	}
	case Clef_DownDownAlto: {
		tone = Tone_G;
		octave = 4;
		break;
	}
	case Clef_DownAlto: {
		tone = Tone_E;
		octave = 4;
		break;
	}
	case Clef_UpUpAlto: {
		tone = Tone_F;
		octave = 3;
		break;
	}
	default:
		break;
	}
}

QString PitchWriter::toneToString(int tone) {
	QString str = "C";

	switch (tone) {
	case 0: {
		str = "C";
		break;
	}
	case 1: {
		str = "D";
		break;
	}
	case 2: {
		str = "E";
		break;
	}
	case 3: {
		str = "F";
		break;
	}
	case 4: {
		str = "G";
		break;
	}
	case 5: {
		str = "A";
		break;
	}
	case 6: {
		str = "B";
		break;
	}
	default:
		break;
	}

	return str;
}

void PitchWriter::setLineNote(int line, int note) {
	line_ = line;
	note_ = note;//c����
}

void PitchWriter::setAccidental(AccidentalType accidental) {
	accidental_ = accidental;
}

void PitchWriter::setClefType(ClefType clefType) {
	clefType_ = clefType;
}

//////////////////////////////////////////////////////////////////////////

TieWriter::TieWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), start_(true) {
	setName("tie");
}

void TieWriter::collect() {
	QString type = start_ ? strStart : strStop;

	attrs_[strType] = type;
}

void TieWriter::setStart(bool start) {
	start_ = start;
}

//////////////////////////////////////////////////////////////////////////

TimeModificationWriter::TimeModificationWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), tuplet_(3) {
	setName("time-modification");
}

void TimeModificationWriter::createChildren() {
	WriterPtr child;

	child = XmlWriterFactory::createWriter("actual-notes", numToName(tuplet_));
	children_.push_back(child);

	child = XmlWriterFactory::createWriter("normal-notes", numToName(space_));
	children_.push_back(child);
}

void TimeModificationWriter::setTupletSpace(int tuplet, int space) {
	tuplet_ = tuplet;
	space_ = space;
}

//////////////////////////////////////////////////////////////////////////

NotationsWriter::NotationsWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("notations");
}

void NotationsWriter::createChildren() {
	int i;
	WriterPtr child;

	// tied | slur | tuplet | glissando 
	for (i = 0; i < datas_.size(); ++i) {
		QString strStartStop = datas_[i].second ? strStart : strStop;

		if (datas_[i].first->getMusicDataType() == MusicData_Tie) {
			child = XmlWriterFactory::createWriter("tied", QString(), strType, strStartStop);
			children_.push_back(child);
		} else {
			QString nameStr = QString();
			QString extraAttr = QString();
			QString extraAttrValue = QString();

			switch (datas_[i].first->getMusicDataType()) {
			case MusicData_Tuplet: {
				nameStr = "tuplet";
				extraAttr = strPlacement;
				extraAttrValue = "above";
				break;
			}
			case MusicData_Slur: {
				nameStr = "slur";
				break;
			}
			case MusicData_Glissando: {
				nameStr = "glissando";
				extraAttr = "line-type";
				extraAttrValue = "wavy";
				break;
			}
			default:
				break;
			}

			if (nameStr != QString()) {
				// startҪ�ȼ�¼��write��stopҪ��write�ټ�¼
				int number = buffer()->calculateStartStopNumber(datas_[i].first, datas_[i].second);

				child = XmlWriterFactory::createWriter(nameStr, QString(),
						strType, strStartStop, strNumber, numToName(number),
						extraAttr, extraAttrValue);

				children_.push_back(child);
			}
		}
	}

	// slide

	// ornaments
    if (!ornaments_.isEmpty()) {
		OrnamentsWriterPtr writer = XmlWriterFactory::createOrnaments(buffer());

		for (i = 0; i < ornaments_.size(); ++i) {
			writer->addOrnament(ornaments_[i]);
		}

		children_.push_back(writer);
	}

	// technical
    if (!technicals_.isEmpty()) {
		TechnicalWriterPtr writer = XmlWriterFactory::createTechnical(buffer());

		for (i = 0; i < technicals_.size(); ++i) {
			writer->addTechnical(technicals_[i]);
		}

		children_.push_back(writer);
	}

	// articulations
    if (!articulations_.isEmpty()) {
		ArticulationWriterPtr writer = XmlWriterFactory::createArticulation(buffer());

		for (i = 0; i < articulations_.size(); ++i) {
			writer->addArticulation(articulations_[i]);
		}

		children_.push_back(writer);
	}

	// dynamics, not exist here in ove

	// fermata
	for (i = 0; i < fermatas_.size(); ++i) {
		QString valueType = fermatas_[i]->getPlacementAbove() ? "upright" : "inverted";
		WriterPtr writer = XmlWriterFactory::createWriter("fermata", QString(), strType, valueType);
		children_.push_back(writer);
	}

	// arpeggiate
	for (i = 0; i < arpeggiates_.size(); ++i) {
		WriterPtr writer = XmlWriterFactory::createWriter("arpeggiate");
		children_.push_back(writer);
	}
}

void NotationsWriter::addDataAndStart(MusicData* data, bool start) {
	datas_.push_back(qMakePair(data, start));
}

void NotationsWriter::addOrnament(Articulation* art) {
	ornaments_.push_back(art);
}

void NotationsWriter::addTechnical(Articulation* art) {
	technicals_.push_back(art);
}

void NotationsWriter::addArticulation(Articulation* art) {
	articulations_.push_back(art);
}

void NotationsWriter::addDynamics(Articulation* art) {
	dynamics_.push_back(art);
}

void NotationsWriter::addFermata(Articulation* art) {
	fermatas_.push_back(art);
}

void NotationsWriter::addArpeggiate(Articulation* art) {
	arpeggiates_.push_back(art);
}

//////////////////////////////////////////////////////////////////////////

TiedWriter::TiedWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("tied");
}

void TiedWriter::collect() {
	QString type = tieStart_ ? strStart : strStop;

	attrs_[strType] = type;
}

void TiedWriter::setTieStart(bool start) {
	tieStart_ = start;
}

//////////////////////////////////////////////////////////////////////////

LyricWriter::LyricWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), verse_(0) {
	setName("lyric");
}

void LyricWriter::collect() {
	attrs_[strNumber] = numToName(verse_ + 1);
}

void LyricWriter::createChildren() {
	WriterPtr child;

	child = XmlWriterFactory::createWriter("syllabic", "single");
	children_.push_back(child);

	child = XmlWriterFactory::createWriter("text", text_);
	children_.push_back(child);
}

void LyricWriter::setVerse(int verse) {
	verse_ = verse;
}

void LyricWriter::setText(const QString& text) {
	text_ = text;
}

//////////////////////////////////////////////////////////////////////////
OrnamentsWriter::OrnamentsWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("ornaments");
}

void OrnamentsWriter::addOrnament(Articulation* art) {
	ornaments_.push_back(art);
}

void OrnamentsWriter::createChildren() {
	WriterPtr writer;

	for (int i = 0; i < ornaments_.size(); ++i) {
		QString str = ornamentTypeToString(ornaments_[i]->getArtType());
		QString value = ornamentTypeToValue(ornaments_[i]->getArtType());
		QString valuePlacement = ornaments_[i]->getPlacementAbove() ? "above" : "below";

		if (str != QString()) {
			if (ornaments_[i]->getArtType() == Articulation_Trill_Section) {
				writer = XmlWriterFactory::createWriter(str, value, strPlacement, valuePlacement, strType, strStart);
				children_.push_back(writer);
				writer = XmlWriterFactory::createWriter(str, value, strPlacement, valuePlacement, strType, strStop);
				children_.push_back(writer);
			} else {
				writer = XmlWriterFactory::createWriter(str, value, strPlacement, valuePlacement);
				children_.push_back(writer);
			}
		}
	}
}

QString OrnamentsWriter::ornamentTypeToString(ArticulationType type) {
	QString str = QString();

	switch (type) {
	case Articulation_Major_Trill:
	case Articulation_Minor_Trill: {
		str = "trill-mark";
		break;
	}
	case Articulation_Trill_Section: {
		str = "wavy-line";
		break;
	}
	case Articulation_Inverted_Short_Mordent:
	case Articulation_Inverted_Long_Mordent: {
		str = "inverted-mordent";
		break;
	}
	case Articulation_Short_Mordent: {
		str = "mordent";
		break;
	}
	case Articulation_Turn: {
		str = "turn";
		break;
	}
		//	case Articulation_Flat_Accidental_For_Trill :
		//	case Articulation_Sharp_Accidental_For_Trill :
		//	case Articulation_Natural_Accidental_For_Trill :
	case Articulation_Tremolo_Eighth:
	case Articulation_Tremolo_Sixteenth:
	case Articulation_Tremolo_Thirty_Second:
	case Articulation_Tremolo_Sixty_Fourth: {
		str = "tremolo";
		break;
	}
	default:
		break;
	}

	return str;
}

QString OrnamentsWriter::ornamentTypeToValue(ArticulationType type) {
	QString value = QString();

	switch (type) {
	case Articulation_Tremolo_Eighth: {
		value = "1";
		break;
	}
	case Articulation_Tremolo_Sixteenth: {
		value = "2";
		break;
	}
	case Articulation_Tremolo_Thirty_Second: {
		value = "3";
		break;
	}
	case Articulation_Tremolo_Sixty_Fourth: {
		value = "4";
		break;
	}
	default:
		break;
	}

	return value;
}

void OrnamentsWriter::ornamentToAttribute(Articulation* ptr, QString& attr, QString& attrValue) {
	attr = QString();
	attrValue = QString();

	switch (ptr->getArtType()) {
	//	case Articulation_Major_Trill :
	//	case Articulation_Minor_Trill :
	case Articulation_Trill_Section: {
		attr = "type";
		break;
	}
		/*	case Articulation_Inverted_Short_Mordent :
		 case Articulation_Inverted_Long_Mordent :
		 {
		 str = "inverted-mordent" ;
		 break ;
		 }
		 case Articulation_Short_Mordent :
		 {
		 str = "mordent" ;
		 break ;
		 }
		 case Articulation_Turn :
		 {
		 str = "turn" ;
		 break ;
		 }
		 //	case Articulation_Flat_Accidental_For_Trill :
		 //	case Articulation_Sharp_Accidental_For_Trill :
		 //	case Articulation_Natural_Accidental_For_Trill :
		 case Articulation_Tremolo_Eighth :
		 case Articulation_Tremolo_Sixteenth :
		 case Articulation_Tremolo_Thirty_Second :
		 case Articulation_Tremolo_Sixty_Fourth :
		 {
		 str = "tremolo" ;
		 break ;
		 }*/
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
TechnicalWriter::TechnicalWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("technical");
}

void TechnicalWriter::addTechnical(Articulation* art) {
	technicals_.push_back(art);
}

void TechnicalWriter::createChildren() {
	for (int i = 0; i < technicals_.size(); ++i) {
		QString str;
		QString value;
		QString valuePlacement = technicals_[i]->getPlacementAbove() ? "above" : "below";

		technicalToStringAndValue(technicals_[i]->getArtType(), str, value);

		if (str != QString()) {
			WriterPtr writer = XmlWriterFactory::createWriter(str, value, strPlacement, valuePlacement);

			children_.push_back(writer);
		}
	}
}

void TechnicalWriter::technicalToStringAndValue(ArticulationType type, QString& str, QString& value) {
	str = QString();
	value = QString();

	switch (type) {
	case Articulation_Up_Bow:
	case Articulation_Up_Bow_Inverted: {
		str = "up-bow";
		break;
	}
	case Articulation_Down_Bow:
	case Articulation_Down_Bow_Inverted: {
		str = "down-bow";
		break;
	}
	case Articulation_Natural_Harmonic:
	case Articulation_Artificial_Harmonic: {
		str = "harmonic";
		break;
	}
	case Articulation_Finger_1:
	case Articulation_Finger_2:
	case Articulation_Finger_3:
	case Articulation_Finger_4:
	case Articulation_Finger_5: {
		str = "fingering";
		break;
	}
	case Articulation_Plus_Sign: {
		str = "stopped";
		break;
	}
	default:
		break;
	}

	switch (type) {
	case Articulation_Finger_1: {
		value = "1";
		break;
	}
	case Articulation_Finger_2: {
		value = "2";
		break;
	}
	case Articulation_Finger_3: {
		value = "3";
		break;
	}
	case Articulation_Finger_4: {
		value = "4";
		break;
	}
	case Articulation_Finger_5: {
		value = "5";
		break;
	}
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
ArticulationWriter::ArticulationWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("articulations");
}

void ArticulationWriter::addArticulation(Articulation* art) {
	articulations_.push_back(art);
}

void ArticulationWriter::createChildren() {
	for (int i = 0; i < articulations_.size(); ++i) {
		QList<QString> strs = artTypeToStrings(articulations_[i]->getArtType());
		QString valuePlacement = articulations_[i]->getPlacementAbove() ? "above" : "below";

		for (int j = 0; j < strs.size(); ++j) {
			if (strs[j] == QString()) {
				continue;
			}

			WriterPtr writer = XmlWriterFactory::createWriter(strs[j], QString(), strPlacement, valuePlacement);

			children_.push_back(writer);
		}
	}
}

QList<QString> ArticulationWriter::artTypeToStrings(ArticulationType type) {
	QList<QString> strs;

	switch (type) {
	case Articulation_Marcato: {
		strs.push_back("accent");
		break;
	}
	case Articulation_Marcato_Dot: {
		strs.push_back("accent");
		strs.push_back("staccato");
		break;
	}
	case Articulation_Heavy_Attack: {
		strs.push_back("accent");
		strs.push_back("tenuto");
		break;
	}
	case Articulation_SForzando:
	case Articulation_SForzando_Inverted: {
		strs.push_back("strong-accent");
		break;
	}
	case Articulation_SForzando_Dot:
	case Articulation_SForzando_Dot_Inverted: {
		strs.push_back("strong-accent");
		strs.push_back("staccato");
		break;
	}
	case Articulation_Heavier_Attack: {
		strs.push_back("strong-accent");
		strs.push_back("tenuto");
		break;
	}
	case Articulation_Staccatissimo: {
		strs.push_back("staccatissimo");
		break;
	}
	case Articulation_Staccato: {
		strs.push_back("staccato");
		break;
	}
	case Articulation_Tenuto: {
		strs.push_back("tenuto");
		break;
	}
	case Articulation_Pause: {
		strs.push_back("breath-mark");
		break;
	}
	case Articulation_Grand_Pause: {
		strs.push_back("caesura");
		break;
	}
	default:
		break;
	}

	return strs;
}

//////////////////////////////////////////////////////////////////////////
DynamicsWriter::DynamicsWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("dynamics");
}

void DynamicsWriter::setDynamics(Dynamics* ptr) {
	dynamics_ = ptr;
}

void DynamicsWriter::createChildren() {
	QString str = dynamicsTypeToStrings(dynamics_->getDynamicsType());

	if (str != QString()) {
		WriterPtr writer = XmlWriterFactory::createWriter(str, QString());

		children_.push_back(writer);
	}
}

QString DynamicsWriter::dynamicsTypeToStrings(DynamicsType type) {
	QString str = QString();

	switch (type) {
	case Dynamics_pppp: {
		str = "pppp";
		break;
	}
	case Dynamics_ppp: {
		str = "ppp";
		break;
	}
	case Dynamics_pp: {
		str = "pp";
		break;
	}
	case Dynamics_p: {
		str = "p";
		break;
	}
	case Dynamics_mp: {
		str = "mp";
		break;
	}
	case Dynamics_mf: {
		str = "mf";
		break;
	}
	case Dynamics_f: {
		str = "f";
		break;
	}
	case Dynamics_ff: {
		str = "ff";
		break;
	}
	case Dynamics_fff: {
		str = "fff";
		break;
	}
	case Dynamics_ffff: {
		str = "ffff";
		break;
	}
	case Dynamics_sf: {
		str = "sf";
		break;
	}
	case Dynamics_fz: {
		str = "fz";
		break;
	}
	case Dynamics_sfz: {
		str = "sfz";
		break;
	}
	case Dynamics_sffz: {
		str = "sffz";
		break;
	}
	case Dynamics_fp: {
		str = "fp";
		break;
	}
	case Dynamics_sfp: {
		str = "sfp";
		break;
	}
	default:
		break;
	}

	return str;
}

//////////////////////////////////////////////////////////////////////////
RootWriter::RootWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), root_(0), alter_(0) {
	setName("root");
}

void RootWriter::collect() {
	step_ = note_2_name(root_);
}

void RootWriter::createChildren() {
	WriterPtr child;

	child = XmlWriterFactory::createWriter("root-step", step_);
	children_.push_back(child);

	if (alter_ != 0) {
		child = XmlWriterFactory::createWriter("root-alter", numToName(alter_));
		children_.push_back(child);
	}
}

void RootWriter::setRoot(int root) {
	root_ = root;
}

void RootWriter::setAlter(int alter) {
	alter_ = alter;
}

//////////////////////////////////////////////////////////////////////////

BassWriter::BassWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), bass_(0), alter_(0) {
	setName("bass");
}

void BassWriter::collect() {
	step_ = note_2_name(bass_);
}

void BassWriter::createChildren() {
	WriterPtr child = XmlWriterFactory::createWriter("bass-step", step_);
	children_.push_back(child);

	if (alter_ != 0) {
		child = XmlWriterFactory::createWriter("bass-alter", numToName(alter_));
		children_.push_back(child);
	}
}

void BassWriter::setBass(int bass) {
	bass_ = bass;
}

void BassWriter::setAlter(int alter) {
	alter_ = alter;
}

//////////////////////////////////////////////////////////////////////////

DegreeWriter::DegreeWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), degreeValue_(0), alter_(0) {
	setName("degree");
}

void DegreeWriter::createChildren() {
	WriterPtr child;

	child = XmlWriterFactory::createWriter("degree-value", numToName(degreeValue_));
	children_.push_back(child);

	child = XmlWriterFactory::createWriter("degree-alter", numToName(alter_));
	children_.push_back(child);

	Q_ASSERT(!type_.isEmpty());
	child = XmlWriterFactory::createWriter("degree-type", type_);
	children_.push_back(child);
}

void DegreeWriter::setValue(int degreeValue) {
	degreeValue_ = degreeValue;
}

void DegreeWriter::setAlter(int alter) {
	alter_ = alter;
}

void DegreeWriter::setType(QString type) {
	type_ = type;
}

//////////////////////////////////////////////////////////////////////////

DirectionTypeWriter::DirectionTypeWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("direction-type");
}

void DirectionTypeWriter::createChildren() {
	for (int i = 0; i < directions_.size(); ++i) {
		MusicDataType type = directions_[i]->getMusicDataType();

		switch (type) {
		case MusicData_Text: {
			Text* ptr = static_cast<Text*> (directions_[i]);
			createDirectionText(ptr);

			break;
		}
		case MusicData_Repeat: {
			RepeatSymbol* repeat = static_cast<RepeatSymbol*> (directions_[i]);
			createDirectionRepeat(repeat);

			break;
		}
		case MusicData_Wedge_EndPoint: {
			WedgeEndPoint* ptr = static_cast<WedgeEndPoint*> (directions_[i]);
			createDirectionWedge(ptr);

			break;
		}
		case MusicData_Dynamics: {
			Dynamics* ptr = static_cast<Dynamics*> (directions_[i]);
			createDirectionDynamics(ptr);

			break;
		}
		case MusicData_Pedal: {
			//Pedal* ptr = static_cast<Pedal*>(directions_[i]) ;

			break;
		}
		case MusicData_OctaveShift_EndPoint: {
			OctaveShiftEndPoint* ptr = static_cast<OctaveShiftEndPoint*> (directions_[i]);
			createDirectionOctaveShift(ptr);

			break;
		}
		case MusicData_Decorator: {
			Decorator* ptr = static_cast<Decorator*> (directions_[i]);

			if (ptr->getDecoratorType() == Decorator::Decorator_Articulation) {
				ArticulationType artType = ptr->getArticulationType();

				if (artType == Articulation_Pedal_Down || artType == Articulation_Pedal_Up) {
					createDirectionPianoPedal(false, artType == Articulation_Pedal_Down);
				}
			}

			break;
		}
		case MusicData_Tempo: {
			Tempo* ptr = static_cast<Tempo*> (directions_[i]);
			createDirectionTempo(ptr);

			break;
		}
		case MusicData_Harp_Pedal: {
			//HarpPedal* ptr = static_cast<HarpPedal*> (directions_[i]);

			break;
		}
		case MusicData_Expressions: {
			Expressions* ptr = static_cast<Expressions*> (directions_[i]);
			createDirectionExpressions(ptr);

			break;
		}
		default:
			break;
		}
	}
}

void DirectionTypeWriter::createDirectionText(Text* ptr) {
	if (ptr->getTextType() == Text::Text_Rehearsal) {
		WriterPtr child = XmlWriterFactory::createWriter("rehearsal", ptr->getText());
		children_.push_back(child);
	} else //Text_SystemText, Text_MeasureText
	{
		WriterPtr child = XmlWriterFactory::createWriter("words", ptr->getText());
		children_.push_back(child);
	}
}

void DirectionTypeWriter::createDirectionWedge(WedgeEndPoint* ptr) {
	QString attrTypeValue = "stop";

	if (ptr->getWedgeStart()) {
		switch (ptr->getWedgeType()) {
		case Wedge_Cres_Line: {
			attrTypeValue = "crescendo";
			break;
		}
		case Wedge_Decresc_Line: {
			attrTypeValue = "diminuendo";
			break;
		}
		default:
			break;
		}
	}

	//int number = buffer()->calculate_start_stop_number(ptr, ptr->get_wedge_start()) ;

	WriterPtr child = XmlWriterFactory::createWriter("wedge", QString(), //strNumber, num_2_name(number),
			strType, attrTypeValue);
	children_.push_back(child);
}

void DirectionTypeWriter::createDirectionDynamics(Dynamics* ptr) {
	DynamicsWriterPtr writer = XmlWriterFactory::createDynamics(buffer());

	writer->setDynamics(ptr);
	children_.push_back(writer);
}

void DirectionTypeWriter::createDirectionPianoPedal(bool line, bool start) {
	QString valueType = start ? strStart : strStop;
	QString valueLine = line ? "yes" : "no";

	WriterPtr child = XmlWriterFactory::createWriter("pedal", QString(), strType, valueType, "line", valueLine);
	children_.push_back(child);
}

void DirectionTypeWriter::createDirectionOctaveShift(OctaveShiftEndPoint* ptr) {
	OctaveShiftPosition position = ptr->getOctaveShiftPosition();

	if (position == OctavePosition_Continue) {
		return;
	}

	OctaveShiftType shiftType = ptr->getOctaveShiftType();
	QString typeValue = "stop";
	QString sizeValue = "8";

	if (position == OctavePosition_Start) {
		switch (shiftType) {
		case OctaveShift_8:
		case OctaveShift_15: {
			typeValue = "down";
			break;
		}
		case OctaveShift_Minus_8:
		case OctaveShift_Minus_15: {
			typeValue = "up";
			break;
		}
		default:
			break;
		}
	} else {
		typeValue = "stop";
	}

	switch (shiftType) {
	case OctaveShift_8:
	case OctaveShift_Minus_8: {
		sizeValue = "8";
		break;
	}
	case OctaveShift_15:
	case OctaveShift_Minus_15: {
		sizeValue = "15";
		break;
	}
	default:
		break;
	}

	WriterPtr child = XmlWriterFactory::createWriter("octave-shift", QString(),
			strType, typeValue, "size", sizeValue);
	children_.push_back(child);
}

void DirectionTypeWriter::createDirectionExpressions(Expressions* ptr) {
	QString text = ptr->getText();

	if (!text.isEmpty()) {
		WriterPtr child = XmlWriterFactory::createWriter("words", text);
		children_.push_back(child);
	}
}

void DirectionTypeWriter::createDirectionTempo(Tempo* ptr) {
	MetronomeWriterPtr writer = XmlWriterFactory::createMetronome(buffer());

	writer->setBeatUnit(ptr->getLeftNoteType());
	writer->setPerMinute(ptr->getTypeTempo());
	children_.push_back(writer);
}

void DirectionTypeWriter::createDirectionRepeat(RepeatSymbol* repeat) {
	WriterPtr child;
	RepeatType type = repeat->getRepeatType();

	switch (type) {
	case Repeat_Segno: {
		child = XmlWriterFactory::createWriter("segno", QString());
		children_.push_back(child);
		break;
	}
	case Repeat_Coda: {
		child = XmlWriterFactory::createWriter("coda", QString());
		children_.push_back(child);
		break;
	}
	case Repeat_DSAlCoda:
	case Repeat_DSAlFine:
	case Repeat_DCAlCoda:
	case Repeat_DCAlFine:
	case Repeat_ToCoda:
	case Repeat_Fine: {
		child = XmlWriterFactory::createWriter("words", getRepeatWords(type), "font-style", "italic");
		children_.push_back(child);
		break;
	}
	default:
		break;
	}
}

QString DirectionTypeWriter::getRepeatWords(RepeatType type) {
	QString str = QString();

	switch (type) {
	case Repeat_ToCoda: {
		str = "To Coda ";
		break;
	}
	case Repeat_DCAlCoda: {
		str = "D.C. al Coda";
		break;
	}
	case Repeat_DCAlFine: {
		str = "D.C. al Fine";
		break;
	}
	case Repeat_DSAlCoda: {
		str = "D.S. al Coda";
		break;
	}
	case Repeat_DSAlFine: {
		str = "D.S. al Fine";
		break;
	}
	case Repeat_Fine: {
		str = "Fine";
		break;
	}
	default:
		Q_ASSERT(false);
	}

	return str;
}

void DirectionTypeWriter::addDirection(MusicData* direction, bool start) {
	directions_.push_back(direction);
	starts_.push_back(start);
}

//////////////////////////////////////////////////////////////////////////
MetronomeWriter::MetronomeWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), beatUnit_(Note_Quarter), perMinute_(100) {
	setName("metronome");
}

void MetronomeWriter::setBeatUnit(NoteType beatUnit) {
	beatUnit_ = beatUnit;
}

void MetronomeWriter::setPerMinute(int perMinute) {
	perMinute_ = perMinute;
}

void MetronomeWriter::createChildren() {
	WriterPtr writer;

	writer = XmlWriterFactory::createWriter("beat-unit", NoteTypeToString(beatUnit_));
	children_.push_back(writer);

	writer = XmlWriterFactory::createWriter("per-minute", numToName(perMinute_));
	children_.push_back(writer);
}

//////////////////////////////////////////////////////////////////////////
EndingWriter::EndingWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer) {
	setName("ending");
}

void EndingWriter::collect() {
	QString strNumverValue;

	attrs_[strType] = start_ ? strStart : strStop;

	for (int i = 0; i < numbers_.size(); ++i) {
		strNumverValue += numToName(numbers_[i]);
		if (i != numbers_.size() - 1) {
			strNumverValue += ", ";
		}
	}
	attrs_[strNumber] = strNumverValue;
}

void EndingWriter::setStartStop(bool start) {
	start_ = start;
}

void EndingWriter::setNumbers(const QList<int>& numbers) {
	numbers_ = numbers;
}

//////////////////////////////////////////////////////////////////////////

RepeatWriter::RepeatWriter(XmlBuffer* xmlBuffer) :
	Writer(xmlBuffer), forward_(true) {
	setName("repeat");
}

void RepeatWriter::collect() {
	QString direction = forward_ ? "forward" : "backward";
	attrs_["direction"] = direction;
}

void RepeatWriter::setForward(bool forward) {
	forward_ = forward;
}

//////////////////////////////////////////////////////////////////////////

}
