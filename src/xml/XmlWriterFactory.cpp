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

#include "XmlWriterFactory.h"

#include "XmlWriter.h"
#include "XmlWriterStructure.h"
#include "XmlWriterMeasure.h"

using namespace XML;

XmlWriterFactory::XmlWriterFactory() {
}

XmlWriterFactory::~XmlWriterFactory() {
}

WriterPtr XmlWriterFactory::createWriter(const QString& name,
		const QString& value, const QString& attr_name,
		const QString& attr_value, const QString& attr_name2,
		const QString& attr_value2, const QString& attr_name3,
		const QString& attr_value3) {
	return WriterPtr(new Writer(name, value, attr_name, attr_value, attr_name2, attr_value2, attr_name3, attr_value3));
}

ScorePartwiseWriterPtr XmlWriterFactory::createScorePartwise(XmlBuffer* xmlBuffer) {
	return ScorePartwiseWriterPtr(new ScorePartwiseWriter(xmlBuffer));
}

MovementTitleWriterPtr XmlWriterFactory::createMovementTitle(XmlBuffer* xmlBuffer) {
	return MovementTitleWriterPtr(new MovementTitleWriter(xmlBuffer));
}

IndentificationWriterPtr XmlWriterFactory::createIndentification(XmlBuffer* xmlBuffer) {
	return IndentificationWriterPtr(new IndentificationWriter(xmlBuffer));
}

DefaultsWriterPtr XmlWriterFactory::createDefaults(XmlBuffer* xmlBuffer) {
	return DefaultsWriterPtr(new DefaultsWriter(xmlBuffer));
}

EncodingWriterPtr XmlWriterFactory::createEncoding(XmlBuffer* xmlBuffer) {
	return EncodingWriterPtr(new EncodingWriter(xmlBuffer));
}

ScalingWriterPtr XmlWriterFactory::createScaling(XmlBuffer* xmlBuffer) {
	return ScalingWriterPtr(new ScalingWriter(xmlBuffer));
}

PageLayoutWriterPtr XmlWriterFactory::createPageLayout(XmlBuffer* xmlBuffer) {
	return PageLayoutWriterPtr(new PageLayoutWriter(xmlBuffer));
}

PageMarginsWriterPtr XmlWriterFactory::createPageMargins(XmlBuffer* xmlBuffer) {
	return PageMarginsWriterPtr(new PageMarginsWriter(xmlBuffer));
}

PartlistWriterPtr XmlWriterFactory::createPartlist(XmlBuffer* xmlBuffer) {
	return PartlistWriterPtr(new PartlistWriter(xmlBuffer));
}

ScorePartWriterPtr XmlWriterFactory::createScorePart(XmlBuffer* xmlBuffer) {
	return ScorePartWriterPtr(new ScorePartWriter(xmlBuffer));
}

ScoreInstrumentWriterPtr XmlWriterFactory::createScoreInstrument(XmlBuffer* xmlBuffer) {
	return ScoreInstrumentWriterPtr(new ScoreInstrumentWriter(xmlBuffer));
}

MidiInstrumentWriterPtr XmlWriterFactory::createMidiInstrument(XmlBuffer* xmlBuffer) {
	return MidiInstrumentWriterPtr(new MidiInstrumentWriter(xmlBuffer));
}

PartWriterPtr XmlWriterFactory::createPart(XmlBuffer* xmlBuffer) {
	return PartWriterPtr(new PartWriter(xmlBuffer));
}

MeasureWriterPtr XmlWriterFactory::createMeasure(XmlBuffer* xmlBuffer) {
	return MeasureWriterPtr(new MeasureWriter(xmlBuffer));
}

AttributeWriterPtr XmlWriterFactory::createAttribute(XmlBuffer* xmlBuffer) {
	return AttributeWriterPtr(new AttributeWriter(xmlBuffer));
}

NoteWriterPtr XmlWriterFactory::createNote(XmlBuffer* xmlBuffer) {
	return NoteWriterPtr(new NoteWriter(xmlBuffer));
}

BarRestWriterPtr XmlWriterFactory::createBarRest(XmlBuffer* xmlBuffer) {
	return BarRestWriterPtr(new BarRestWriter(xmlBuffer));
}

BarBackupWriterPtr XmlWriterFactory::createBarBackup(XmlBuffer* xmlBuffer) {
	return BarBackupWriterPtr(new BarBackupWriter(xmlBuffer));
}

HarmonyWriterPtr XmlWriterFactory::createHarmony(XmlBuffer* xmlBuffer) {
	return HarmonyWriterPtr(new HarmonyWriter(xmlBuffer));
}

BarlineWriterPtr XmlWriterFactory::createBarline(XmlBuffer* xmlBuffer) {
	return BarlineWriterPtr(new BarlineWriter(xmlBuffer));
}

DirectionWriterPtr XmlWriterFactory::createDirection(XmlBuffer* xmlBuffer) {
	return DirectionWriterPtr(new DirectionWriter(xmlBuffer));
}

SoundWriterPtr XmlWriterFactory::createSound(XmlBuffer* xmlBuffer) {
	return SoundWriterPtr(new SoundWriter(xmlBuffer));
}

ClefWriterPtr XmlWriterFactory::createClef(XmlBuffer* xmlBuffer) {
	return ClefWriterPtr(new ClefWriter(xmlBuffer));
}

KeyWriterPtr XmlWriterFactory::createKey(XmlBuffer* xmlBuffer) {
	return KeyWriterPtr(new KeyWriter(xmlBuffer));
}

TimeWriterPtr XmlWriterFactory::createTime(XmlBuffer* xmlBuffer) {
	return TimeWriterPtr(new TimeWriter(xmlBuffer));
}

MeasureStyleWriterPtr XmlWriterFactory::createMeasureStyle(XmlBuffer* xmlBuffer) {
	return MeasureStyleWriterPtr(new MeasureStyleWriter(xmlBuffer));
}

RestWriterPtr XmlWriterFactory::createRest(XmlBuffer* xmlBuffer) {
	return RestWriterPtr(new RestWriter(xmlBuffer));
}

PitchWriterPtr XmlWriterFactory::createPitch(XmlBuffer* xmlBuffer) {
	return PitchWriterPtr(new PitchWriter(xmlBuffer));
}

TieWriterPtr XmlWriterFactory::createTie(XmlBuffer* xmlBuffer) {
	return TieWriterPtr(new TieWriter(xmlBuffer));
}

TimeModificationWriterPtr XmlWriterFactory::createTimeModification(XmlBuffer* xmlBuffer) {
	return TimeModificationWriterPtr(new TimeModificationWriter(xmlBuffer));
}

NotationsWriterPtr XmlWriterFactory::createNotations(XmlBuffer* xmlBuffer) {
	return NotationsWriterPtr(new NotationsWriter(xmlBuffer));
}

LyricWriterPtr XmlWriterFactory::createLyric(XmlBuffer* xmlBuffer) {
	return LyricWriterPtr(new LyricWriter(xmlBuffer));
}

ForwardWriterPtr XmlWriterFactory::createForward(XmlBuffer* xmlBuffer) {
	return ForwardWriterPtr(new ForwardWriter(xmlBuffer));
}

OrnamentsWriterPtr XmlWriterFactory::createOrnaments(XmlBuffer* xmlBuffer) {
	return OrnamentsWriterPtr(new OrnamentsWriter(xmlBuffer));
}

TechnicalWriterPtr XmlWriterFactory::createTechnical(XmlBuffer* xmlBuffer) {
	return TechnicalWriterPtr(new TechnicalWriter(xmlBuffer));
}

ArticulationWriterPtr XmlWriterFactory::createArticulation(XmlBuffer* xmlBuffer) {
	return ArticulationWriterPtr(new ArticulationWriter(xmlBuffer));
}

DynamicsWriterPtr XmlWriterFactory::createDynamics(XmlBuffer* xmlBuffer) {
	return DynamicsWriterPtr(new DynamicsWriter(xmlBuffer));
}

RootWriterPtr XmlWriterFactory::createRoot(XmlBuffer* xmlBuffer) {
	return RootWriterPtr(new RootWriter(xmlBuffer));
}

BassWriterPtr XmlWriterFactory::createBass(XmlBuffer* xmlBuffer) {
	return BassWriterPtr(new BassWriter(xmlBuffer));
}

DegreeWriterPtr XmlWriterFactory::createDegree(XmlBuffer* xmlBuffer) {
	return DegreeWriterPtr(new DegreeWriter(xmlBuffer));
}

DirectionTypeWriterPtr XmlWriterFactory::createDirectionType(XmlBuffer* xmlBuffer) {
	return DirectionTypeWriterPtr(new DirectionTypeWriter(xmlBuffer));
}

EndingWriterPtr XmlWriterFactory::createEnding(XmlBuffer* xmlBuffer) {
	return EndingWriterPtr(new EndingWriter(xmlBuffer));
}

RepeatWriterPtr XmlWriterFactory::createRepeat(XmlBuffer* xmlBuffer) {
	return RepeatWriterPtr(new RepeatWriter(xmlBuffer));
}

TiedWriterPtr XmlWriterFactory::createTied(XmlBuffer* xmlBuffer) {
	return TiedWriterPtr(new TiedWriter(xmlBuffer));
}

MetronomeWriterPtr XmlWriterFactory::createMetronome(XmlBuffer* xmlBuffer) {
	return MetronomeWriterPtr(new MetronomeWriter(xmlBuffer));
}
