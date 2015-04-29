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

#ifndef CONVERT_WINDOW_H
#define CONVERT_WINDOW_H

#include <QMainWindow>
#include "ui_ConvertWindow.h"

class QAction;
class QMenu;
class QProgressBar;
class QTextEdit;
class QToolBar;
class FileBrowser;
class LoadOveThread;
class ConvertXmlThread;

class ConvertWindow: public QMainWindow {
	Q_OBJECT

public:
	ConvertWindow();

private slots:
	void onAbout();
	void onAboutQt();
	void selectOveFileSlot(const QString& path);
	void processOveFileSlot(const QString& path);

	void onConvert();
	void onConvertAll();

	// parse status
	void startLoadOveSlot();
	void finishLoadOveSlot();
	void startConvertSlot();
	void finishConvertSlot();
	void loadErrorSlot();
	void convertInfoSlot(const QStringList& infos);
	void convertProgressSlot(int step, int totalStep);

private:
	virtual void closeEvent(QCloseEvent *event);

private:
	void createWidgets();
	void createToolbarAndMenu();
	void createThreads();

	void doSomeTests();

private:
	Ui::ConvertWindow ui_;

	QMenu* fileMenu_;
	QAction* exitAction_;

	QMenu *helpMenu;
	QAction *aboutAction_;
	QAction *aboutQtAction_;

	QAction* convertAction_;
	QAction* convertAllAction_;

	// convert mode
	FileBrowser* oveBrowser_;
	QProgressBar* progressBar_;

	LoadOveThread* loadOveThread_;
	ConvertXmlThread* convertXmlThread_;

private:
	bool exitProcess_;
	QString currentFile_;
	bool loadError_;
	bool convertAll_;
};

#endif
