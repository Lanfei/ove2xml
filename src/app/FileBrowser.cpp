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

#include "FileBrowser.h"
#include <QtGui>
#include <QFileDialog>

const bool ENABLE_SORT = true;

std::string QString2string(const QString& qstr) {
	QByteArray array = qstr.toLocal8Bit();
	std::string str = array.data();

	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DirTableView::DirTableView(QWidget* parent) :
	QTableView(parent) {
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	if (ENABLE_SORT) {
		QHeaderView* header = horizontalHeader();
		connect(header, SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));
	}

	QHeaderView* header = horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
	//header->setResizeMode(QHeaderView::Interactive);

	if (ENABLE_SORT) {
		QHeaderView* header = horizontalHeader();
		header->setSortIndicatorShown(true);
        header->setSectionsClickable(true);
		setSortingEnabled(true);
	}

	//	verticalHeader()->hide() ;
	//	setShowGrid(false) ;
}

void DirTableView::setRootIndex(const QModelIndex& index) {
	QFileSystemModel* oveModel = qobject_cast<QFileSystemModel*> (QTableView::model());
	std::string path = QString2string(oveModel->filePath(index));

	QTableView::setRootIndex(index);
}

void DirTableView::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
	case Qt::Key_Enter:
	case Qt::Key_Return:
	case Qt::Key_Backspace: {
		emit keyPressed(event->key());
		return;

		break;
	}
	default:
		break;
	}

	QTableView::keyPressEvent(event);
}

int DirTableView::sizeHintForColumn(int column) const {
	int width = QTableView::sizeHintForColumn(column);

	if (column == 0) {
		width += 100;
	} else {
		width += 20;
	}

	return width;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DropArea::DropArea(QWidget *parent) :
	QWidget(parent) {
	setAcceptDrops(true);
}

void DropArea::dragEnterEvent(QDragEnterEvent *event) {
	event->acceptProposedAction();
}

void DropArea::dragMoveEvent(QDragMoveEvent *event) {
	event->acceptProposedAction();
}

void DropArea::dropEvent(QDropEvent *event) {
	event->acceptProposedAction();
	onDropped(event->mimeData());
}

void DropArea::dragLeaveEvent(QDragLeaveEvent *event) {
	event->accept();
}

void DropArea::onDropped(const QMimeData* /*mimeData*/) {
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FileBrowser::FileBrowser(QWidget* parent) :
	DropArea(parent) {
	ui_.setupUi(this);

	exts_ << ".ove";
	//exts_ << ".mid";

	dirModel_ = new QFileSystemModel(this);
	doSetNameFilters(QString());
	dirModel_->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
	//dirModel_->setSorting(QDir::DirsFirst|QDir::Name|QDir::LocaleAware) ;
	dirModel_->setNameFilterDisables(false);

	dirTableView_ = new DirTableView(this);
	dirTableView_->setModel(dirModel_);
	dirTableView_->setFocusPolicy(Qt::StrongFocus);
	dirTableView_->setFocus();

	QVBoxLayout* vbox = new QVBoxLayout;
	vbox->addWidget(dirTableView_);
	vbox->setMargin(0);
	ui_.widget->setLayout(vbox);

	connect(ui_.searchEdit, SIGNAL(textChanged(const QString&)), this, SLOT(searchTextChanged(const QString&)));
	connect(ui_.dirToolButton, SIGNAL(clicked()), this, SLOT(dirButtonClicked()));
	connect(ui_.upToolButton, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
	connect(ui_.intoToolButton, SIGNAL(clicked()), this, SLOT(intoButtonClicked()));

	connect(dirTableView_, SIGNAL(clicked(const QModelIndex&)), this,
			SLOT(tableViewMouseClick(const QModelIndex&)));
	connect(dirTableView_, SIGNAL(doubleClicked(const QModelIndex&)), this,
			SLOT(tableViewMouseDoubleClick(const QModelIndex&)));
	connect(dirTableView_, SIGNAL(keyPressed(int)), this, SLOT(tableViewKeyPressed(int)));
	connect(dirModel_, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
			this, SLOT(tableViewRowsInserted(const QModelIndex&, int, int)));

	QFont font = ui_.dirLabel->font();
	//font.setPointSize(10) ;
	dirTableView_->setFont(font);

	needSelectItem_ = false;
}

QString FileBrowser::getRootPath() const {
	return dirModel_->rootPath();
}

void FileBrowser::setRootPath(const QString& path) {
	QFileInfo info(path);
	QString pathStr = info.exists() ? path : QDir::currentPath();

	needSelectItem_ = true;

	QModelIndex oveIndex = dirModel_->setRootPath(pathStr);

	setRootIndex(oveIndex);
}

void FileBrowser::chooseRootPath() {
	dirButtonClicked();
}

void FileBrowser::setRootIndex(const QModelIndex& index) {
	QModelIndex dir = index.sibling(index.row(), 0);

	if (!dirModel_->isDir(dir)) {
		dir = dir.parent();
	}

	if (dir == dirTableView_->rootIndex()) {
		return;
	}

	int selectID = 0;
	QString oldPath = dirModel_->filePath(dirTableView_->rootIndex())/* ui_.dirLabel->text() */;
	QString path = dirModel_->filePath(dir);
	QModelIndex oldRoot = dirModel_->index(ui_.dirLabel->text());
	QModelIndex newRoot = dirModel_->index(dirModel_->filePath(dir));

	dirTableView_->setRootIndex(dir);
	ui_.dirLabel->setText(path);
	ui_.dirLabel->setStatusTip(path);

	if (newRoot == oldRoot.parent()) {
		QString parentFolderText = oldPath.right(oldPath.length() - path.length() - 1);
		std::string a = QString2string(parentFolderText);

		for (int i = 0; i < dirModel_->rowCount(dir); ++i) {
			QModelIndex id = dirModel_->index(i, 0, dir);
			QString str = dirModel_->data(id).toString();
			std::string b = QString2string(str);
			if (dirModel_->isDir(id) && str == parentFolderText) {
				selectID = i;
			}
		}
	}

	if (newRoot.parent() == oldRoot) {
		needSelectItem_ = true;
		dirTableView_->scrollToTop();
	}

	QModelIndex i = dirModel_->index(selectID, 0, dir);

	if (i != QModelIndex()) {
		dirTableView_->selectionModel()->select(i,
				QItemSelectionModel::ClearAndSelect);
		dirTableView_->setCurrentIndex(i);
	}

	//dirTableView_->resizeColumnsToContents() ;
	//dirTableView_->resizeRowsToContents() ;
}

QString FileBrowser::getCurrentPath() const {
	return ui_.dirLabel->text();
}

void FileBrowser::searchTextChanged(const QString& /*text*/) {
	doSetNameFilters(ui_.searchEdit->text().trimmed());
}

void FileBrowser::doSetNameFilters(const QString& searchText) {
	int i;
	QStringList filter;

	for (i = 0; i < exts_.size(); ++i) {
		QString str = "*";

		if (searchText != QString()) {
			str += searchText;
			str += "*";
		}

		str += exts_[i];

		filter << str;
	}

	dirModel_->setNameFilters(filter);
}

void FileBrowser::dirButtonClicked() {
	QString directory = QFileDialog::getExistingDirectory(this, tr("Find Files In..."), getCurrentPath());
	QFileInfo info(directory);

	if (!directory.isEmpty() && info.isDir()) {
		setRootPath(directory);
	}
}

void FileBrowser::intoButtonClicked() {
	QItemSelectionModel* sm = dirTableView_->selectionModel();
	QModelIndexList list = sm->selectedRows();

	for (int i = 0; i < list.size(); ++i) {
		tableViewMouseDoubleClick(list[i]);
		break;
	}
}

void FileBrowser::upButtonClicked() {
	QModelIndex parent = dirTableView_->rootIndex().parent();
	QString path = dirModel_->filePath(dirTableView_->rootIndex());
	std::string str = QString2string(path);

	if (parent != QModelIndex()) {
		setRootIndex(parent);
	}
}

void FileBrowser::chooseIndex(const QModelIndex& index, bool enterFolder, bool process) {
	if (dirModel_->isDir(index)) {
		if (enterFolder) {
			QModelIndex childIndex = index.sibling(index.row(), 0);

			if (dirModel_->isDir(childIndex)) {
				setRootIndex(childIndex);
			}
		}
	} else {
		QString path = dirModel_->filePath(index);
		QFileInfo info(path);

		if (info.exists()) {
			currentIndex_ = index;
			dirTableView_->selectRow(index.row());

			if (process) {
				emit processOveFile(path);
			} else {
				emit selectOveFile(path);
			}
		}
	}
}

void FileBrowser::tableViewMouseClick(const QModelIndex& index) {
	chooseIndex(index, false, false);
}

void FileBrowser::tableViewMouseDoubleClick(const QModelIndex& index) {
	chooseIndex(index, true, true);
}

void FileBrowser::tableViewKeyPressed(int key) {
	switch (key) {
	case Qt::Key_Enter:
	case Qt::Key_Return: {
		intoButtonClicked();

		break;
	}
	case Qt::Key_Backspace: {
		upButtonClicked();

		break;
	}
	default:
		break;
	}
}

void FileBrowser::tableViewRowsInserted(const QModelIndex& /*parent*/, int start, int /*end*/) {
	if (needSelectItem_) {
		if (start == 0 && dirModel_->hasIndex(start, 0)) {
			QModelIndex index = dirModel_->index(start, 0);
			if (index != QModelIndex()) {
				dirTableView_->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
				dirTableView_->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
			}
		}

		needSelectItem_ = false;
	}
}

bool FileBrowser::jumpToNextSong() {
	if (indexFileExists(currentIndex_)) {
		QModelIndex nextIndex = dirModel_->index(currentIndex_.row() + 1, 0, dirTableView_->rootIndex());

		if (nextIndex != QModelIndex()) {
			chooseIndex(nextIndex, false, true);
			return true;
		}
	}

	return false;
}

bool FileBrowser::jumpToPreviousSong() {
	if (indexFileExists(currentIndex_)) {
		QModelIndex previousIndex = dirModel_->index(currentIndex_.row() - 1, 0, dirTableView_->rootIndex());

		if (previousIndex != QModelIndex()) {
			chooseIndex(previousIndex, false, true);
			return true;
		}
	}

	return false;
}

bool pathExists(const QString& path) {
	QFileInfo info(path);
	return info.exists();
}

bool FileBrowser::indexFileExists(const QModelIndex& index) {
	QString path = dirModel_->filePath(index);
	return pathExists(path);
}

void FileBrowser::onDropped(const QMimeData* mimeData) {
	if (mimeData->hasUrls()) {
		QList<QUrl> urlList = mimeData->urls();
		QString text;
		for (int i = 0; i < urlList.size() && i < 32; ++i) {
			QString url = urlList.at(i).path();
			if (url.size() > 0 && url[0] == '/') {
				url = url.right(url.size() - 1);
			}

			QFileInfo file(url);
			QDir dir;
			bool canDrop = false;

			if (file.isDir()) {
				canDrop = true;
				dir = QDir(url);
			}

			if (file.isFile() && file.exists()) {
				dir = file.absoluteDir();
				canDrop = true;
			}

			if (canDrop && dir.exists()) {
				setRootPath(dir.absolutePath());
			}
		}
	}
}
