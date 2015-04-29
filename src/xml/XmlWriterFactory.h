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

#ifndef XML_WRITER_FACTORY_H
#define XML_WRITER_FACTORY_H

#include <QString>
#include "XmlWriterPtr.h"

namespace XML {

class XmlBuffer;

class XmlWriterFactory {
public:
	XmlWriterFactory();
	~XmlWriterFactory();

public:
	static WriterPtr createWriter(
						const QString& name, const QString& value = QString(),
						const QString& attr_name = QString(), const QString& attr_value = QString(),
						const QString& attr_name2 =	QString(), const QString& attr_value2 = QString(),
						const QString& attr_name3 = QString(), const QString& attr_value3 =	QString());

	// structure
	static ScorePartwiseWriterPtr createScorePartwise(XmlBuffer* xmlBuffer);
	static MovementTitleWriterPtr createMovementTitle(XmlBuffer* xmlBuffer);
	static IndentificationWriterPtr createIndentification(XmlBuffer* xmlBuffer);
	static DefaultsWriterPtr createDefaults(XmlBuffer* xmlBuffer);
	static EncodingWriterPtr createEncoding(XmlBuffer* xmlBuffer);
	static ScalingWriterPtr createScaling(XmlBuffer* xmlBuffer);
	static PageLayoutWriterPtr createPageLayout(XmlBuffer* xmlBuffer);
	static PageMarginsWriterPtr createPageMargins(XmlBuffer* xmlBuffer);
	static PartlistWriterPtr createPartlist(XmlBuffer* xmlBuffer);
	static ScorePartWriterPtr createScorePart(XmlBuffer* xmlBuffer);
	static ScoreInstrumentWriterPtr createScoreInstrument(XmlBuffer* xmlBuffer);
	static MidiInstrumentWriterPtr createMidiInstrument(XmlBuffer* xmlBuffer);
	static PartWriterPtr createPart(XmlBuffer* xmlBuffer);

	// measure
	static MeasureWriterPtr createMeasure(XmlBuffer* xmlBuffer);
	static AttributeWriterPtr createAttribute(XmlBuffer* xmlBuffer);
	static NoteWriterPtr createNote(XmlBuffer* xmlBuffer);
	static BarRestWriterPtr createBarRest(XmlBuffer* xmlBuffer);
	static BarBackupWriterPtr createBarBackup(XmlBuffer* xmlBuffer);
	static HarmonyWriterPtr createHarmony(XmlBuffer* xmlBuffer);
	static BarlineWriterPtr createBarline(XmlBuffer* xmlBuffer);
	static DirectionWriterPtr createDirection(XmlBuffer* xmlBuffer);
	static SoundWriterPtr createSound(XmlBuffer* xmlBuffer);
	static ClefWriterPtr createClef(XmlBuffer* xmlBuffer);
	static KeyWriterPtr createKey(XmlBuffer* xmlBuffer);
	static TimeWriterPtr createTime(XmlBuffer* xmlBuffer);
	static MeasureStyleWriterPtr createMeasureStyle(XmlBuffer* xmlBuffer);
	static RestWriterPtr createRest(XmlBuffer* xmlBuffer);
	static PitchWriterPtr createPitch(XmlBuffer* xmlBuffer);
	static TieWriterPtr createTie(XmlBuffer* xmlBuffer);
	static TimeModificationWriterPtr createTimeModification(XmlBuffer* xmlBuffer);
	static NotationsWriterPtr createNotations(XmlBuffer* xmlBuffer);
	static LyricWriterPtr createLyric(XmlBuffer* xmlBuffer);
	static ForwardWriterPtr createForward(XmlBuffer* xmlBuffer);
	static OrnamentsWriterPtr createOrnaments(XmlBuffer* xmlBuffer);
	static TechnicalWriterPtr createTechnical(XmlBuffer* xmlBuffer);
	static ArticulationWriterPtr createArticulation(XmlBuffer* xmlBuffer);
	static DynamicsWriterPtr createDynamics(XmlBuffer* xmlBuffer);
	static RootWriterPtr createRoot(XmlBuffer* xmlBuffer);
	static BassWriterPtr createBass(XmlBuffer* xmlBuffer);
	static DegreeWriterPtr createDegree(XmlBuffer* xmlBuffer);
	static DirectionTypeWriterPtr createDirectionType(XmlBuffer* xmlBuffer);
	static EndingWriterPtr createEnding(XmlBuffer* xmlBuffer);
	static RepeatWriterPtr createRepeat(XmlBuffer* xmlBuffer);
	static TiedWriterPtr createTied(XmlBuffer* xmlBuffer);
	static MetronomeWriterPtr createMetronome(XmlBuffer* xmlBuffer);
};

}

#endif
