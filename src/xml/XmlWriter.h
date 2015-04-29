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

#ifndef XML_WRITER_H
#define XML_WRITER_H

#include <QMap>
#include <QList>
#include <QString>
#include <QtXml>
#include <XmlBuffer.h>
#include "XmlWriterPtr.h"

namespace XML {

class Writer {
public:
	explicit Writer(XmlBuffer* xmlBuffer);

	explicit Writer(const QString& name, const QString& value = QString(),
			const QString& attr_name = QString(), const QString& attr_value = QString(),
			const QString& attr_name2 = QString(), const QString& attr_value2 = QString(),
			const QString& attr_name3 =	QString(), const QString& attr_value3 = QString());
	virtual ~Writer();

private:
	Writer();

public:
	struct POS {
		int stave_;
		int staff_;
		int bar_;

		POS() :	stave_(0), staff_(0), bar_(0) {}
		POS(int stave, int staff, int bar) : stave_(stave), staff_(staff), bar_(bar) {}
	};

	void process(QDomDocument& doc, QDomNode& parent);
	void setPos(const POS& pos);

	void addChild(const WriterPtr& child);

public:
	static QString strType;
	static QString strStart;
	static QString strStop;
	static QString strNumber;
	static QString strPlacement;

private:
	virtual void collect(); // get all element's value, attribute's name & value
	virtual void createChildren();
	virtual void onBeforeProcess();

protected:
	XmlBuffer* buffer() const;
	POS pos() const;

	void setName(const QString& name);
	void setIsRoot(bool isRoot);
	void setCreatePrefixComment(bool createPrefixComment);

protected:
	QString name_;
	QString value_;
	QMap<QString, QString> attrs_;

	QList<WriterPtr> children_;
	bool createPrefixComment_;
	bool createSuffixComment_;

private:
	POS pos_;
	bool isRoot_;
	XmlBuffer* buffer_;
};

}

#endif
