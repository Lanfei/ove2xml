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

#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include <QTableView>
#include <QDirModel>
#include <QFileSystemModel>
#include <QWidget>
#include "ui_FileBrowser.h"

class QString;
class QDirModel;
class QFileSystemModel;

class DirTableView: public QTableView {
	Q_OBJECT

public:
	DirTableView(QWidget* parent = 0);

signals:
	void keyPressed(int key);

public:
	virtual void setRootIndex(const QModelIndex& index);

private:
	virtual void keyPressEvent(QKeyEvent* event);
	virtual int sizeHintForColumn(int column) const;
};

class DropArea: public QWidget {
	Q_OBJECT

public:
	DropArea(QWidget* parent = 0);

protected:
	void dragEnterEvent(QDragEnterEvent* event);
	void dragMoveEvent(QDragMoveEvent* event);
	void dragLeaveEvent(QDragLeaveEvent* event);
	void dropEvent(QDropEvent* event);

	virtual void onDropped(const QMimeData* mimeData = 0);
};

class FileBrowser: public DropArea {
	Q_OBJECT

public:
	FileBrowser(QWidget* parent = 0);

public:
	void setRootPath(const QString& path);
	QString getRootPath() const;

	void chooseRootPath();
	bool jumpToNextSong();
	bool jumpToPreviousSong();

signals:
	void selectOveFile(const QString& path);
	void processOveFile(const QString& path);
	//void endOfList() ;

private slots:
	void searchTextChanged(const QString& /*text*/);
	void dirButtonClicked();
	void upButtonClicked();
	void intoButtonClicked();
	void tableViewMouseClick(const QModelIndex& index);
	void tableViewMouseDoubleClick(const QModelIndex& index);
	void tableViewKeyPressed(int key);
	void tableViewRowsInserted(const QModelIndex& parent, int start, int end);
	void chooseIndex(const QModelIndex& index, bool enterFolder, bool process);

private:
	QString getCurrentPath() const;
	void setRootIndex(const QModelIndex& index);
	bool indexFileExists(const QModelIndex& index);

	virtual void onDropped(const QMimeData* mimeData);

	void doSetNameFilters(const QString& searchText);

private:
	Ui::FileBrowser ui_;
	QFileSystemModel* dirModel_;
	DirTableView* dirTableView_;
	QModelIndex currentIndex_;

	QStringList exts_;

	bool needSelectItem_;
};

#endif
