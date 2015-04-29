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

#include "XmlWriter.h"

namespace XML {

QString Writer::strType = "type";
QString Writer::strStart = "start";
QString Writer::strStop = "stop";
QString Writer::strNumber = "number";
QString Writer::strPlacement = "placement";

Writer::Writer(XmlBuffer* xmlBuffer) :
	createPrefixComment_(false), createSuffixComment_(false), isRoot_(false), buffer_(xmlBuffer) {
}

Writer::Writer() :
	createPrefixComment_(false), createSuffixComment_(false), isRoot_(false), buffer_(NULL) {
}

Writer::Writer(const QString& name, const QString& value,
		const QString& attr_name, const QString& attr_value,
		const QString& attr_name2, const QString& attr_value2,
		const QString& attr_name3, const QString& attr_value3) :
	name_(name), value_(value), createPrefixComment_(false),
	createSuffixComment_(false), isRoot_(false), buffer_(NULL) {
	if (!attr_name.isEmpty()) {
		attrs_[attr_name] = attr_value;
	}

	if (!attr_name2.isEmpty() && attr_name2 != attr_name) {
		attrs_[attr_name2] = attr_value2;
	}

	if (!attr_name3.isEmpty() && attr_name3 != attr_name) {
		attrs_[attr_name3] = attr_value3;
	}
}

Writer::~Writer() {
	buffer_ = NULL;
}

void Writer::collect() {
}

void Writer::createChildren() {
}

void Writer::onBeforeProcess() {
}

void Writer::process(QDomDocument& doc, QDomNode& parent) {
	int i;
	QMap<QString, QString>::iterator it;
	QDomElement elem;
	QString spliter = "=========================================================";

	//////////////////////////////////////////////////////////////////////////

	collect();
	createChildren();

	//////////////////////////////////////////////////////////////////////////

	Q_ASSERT(!name_.isEmpty());

	if (isRoot_) {
		elem = doc.documentElement();
	} else {
		elem = doc.createElement(name_);

		if (createPrefixComment_) {
            QDomComment comment = doc.createComment(spliter);
			parent.appendChild(comment);
		}

		parent.appendChild(elem);
	}

	if (!value_.isEmpty()) {
		QDomText text = doc.createTextNode(value_);
		elem.appendChild(text);
	}

	for (it = attrs_.begin(); it != attrs_.end(); ++it) {
		elem.setAttribute(it.key(), it.value());
	}

	//////////////////////////////////////////////////////////////////////////

	for (i = 0; i < children_.size(); ++i) {
		children_[i]->onBeforeProcess();
		children_[i]->process(doc, elem);
	}

	if (createSuffixComment_) {
        QDomComment comment = doc.createComment(spliter);
		parent.appendChild(comment);
	}
}

void Writer::setPos(const POS& pos) {
	pos_.stave_ = pos.stave_;
	pos_.staff_ = pos.staff_;
	pos_.bar_ = pos.bar_;
}

void Writer::addChild(const WriterPtr& child) {
	children_.push_back(child);
}

void Writer::setName(const QString& name) {
	name_ = name;
}

void Writer::setIsRoot(bool isRoot) {
	isRoot_ = isRoot;
}

XmlBuffer* Writer::buffer() const {
	return buffer_;
}

Writer::POS Writer::pos() const {
	return pos_;
}

void Writer::setCreatePrefixComment(bool createPrefixComment) {
	createPrefixComment_ = createPrefixComment;
}

}
