//=============================================================================
//  ove2midi
//  Overture Document to Midi file convertor
//
//  Copyright (C) 2010-2010 Rui Fan <vanferry@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#ifndef OVE_CONVERTOR_H
#define OVE_CONVERTOR_H

#include <QString>
#include "ove.h"

class OveConvertor : public OVE::IOveNotify{
public:
	OveConvertor();
	~OveConvertor();

public:
	bool convert(const QString& from, const QString& to);

	virtual void loadInfo(const QString& info);
	virtual void loadError();
	virtual void loadPosition(int currentMeasure, int totalMeasure, int currentTrack, int totalTrack);
};

#endif
