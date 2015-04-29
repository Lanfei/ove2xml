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

#ifndef XML_WRITER_STRUCTURE_H
#define XML_WRITER_STRUCTURE_H

#include "XmlWriter.h"
#include "ove.h"

using namespace OVE;

namespace XML {

class XmlBuffer;

class ScorePartwiseWriter: public Writer {
public:
	ScorePartwiseWriter(XmlBuffer* xmlBuffer);
	virtual ~ScorePartwiseWriter() {}

public:
	virtual void createChildren();
};

class MovementTitleWriter: public Writer {
public:
	MovementTitleWriter(XmlBuffer* xmlBuffer);
	virtual ~MovementTitleWriter() {}

public:
	virtual void collect();
};

class IndentificationWriter: public Writer {
public:
	IndentificationWriter(XmlBuffer* xmlBuffer);
	virtual ~IndentificationWriter() {}

public:
	virtual void createChildren();
};

class DefaultsWriter: public Writer {
public:
	DefaultsWriter(XmlBuffer* xmlBuffer);
	virtual ~DefaultsWriter() {}

public:
	virtual void createChildren();
};

class EncodingWriter: public Writer {
public:
	EncodingWriter(XmlBuffer* xmlBuffer);
	virtual ~EncodingWriter() {}

public:
	virtual void createChildren();
};

class ScalingWriter: public Writer {
public:
	ScalingWriter(XmlBuffer* xmlBuffer);
	virtual ~ScalingWriter() {}

public:
	virtual void createChildren();
};

class PageLayoutWriter: public Writer {
public:
	PageLayoutWriter(XmlBuffer* xmlBuffer);
	virtual ~PageLayoutWriter() {}

public:
	virtual void createChildren();
};

class PageMarginsWriter: public Writer {
public:
	PageMarginsWriter(XmlBuffer* xmlBuffer);
	virtual ~PageMarginsWriter() {}

public:
	virtual void createChildren();

	void setPage(Page* page);

private:
	Page* page_;
};

class PartlistWriter: public Writer {
public:
	PartlistWriter(XmlBuffer* xmlBuffer);
	virtual ~PartlistWriter() {}

public:
	virtual void createChildren();
};

class ScorePartWriter: public Writer {
public:
	ScorePartWriter(XmlBuffer* xmlBuffer);
	virtual ~ScorePartWriter() {}

public:
	virtual void collect();
	virtual void createChildren();
};

class ScoreInstrumentWriter: public Writer {
public:
	ScoreInstrumentWriter(XmlBuffer* xmlBuffer);
	virtual ~ScoreInstrumentWriter() {}

public:
	virtual void collect();
	virtual void createChildren();
};

class MidiInstrumentWriter: public Writer {
public:
	MidiInstrumentWriter(XmlBuffer* xmlBuffer);
	virtual ~MidiInstrumentWriter() {}

public:
	virtual void collect();
	virtual void createChildren();
};

class PartWriter: public Writer {
public:
	PartWriter(XmlBuffer* xmlBuffer);
	virtual ~PartWriter() {}

public:
	virtual void collect();
	virtual void createChildren();
	virtual void onBeforeProcess();

private:
	int getPartDivision();
	int calculateDivision(const QList<NoteContainer*>& containers);
};

}

#endif
