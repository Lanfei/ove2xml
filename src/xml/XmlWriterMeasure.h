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

#ifndef XML_WRITERS_H
#define XML_WRITERS_H

#include "XmlWriter.h"
#include "ove.h"
#include <QList>

using namespace OVE;

namespace XML {

//////////////////////////////////////////////////////////////////////////

enum BarChildType {
	NODE_NOTE = 0, NODE_REST, NODE_Harmony
};

struct NoteIndex {
	bool rest_;
	BarChildType type_;//true��ʾ��note�У�false��ʾ��rest��
	int id_; //��note��rest�����е�λ��
	int pos_;
};

class MeasureWriter: public Writer {
public:
	MeasureWriter(XmlBuffer* xmlBuffer);
	virtual ~MeasureWriter() {}

public:
	void setNumber(int number);
	void setDivision(int division);

private:
	virtual void collect();
	virtual void createChildren();

private:
	void createBarline(bool left_barline);
	void createAttributes();
	void createSoundTempo(void);
	void createMeasuresMusicDatas();
	void createMeasureMusicDatas(MeasureData* bar, int staff);
	bool canCreateDirection(MusicData* ptr);

	void createBarBackup();
	void createEmptyBarRest(unsigned int voice, int staff);
	//note����һstaff��ǩ��ֻ��Clef_Treble && Clef_Bass��ɵĸߵ����ױ��³���
	void createNotes(MeasureData* measureData, NoteContainer* slot, int staff);

	int getBarDuration(int bar, int division);

	void buildClefs();
	ClefType checkClefType(int staff, int unit);
	bool createStaff() const;

private:
	int division_;
	int number_;

	Measure* measure_;
	QList<MeasureData*> measureDatas_;
	QList<QList<Clef*> > clefss_;
};

//////////////////////////////////////////////////////////////////////////

class AttributeWriter: public Writer {
public:
	AttributeWriter(XmlBuffer* xmlBuffer);
	virtual ~AttributeWriter() {}

public:
	void setCreateDivision(bool create, int division);
	void setCreateKey(bool create, Key* keyPtr);
	void setCreateTime(bool create);
	void setCreateClef(bool create, int staff);
	void setCreateStaves(bool create, int staves);
	void addClef(ClefType clefType, int staff, bool createNumber);

	void addMeasureStyle(MusicData* ptr, bool start);

private:
	virtual void createChildren();

private:
	QList<QPair<MusicData*, bool> > measureStyles_;

	bool createDivision_;
	int division_;
	bool createKey_;
	Key* keyPtr_;
	bool createTime_;
	bool createStaves_;
	bool createClef_;
	int staff_;
	int staves_;

	struct ClefNode {
		ClefType type_;
		int staff_;
		bool createNumber_;

		ClefNode() :
			type_(Clef_Treble), staff_(0), createNumber_(false) {
		}
	};
	QList<ClefNode> clefs_;
};

//////////////////////////////////////////////////////////////////////////

class KeyWriter: public Writer {
public:
	KeyWriter(XmlBuffer* xmlBuffer);
	virtual ~KeyWriter() {}

public:
	void setKey(Key* ptr);

private:
	virtual void createChildren();

private:
	Key* key_;
};

class TimeWriter: public Writer {
public:
	TimeWriter(XmlBuffer* xmlBuffer);
	virtual ~TimeWriter() {}

private:
	virtual void createChildren();
};

class ClefWriter: public Writer {
public:
	ClefWriter(XmlBuffer* xmlBuffer);
	virtual ~ClefWriter() {}

public:
	void setClefNumber(int number, bool create);
	void setClefType(ClefType type);

private:
	virtual void collect();
	virtual void createChildren();

private:
	bool createNumber_;
	int number_;
	ClefType type_;
};

class MeasureStyleWriter: public Writer {
public:
	MeasureStyleWriter(XmlBuffer* xmlBuffer);
	virtual ~MeasureStyleWriter() {}

public:
	void setRepeat(int styleValue, bool start);
	void setMultiRestCount(int count);

private:
	virtual void createChildren();

private:
	bool setRepeat_;
	bool start_;
	int styleValue_;
	int multiRestCount_;
};

//////////////////////////////////////////////////////////////////////////

class NoteWriter: public Writer {
public:
	NoteWriter(XmlBuffer* xmlBuffer);
	virtual ~NoteWriter() {}

public:
	void setMeasure(Measure* measure);
	void setMeasureData(MeasureData* measureData);
	void setContainer(NoteContainer* container);
	void setNote(Note* note);
	void setDivision(int division);
	void setCreateChord(bool create);
	void setStartTuplet(bool start);
	void setCreateStaff(bool create, int staff);
	void setClefType(ClefType type);

private:
	virtual void collect();
	virtual void createChildren();

	void createGrace();
	void createCue();
	void createFullNote();
	void createDuration();
	void createTie();
	void createVoice();
	void createTypeWriter();
	void createDot();
	void createAccidental();
	void createTimeModification();
	void createStem();
	void createNotehead();
	void createStaff();
	void createBeam();
	void createNotations();
	void createLyric();

	// notations
	void createNotationsPairElements(const QList<MusicData*>& pairs);
	void createNotationsTied();
	void createNotationsSlur();
	void createNotationsTuplet();
	void createNotationsGlissando();
	void createNotationsSlide();
	void createNotationsOrnaments();
	void createNotationsTechnical();
	void createNotationsArticulations();
	void createNotationsDynamics();
	void createNotationsFermata();
	void createNotationsArpeggiate();

private:
	int division_;
	bool startTuplet_;
	bool createStaff_;
	bool createChord_;
	int staff_;
	ClefType clefType_;

	Note* note_;
	NoteContainer* container_;
	Measure* measure_;
	MeasureData* measureData_;

	QList<QPair<MusicData*, bool> > ties_;

	enum BeamType {
		Beam_begin,
		Beam_continue,
		Beam_end,
		Beam_forward_hook,
		Beam_backward_hook
	};
	QList<BeamType> beams_;
};

class BarRestWriter: public Writer {
public:
	BarRestWriter(XmlBuffer* xmlBuffer);
	virtual ~BarRestWriter() {}

public:
	void setDuration(int duration);
	void setVoice(unsigned int voice);
	void setStaff(int staff);//if not set, then not create

private:
	virtual void createChildren();

private:
	int duration_;
	unsigned int voice_;
	int staff_;
	bool createStaff_;
};

class HarmonyWriter: public Writer {
public:
	HarmonyWriter(XmlBuffer* xmlBuffer);
	virtual ~HarmonyWriter() {}

public:
	void setHarmony(Harmony* harmony);

private:
	virtual void collect();
	virtual void createChildren();

	QString harmonyTypeToKind(HarmonyType type);

	struct DegreeNode {
		int value_;
		int alter_;
		QString type_; //add, alter, subtract

		DegreeNode() :
			value_(0), alter_(0), type_(QString()) {
		}
	};
	QList<DegreeNode> harmonyTypeToDegrees(HarmonyType type);

private:
	Harmony* harmony_;
	QString kind_;
	QList<DegreeNode> degrees_;
};

class BarBackupWriter: public Writer {
public:
	BarBackupWriter(XmlBuffer* xmlBuffer);
	virtual ~BarBackupWriter() {}

public:
	void setDivision(int division);
	void setDuration(int duration);

private:
	virtual void createChildren();

private:
	int division_;
	int duration_;
};

class ForwardWriter: public Writer {
public:
	ForwardWriter(XmlBuffer* xmlBuffer);
	virtual ~ForwardWriter() {}

public:
	void setDuration(int duration);

private:
	virtual void createChildren();

private:
	int duration_;
};

class DirectionWriter: public Writer {
public:
	DirectionWriter(XmlBuffer* xmlBuffer);
	virtual ~DirectionWriter() {}

public:
	void addDirection(MusicData* ptr);

private:
	virtual void createChildren();

private:
	QList<MusicData*> datas_;
};

class SoundWriter: public Writer {
public:
	SoundWriter(XmlBuffer* xmlBuffer);
	virtual ~SoundWriter() {}

public:
	void setTempo(Tempo* ptr);

private:
	virtual void collect();

private:
	Tempo* tempo_;
};

class BarlineWriter: public Writer {
public:
	BarlineWriter(XmlBuffer* xmlBuffer);
	virtual ~BarlineWriter() {}

public:
	void setLeftBarline(bool left);
	void setBarlineType(BarlineType barlineType);
	void setRepeatType(RepeatType repeatType);
	void setNumericEnding(bool start, const QList<int>& nums);

private:
	virtual void collect();
	virtual void createChildren();

	QString barlineToBarstyle(BarlineType type, bool leftBarline);

private:
	bool left_;
	BarlineType barlineType_;

	QList<int> numerics_;
	bool numericStart_;

	RepeatType repeatType_;
};

//////////////////////////////////////////////////////////////////////////
// note children

class RestWriter: public Writer {
public:
	RestWriter(XmlBuffer* xmlBuffer);
	virtual ~RestWriter() {}

public:
	void setNote(Note* ptr);
	void setNoteType(NoteType type);
	void setClefType(ClefType type);
	void setCreateDisplay(bool create);

private:
	virtual void createChildren();
	int restTypeToDefaultLine(NoteType type);

private:
	Note* note_;
	NoteType noteType_;
	ClefType clefType_;
	bool createDisplay_;
};

class PitchWriter: public Writer {
public:
	PitchWriter(XmlBuffer* xmlBuffer);
	virtual ~PitchWriter() {}

public:
	void setLineNote(int line, int note);
	void setAccidental(AccidentalType accidental);
	void setClefType(ClefType clefType);

private:
	virtual void createChildren();

	static void getMiddleToneOctave(ClefType clef, ToneType& tone, int& octave);
	QString toneToString(int tone);
	int accidentalToAlter(AccidentalType type);

private:
	int line_;
	int note_;
	AccidentalType accidental_;
	ClefType clefType_;
};

class TieWriter: public Writer {
public:
	TieWriter(XmlBuffer* xmlBuffer);
	virtual ~TieWriter() {}

public:
	void setStart(bool start);

private:
	virtual void collect();

private:
	bool start_;
};

class TimeModificationWriter: public Writer {
public:
	TimeModificationWriter(XmlBuffer* xmlBuffer);
	virtual ~TimeModificationWriter() {}

public:
	void setTupletSpace(int tuplet, int space);

private:
	virtual void createChildren();

private:
	int tuplet_;
	int space_;
};

class NotationsWriter: public Writer {
public:
	NotationsWriter(XmlBuffer* xmlBuffer);
	virtual ~NotationsWriter() {}

public:
	void addDataAndStart(MusicData* data, bool start);
	void addOrnament(Articulation* art);
	void addTechnical(Articulation* art);
	void addArticulation(Articulation* art);
	void addDynamics(Articulation* art);
	void addFermata(Articulation* art);
	void addArpeggiate(Articulation* art);

private:
	virtual void createChildren();

private:
	QList<QPair<MusicData*, bool> > datas_;
	QList<Articulation*> ornaments_;
	QList<Articulation*> technicals_;
	QList<Articulation*> articulations_;
	QList<Articulation*> dynamics_;
	QList<Articulation*> fermatas_;
	QList<Articulation*> arpeggiates_;
};

class TiedWriter: public Writer {
public:
	TiedWriter(XmlBuffer* xmlBuffer);
	virtual ~TiedWriter() {}

public:
	void setTieStart(bool start);

private:
	virtual void collect();

private:
	bool tieStart_;
};

class LyricWriter: public Writer {
public:
	LyricWriter(XmlBuffer* xmlBuffer);
	virtual ~LyricWriter() {}

public:
	void setVerse(int verse);
	void setText(const QString& text);

private:
	virtual void collect();
	virtual void createChildren();

private:
	int verse_;
	QString text_;
};

class OrnamentsWriter: public Writer {
public:
	OrnamentsWriter(XmlBuffer* xmlBuffer);
	virtual ~OrnamentsWriter() {}

public:
	void addOrnament(Articulation* art);

private:
	virtual void createChildren();

	QString ornamentTypeToString(ArticulationType type);
	QString ornamentTypeToValue(ArticulationType type);
	void ornamentToAttribute(Articulation* ptr, QString& attr,	QString& attrValue);

private:
	QList<Articulation*> ornaments_;
};

class TechnicalWriter: public Writer {
public:
	TechnicalWriter(XmlBuffer* xmlBuffer);
	virtual ~TechnicalWriter() {}

public:
	void addTechnical(Articulation* art);

private:
	virtual void createChildren();

	void technicalToStringAndValue(ArticulationType type, QString& str,	QString& value);

private:
	QList<Articulation*> technicals_;
};

class ArticulationWriter: public Writer {
public:
	ArticulationWriter(XmlBuffer* xmlBuffer);
	virtual ~ArticulationWriter() {}

public:
	void addArticulation(Articulation* art);

private:
	virtual void createChildren();

	QList<QString> artTypeToStrings(ArticulationType type);

private:
	QList<Articulation*> articulations_;
};

class DynamicsWriter: public Writer {
public:
	DynamicsWriter(XmlBuffer* xmlBuffer);
	virtual ~DynamicsWriter() {}

public:
	void setDynamics(Dynamics* ptr);

private:
	virtual void createChildren();

	QString dynamicsTypeToStrings(DynamicsType type);

private:
	Dynamics* dynamics_;
};

//////////////////////////////////////////////////////////////////////////
// harmony child

class RootWriter: public Writer {
public:
	RootWriter(XmlBuffer* xmlBuffer);
	virtual ~RootWriter() {}

public:
	void setRoot(int root);
	void setAlter(int alter);

private:
	virtual void collect();
	virtual void createChildren();

private:
	int root_;
	int alter_;
	QString step_;
};

class BassWriter: public Writer {
public:
	BassWriter(XmlBuffer* xmlBuffer);
	virtual ~BassWriter() {}

public:
	void setBass(int bass);
	void setAlter(int alter);

private:
	virtual void collect();
	virtual void createChildren();

private:
	int bass_;
	int alter_;
	QString step_;
};

class DegreeWriter: public Writer {
public:
	DegreeWriter(XmlBuffer* xmlBuffer);
	virtual ~DegreeWriter() {}

public:
	void setValue(int degreeValue);
	void setAlter(int alter);
	void setType(QString type);

private:
	virtual void createChildren();

private:
	int degreeValue_;
	int alter_;
	QString type_;
};

//////////////////////////////////////////////////////////////////////////
// direction child

class DirectionTypeWriter: public Writer {
public:
	DirectionTypeWriter(XmlBuffer* xmlBuffer);
	virtual ~DirectionTypeWriter() {}

public:
	void addDirection(MusicData* direction, bool start);

private:
	virtual void createChildren();

	void createDirectionRepeat(RepeatSymbol* repeat);
	QString getRepeatWords(RepeatType type);

	void createDirectionText(Text* ptr);
	void createDirectionWedge(WedgeEndPoint* ptr);
	void createDirectionDynamics(Dynamics* ptr);
	void createDirectionPianoPedal(bool line, bool start);
	void createDirectionOctaveShift(OctaveShiftEndPoint* ptr);
	void createDirectionTempo(Tempo* ptr);
	void createDirectionExpressions(Expressions* ptr);

private:
	QList<MusicData*> directions_;
	QList<bool> starts_;
};

class MetronomeWriter: public Writer {
public:
	MetronomeWriter(XmlBuffer* xmlBuffer);
	virtual ~MetronomeWriter() {}

public:
	void setBeatUnit(NoteType beatUnit);
	void setPerMinute(int perMinute);

private:
	virtual void createChildren();

private:
	NoteType beatUnit_;
	int perMinute_;
};

//////////////////////////////////////////////////////////////////////////
// barline child

class EndingWriter: public Writer {
public:
	EndingWriter(XmlBuffer* xmlBuffer);
	virtual ~EndingWriter() {}

public:
	void setStartStop(bool start);
	void setNumbers(const QList<int>& numbers);

private:
	virtual void collect();

private:
	bool start_;
	QList<int> numbers_;
};

class RepeatWriter: public Writer {
public:
	RepeatWriter(XmlBuffer* xmlBuffer);
	virtual ~RepeatWriter() {}

public:
	void setForward(bool forward);

private:
	bool forward_;

private:
	virtual void collect();
};

}

#endif
