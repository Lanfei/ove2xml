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

#include "XmlSerialize.h"

#include <QDebug>
#include "XmlWriter.h"
#include "XmlWriterPtr.h"
#include "XmlWriterStructure.h"
#include "XmlWriterFactory.h"

namespace XML {

XmlSerialize::XmlSerialize() {
	notify_ = NULL;
}

XmlSerialize::~XmlSerialize() {
}

void XmlSerialize::setOveSong(OVE::OveSong* ove) {
	ove_ = ove;
}

void XmlSerialize::setDate(const QString& date) {
	date_ = date;
}

void XmlSerialize::setNotify(IXmlNotify* notify) {
	notify_ = notify;
}

bool XmlSerialize::load(const QString& /*path*/) {
	return false;
}

bool XmlSerialize::save(const QString& path) {
	if (path.isEmpty()) {
		return false;
	}

	QDomImplementation impl;
	QDomDocument doc;
	QDomDocumentType docType;
	QDomNode root;

	docType = impl.createDocumentType(
				"score-partwise",
				"-//Recordare//DTD MusicXML 1.1 Partwise//EN",
				"http://www.musicxml.org/dtds/partwise.dtd");

    doc = impl.createDocument(QString::null, "score-partwise", docType);

    QDomNode xmlNode = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"");
	doc.insertBefore(xmlNode, doc.firstChild());

	root = doc.documentElement();

	XmlBuffer xmlBuffer;
	xmlBuffer.setOve(ove_);
	xmlBuffer.setEncodeDate(date_);
	xmlBuffer.setNotify(notify_);
	xmlBuffer.organize();

	ScorePartwiseWriterPtr writer = XmlWriterFactory::createScorePartwise(&xmlBuffer);
	writer->process(doc, root);

	QFile file(path);
	if(!file.open(QIODevice::WriteOnly))
  	    return false;

    QTextStream ts(&file);
	ts << doc.toString();

	file.close();

	return true;
}

}
