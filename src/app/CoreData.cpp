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

#include "CoreData.h"

CoreData* CoreData::coredata_ = NULL;

CoreData::CoreData() {
	oveSong_ = 0;
}

CoreData::~CoreData() {
}

OVE::OveSong* CoreData::getOveSong(void) const {
	return oveSong_;
}

void CoreData::init(void) {
	oveSong_ = new OVE::OveSong();
    oveSong_->setTextCodecName("GBK");
}

void CoreData::exit(void) {
	if (oveSong_ != 0) {
		delete oveSong_;
	}
}

CoreData* CoreData::get(void) {
	return coredata_;
}

void CoreData::createInstance(void) {
	coredata_ = new CoreData;
}

void CoreData::destroyInstance(void) {
	delete coredata_;
}
