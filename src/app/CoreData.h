//=============================================================================
//  Ove2Xml
//  Overture Document To MusicXml Document Convertor
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

#ifndef CORE_DATA_H
#define CORE_DATA_H

#include "ove.h"

class CoreData {
public:
	CoreData();
	virtual ~CoreData();

public:
	void init(void);
	void exit(void);
	static CoreData* get(void);
	static void createInstance(void);
	static void destroyInstance(void);

public:
	OVE::OveSong* getOveSong(void) const;

private:
	static CoreData* coredata_;

private:
	OVE::OveSong* oveSong_;

};

#endif
