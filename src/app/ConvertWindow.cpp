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

#include <QtGui>
#include <QMessageBox>
#include <QProgressBar>
#include "ConvertWindow.h"

#include "CoreData.h"
#include "ConvertXmlThread.h"
#include "LoadOveThread.h"
#include "FileBrowser.h"
#include "About.h"

///////////////////////////////////////////////////////////////////////////////
ConvertWindow::ConvertWindow() {
	ui_.setupUi(this);

	createWidgets();
	createToolbarAndMenu();

	createThreads();

	exitProcess_ = false;
	loadError_ = false;
	convertAll_ = false;
}

void ConvertWindow::createWidgets() {
	// progress
	progressBar_ = new QProgressBar;

	// file browser
	//oveBrowser_ = new FileBrowser(this, Qt::Widget) ;
	oveBrowser_ = new FileBrowser(this);

    QString dirStr = QString("/home/fanrui/Music/Overture");
	oveBrowser_->setRootPath(dirStr);

    connect(oveBrowser_, SIGNAL(selectOveFile(const QString&)), this, SLOT(selectOveFileSlot(const QString&)));
    connect(oveBrowser_, SIGNAL(processOveFile(const QString&)), this, SLOT(processOveFileSlot(const QString&)));

	QVBoxLayout* vbox = new QVBoxLayout;
	vbox->addWidget(oveBrowser_);
	vbox->setMargin(0);

	ui_.widgetFileList->setLayout(vbox);
}

void ConvertWindow::createToolbarAndMenu() {
	// action
	exitAction_ = new QAction(tr("E&xit"), this);
	exitAction_->setShortcut(tr("Ctrl+Q"));
	exitAction_->setStatusTip(tr("Exit the application"));
	connect(exitAction_, SIGNAL(triggered()), this, SLOT(close()));

	aboutAction_ = new QAction(tr("&About"), this);
	aboutAction_->setStatusTip(tr("Show the application's About box"));
	connect(aboutAction_, SIGNAL(triggered()), this, SLOT(onAbout()));

	aboutQtAction_ = new QAction(tr("About Qt"), this);
	aboutQtAction_->setStatusTip(tr("Show Qt's About box"));
	connect(aboutQtAction_, SIGNAL(triggered()), this, SLOT(onAboutQt()));

        convertAction_ = new QAction(QIcon(":/save.xml.png"), tr("Convert"), this);
	convertAction_->setShortcut(tr("F5"));
	convertAction_->setStatusTip(tr("Convert to MusicXml file"));
	connect(convertAction_, SIGNAL(triggered()), this, SLOT(onConvert()));

        convertAllAction_ = new QAction(QIcon(":/save.xmls.png"), tr("Convert All"), this);
	convertAllAction_->setShortcut(tr("F6"));
	convertAllAction_->setStatusTip(tr("Convert all to MusicXml file"));
	connect(convertAllAction_, SIGNAL(triggered()), this, SLOT(onConvertAll()));

	// menu
	fileMenu_ = menuBar()->addMenu(tr("&File"));
	fileMenu_->addAction(convertAction_);
	fileMenu_->addAction(convertAllAction_);
	fileMenu_->addAction(exitAction_);

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAction_);

	ui_.toolBar->addAction(convertAction_);
	ui_.toolBar->addAction(convertAllAction_);
}

void ConvertWindow::createThreads() {
	loadOveThread_ = new LoadOveThread;
	connect(loadOveThread_, SIGNAL(started()), this, SLOT(startLoadOveSlot()));
	connect(loadOveThread_, SIGNAL(finished()), this, SLOT(finishLoadOveSlot()));
	connect(loadOveThread_, SIGNAL(loadInfoSignal(const QStringList&)), this, SLOT(convertInfoSlot(const QStringList&)));
	connect(loadOveThread_, SIGNAL(loadErrorSignal()), this, SLOT(loadErrorSlot()));
	connect(loadOveThread_, SIGNAL(loadProgressSignal(int, int)), this, SLOT(convertProgressSlot(int, int)));

	convertXmlThread_ = new ConvertXmlThread;
	connect(convertXmlThread_, SIGNAL(started()), this, SLOT(startConvertSlot()));
	connect(convertXmlThread_, SIGNAL(finished()), this, SLOT(finishConvertSlot()));
	connect(convertXmlThread_, SIGNAL(convertInfoSignal(const QStringList&)), this, SLOT(convertInfoSlot(const QStringList&)));
	connect(convertXmlThread_, SIGNAL(convertProgressSignal(int, int)), this, SLOT(convertProgressSlot(int, int)));
}

void ConvertWindow::closeEvent(QCloseEvent *event) {
	exitProcess_ = true;

	disconnect(loadOveThread_, SIGNAL(started()), this, SLOT(startLoadOveSlot()));
	disconnect(loadOveThread_, SIGNAL(finished()), this, SLOT(finishLoadOveSlot()));
	disconnect(loadOveThread_, SIGNAL(loadInfoSignal(const QStringList&)), this, SLOT(convertInfoSlot(const QStringList&)));
	disconnect(loadOveThread_, SIGNAL(loadErrorSignal()), this, SLOT(loadErrorSlot()));
	disconnect(loadOveThread_, SIGNAL(loadProgressSignal(int, int)), this, SLOT(convertProgressSlot(int, int)));

	disconnect(convertXmlThread_, SIGNAL(started()), this, SLOT(startConvertSlot()));
	disconnect(convertXmlThread_, SIGNAL(finished()), this, SLOT(finishConvertSlot()));
	disconnect(convertXmlThread_, SIGNAL(convertInfoSignal(const QStringList&)), this, SLOT(convertInfoSlot(const QStringList&)));
	disconnect(convertXmlThread_, SIGNAL(convertProgressSignal(int, int)), this, SLOT(convertProgressSlot(int, int)));

	disconnect(oveBrowser_, SIGNAL(selectOveFile(const QString&)), this, SLOT(selectOveFileSlot(const QString&)));
	disconnect(oveBrowser_, SIGNAL(processOveFile(const QString&)), this, SLOT(processOveFileSlot(const QString&)));

	delete loadOveThread_;
	delete convertXmlThread_;
	delete oveBrowser_;

	event->accept();
}

void ConvertWindow::onAbout() {
	AboutDialog dlg(this);
	dlg.exec();
}

void ConvertWindow::onAboutQt() {
	QApplication::aboutQt();
}

void ConvertWindow::selectOveFileSlot(const QString& path) {
	if (!convertXmlThread_->isRunning()) {
		currentFile_ = path;
	}
}

void ConvertWindow::processOveFileSlot(const QString& path) {
	if (!convertXmlThread_->isRunning()) {
		currentFile_ = path;
		onConvert();
	}
}

void ConvertWindow::startLoadOveSlot() {
	loadError_ = false;

	statusBar()->addWidget(progressBar_);
	progressBar_->show();
	progressBar_->setRange(0, 100);
	progressBar_->setValue(0);

	convertAction_->setEnabled(false);
	convertAllAction_->setEnabled(false);
}

void ConvertWindow::finishLoadOveSlot() {
	if (!loadError_) {
		convertXmlThread_->setOveData(CoreData::get()->getOveSong());
		convertXmlThread_->setOveFile(currentFile_);
		convertXmlThread_->start();
	} else {
		finishConvertSlot();
	}
}

void ConvertWindow::startConvertSlot() {
}

void ConvertWindow::finishConvertSlot() {
	statusBar()->removeWidget(progressBar_);
	progressBar_->hide();

	convertAction_->setEnabled(true);
	convertAllAction_->setEnabled(true);

	if (loadError_) {
		convertAll_ = false;
	}

	if (convertAll_) {
		if (!oveBrowser_->jumpToNextSong()) {
			convertAll_ = false;
		}
	} else {
		doSomeTests();
	}

	update();
}

void ConvertWindow::loadErrorSlot() {
	loadError_ = true;
}

void ConvertWindow::convertInfoSlot(const QStringList& infos) {
	QStringList::const_iterator constIterator;
	for (constIterator = infos.constBegin(); constIterator != infos.constEnd(); ++constIterator) {
		ui_.textEdit->append((*constIterator));
	}
}

void ConvertWindow::convertProgressSlot(int step, int totalStep) {
	if (progressBar_->isVisible()) {
		progressBar_->setValue(step);
	}
}

void ConvertWindow::onConvert() {
	QFileInfo info(currentFile_);

	if (info.exists() && !convertXmlThread_->isRunning()) {
		OVE::OveSong* ove = CoreData::get()->getOveSong();
		loadOveThread_->setOveSong(ove);
		loadOveThread_->setOveFile(currentFile_);
		loadOveThread_->start();
	}
}

void ConvertWindow::onConvertAll() {
	QMessageBox::StandardButton ret = QMessageBox::question(
							this,
							tr("Are You Sure?"),
							tr("This will convert all ove files under current folder to MusicXml fils, \n are you sure?"),
							QMessageBox::Ok | QMessageBox::Cancel,
							QMessageBox::Cancel);

	if (ret == QMessageBox::Ok) {
		convertAll_ = true;

		onConvert();
	}
}

QString GetXmlFilePath(const QString& ovePath) {
	QFileInfo info(ovePath);
	QString xmlFile = info.absolutePath() + "/" + info.completeBaseName() + ".xml";

	return xmlFile;
}

void ConvertWindow::doSomeTests() {
	/*	QString oveProgram = "C:\\Program Files\\GenieSoft\\Overture 4.0\\Overture.exe" ;
	 QString finaleProgram = "C:\\Program Files\\Finale 2008\\finale.exe" ;
	 //	QProcess myProcess ;
	 QStringList arguments ;

	 //	myProcess.start(program) ;
	 //	arguments << "-f " << currentFile_ ;
	 //	QProcess::execute(oveProgram, arguments) ;

	 //	arguments.clear() ;
	 arguments << "-f " << GetXmlFilePath(currentFile_) ;
	 //	myProcess.start(finaleProgram, arguments) ;*/
}
