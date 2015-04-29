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

#ifndef XML_WRITER_PTR_H
#define XML_WRITER_PTR_H

#include <QSharedPointer>

namespace XML {

class Writer;
typedef QSharedPointer<Writer> WriterPtr;

class ScorePartwiseWriter;
class MovementTitleWriter;
class IndentificationWriter;
class DefaultsWriter;
class EncodingWriter;
class ScalingWriter;
class PageLayoutWriter;
class PageMarginsWriter;
class PartlistWriter;
class ScorePartWriter;
class ScoreInstrumentWriter;
class MidiInstrumentWriter;
class PartWriter;
typedef QSharedPointer<ScorePartwiseWriter> ScorePartwiseWriterPtr;
typedef QSharedPointer<MovementTitleWriter> MovementTitleWriterPtr;
typedef QSharedPointer<IndentificationWriter> IndentificationWriterPtr;
typedef QSharedPointer<DefaultsWriter> DefaultsWriterPtr;
typedef QSharedPointer<EncodingWriter> EncodingWriterPtr;
typedef QSharedPointer<ScalingWriter> ScalingWriterPtr;
typedef QSharedPointer<PageLayoutWriter> PageLayoutWriterPtr;
typedef QSharedPointer<PageMarginsWriter> PageMarginsWriterPtr;
typedef QSharedPointer<PartlistWriter> PartlistWriterPtr;
typedef QSharedPointer<ScorePartWriter> ScorePartWriterPtr;
typedef QSharedPointer<ScoreInstrumentWriter> ScoreInstrumentWriterPtr;
typedef QSharedPointer<MidiInstrumentWriter> MidiInstrumentWriterPtr;
typedef QSharedPointer<PartWriter> PartWriterPtr;

class MeasureWriter;
typedef QSharedPointer<MeasureWriter> MeasureWriterPtr;

class AttributeWriter;
class NoteWriter;
class BarRestWriter;
class BarBackupWriter;
class HarmonyWriter;
class BarlineWriter;
class DirectionWriter;
class SoundWriter;
typedef QSharedPointer<AttributeWriter> AttributeWriterPtr;
typedef QSharedPointer<NoteWriter> NoteWriterPtr;
typedef QSharedPointer<BarRestWriter> BarRestWriterPtr;
typedef QSharedPointer<BarBackupWriter> BarBackupWriterPtr;
typedef QSharedPointer<HarmonyWriter> HarmonyWriterPtr;
typedef QSharedPointer<BarlineWriter> BarlineWriterPtr;
typedef QSharedPointer<DirectionWriter> DirectionWriterPtr;
typedef QSharedPointer<SoundWriter> SoundWriterPtr;

class ClefWriter;
class KeyWriter;
class TimeWriter;
class MeasureStyleWriter;
typedef QSharedPointer<ClefWriter> ClefWriterPtr;
typedef QSharedPointer<KeyWriter> KeyWriterPtr;
typedef QSharedPointer<TimeWriter> TimeWriterPtr;
typedef QSharedPointer<MeasureStyleWriter> MeasureStyleWriterPtr;

class RestWriter;
class PitchWriter;
class TieWriter;
class TimeModificationWriter;
class NotationsWriter;
class LyricWriter;
class ForwardWriter;
class OrnamentsWriter;
class TechnicalWriter;
class ArticulationWriter;
class DynamicsWriter;
typedef QSharedPointer<RestWriter> RestWriterPtr;
typedef QSharedPointer<PitchWriter> PitchWriterPtr;
typedef QSharedPointer<TieWriter> TieWriterPtr;
typedef QSharedPointer<TimeModificationWriter> TimeModificationWriterPtr;
typedef QSharedPointer<NotationsWriter> NotationsWriterPtr;
typedef QSharedPointer<LyricWriter> LyricWriterPtr;
typedef QSharedPointer<ForwardWriter> ForwardWriterPtr;
typedef QSharedPointer<OrnamentsWriter> OrnamentsWriterPtr;
typedef QSharedPointer<TechnicalWriter> TechnicalWriterPtr;
typedef QSharedPointer<ArticulationWriter> ArticulationWriterPtr;
typedef QSharedPointer<DynamicsWriter> DynamicsWriterPtr;

class RootWriter;
class BassWriter;
class DegreeWriter;
typedef QSharedPointer<RootWriter> RootWriterPtr;
typedef QSharedPointer<BassWriter> BassWriterPtr;
typedef QSharedPointer<DegreeWriter> DegreeWriterPtr;

class DirectionTypeWriter;
typedef QSharedPointer<DirectionTypeWriter> DirectionTypeWriterPtr;

class EndingWriter;
class RepeatWriter;
typedef QSharedPointer<EndingWriter> EndingWriterPtr;
typedef QSharedPointer<RepeatWriter> RepeatWriterPtr;

class TiedWriter;
typedef QSharedPointer<TiedWriter> TiedWriterPtr;

class MetronomeWriter;
typedef QSharedPointer<MetronomeWriter> MetronomeWriterPtr;

}

#endif
