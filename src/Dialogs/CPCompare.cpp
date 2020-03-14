/************************************************************************
 **
 **  Copyright (C) 2020 Kevin B. Hendricks, Stratford Ontario Canada
 **
 **  This file is part of Sigil.
 **
 **  Sigil is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Sigil is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/
#include <QString>
#include <QList>
#include <QDialog>
#include <QWidget>
#include <QKeySequence>
#include <QKeyEvent>
#include <QLabel>
#include <QToolButton>
#include <QListWidget>
#include <QApplication>
#include <QtConcurrent>
#include <QFuture>
#include <QFileInfo>
#include <QDebug>

#include "Dialogs/ListSelector.h"
#include "Dialogs/SourceViewer.h"
#include "Dialogs/ChgViewer.h"
#include "Misc/SettingsStore.h"
#include "Misc/Utility.h"
#include "Misc/DiffRec.h"
#include "Misc/PythonRoutines.h"

#include "Dialogs/CPCompare.h"

static const QString SETTINGS_GROUP = "checkpoint_compare";

static const QStringList TEXT_EXTENSIONS = QStringList() << "css" << "htm" << "html" <<
                                                            "js" << "ncx" << "opf" << "pls" << "smil" <<
                                                            "svg" << "ttml" << "txt" << "vtt" << "xhtml" <<
                                                            "xml" << "xpgt";
CPCompare::CPCompare(const QString& bookroot,
		     const QString& cpdir,
		     const QStringList& dlist,
		     const QStringList& alist,
		     const QStringList& mlist,
		     QWidget * parent)
    : QDialog(parent),
      m_bookroot(bookroot),
      m_cpdir(cpdir),
      m_bp(new QToolButton(this)),
      m_layout(new QVBoxLayout(this))
{
    m_dlist = new ListSelector(tr("Files Only in Checkpoint"), tr("View"), dlist, this);
    m_alist = new ListSelector(tr("Files Only in Current ePub"), tr("View"), alist, this);
    m_mlist = new ListSelector(tr("Modified since Checkpoint"), tr("View"), mlist, this);
    setWindowTitle(tr("Results of Comparison"));
    m_bp->setText(tr("Done"));
    m_bp->setToolButtonStyle(Qt::ToolButtonTextOnly);
    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(m_dlist);
    hl->addWidget(m_alist);
    hl->addWidget(m_mlist);
    m_layout->addLayout(hl);
    QHBoxLayout* hl2 = new QHBoxLayout();
    hl2->addStretch(0);
    hl2->addWidget(m_bp);
    m_layout->addLayout(hl2);
    ReadSettings();
    connectSignalsToSlots();
}

void CPCompare::handle_del_request()
{
    // only exists in checkpoint
    QStringList pathlist = m_dlist->get_selections();
    foreach(QString apath, pathlist) {
	QString filepath = m_cpdir + "/" + apath;
	QFileInfo fi(filepath);
	if (TEXT_EXTENSIONS.contains(fi.suffix().toLower())) {
	    QString data = Utility::ReadUnicodeTextFile(filepath);
	    SourceViewer* sv = new SourceViewer(apath, data, this);
	    sv->show();
	    sv->raise();
	} else {
	    qDebug() << "attempted to show a binary file " << apath;
	}
    }
}

void CPCompare::handle_add_request()
{
    // only exists in current epub
    QStringList pathlist = m_alist->get_selections();
    foreach(QString apath, pathlist) {
        QString filepath = m_bookroot + "/" + apath;
	QFileInfo fi(filepath);
	if (TEXT_EXTENSIONS.contains(fi.suffix().toLower())) {
	    QString data = Utility::ReadUnicodeTextFile(filepath);
	    SourceViewer* sv = new SourceViewer(apath, data, this);
	    sv->show();
	    sv->raise();
	} else {
	    qDebug() << "attempted to show a binary file " << apath;
	}
    }
}

void CPCompare::handle_mod_request()
{
    QStringList pathlist = m_mlist->get_selections();
    PythonRoutines pr;
    foreach(QString apath, pathlist) {
	QString leftpath = m_cpdir + "/" + apath;
	QString rightpath = m_bookroot + "/" + apath;
	QFileInfo fi(rightpath);
	if (TEXT_EXTENSIONS.contains(fi.suffix().toLower())) {
	    QApplication::setOverrideCursor(Qt::WaitCursor);
	    QFuture<QList<DiffRecord::DiffRec>> bfuture = QtConcurrent::run(&pr, 
									&PythonRoutines::GenerateParsedNDiffInPython,
									leftpath, rightpath);
	    bfuture.waitForFinished();
	    QList<DiffRecord::DiffRec> diffinfo = bfuture.result();
	    QApplication::restoreOverrideCursor();
	    ChgViewer* cv = new ChgViewer(diffinfo, "Checkpoint: "+apath, "Current: "+apath, this);
	    cv->show();
	    cv->raise();
	} else {
	    qDebug() << "attempted to show a binary file " << apath;
	}
    }
}

void CPCompare::handle_cleanup()
{
}

CPCompare::~CPCompare()
{
    WriteSettings();
}

#if 0
void CPCompare::keyPressEvent(QKeyEvent * ev)
{
    if ((ev->key() == Qt::Key_Enter) || (ev->key() == Qt::Key_Return)) return;

    if (ev->key() == Qt::Key_Slash) {
	m_nav->set_focus_on_search();
	return;
    }

    if (ev->matches(QKeySequence::Copy)) {
	QString text = m_view1->GetSelectedText() + m_view2->GetSelectedText();
	if (!text.isEmpty()) {
	    QApplication::clipboard()->setText(text);
	}
	return;
    }

    if (ev->matches(QKeySequence::FindNext)) {
	do_search(false);
        return;
    }
    if (ev->matches(QKeySequence::FindPrevious)) {
	do_search(true);
        return;
    }
    return QDialog::keyPressEvent(ev);
}
#endif

void CPCompare::ReadSettings()
{
    SettingsStore settings;
    settings.beginGroup(SETTINGS_GROUP);
    // The size of the window and it's full screen status
    QByteArray geometry = settings.value("geometry").toByteArray();
    if (!geometry.isNull()) {
        restoreGeometry(geometry);
    }
    settings.endGroup();
}

void CPCompare::WriteSettings()
{
    SettingsStore settings;
    settings.beginGroup(SETTINGS_GROUP);
    // The size of the window and it's full screen status
    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

int CPCompare::exec()
{
    return QDialog::exec();
}

// should cover both escape key use and using x to close the runner dialog
void CPCompare::reject()
{
    handle_cleanup();
    QDialog::reject();
}

// should cover both escape key use and using x to close the runner dialog
void CPCompare::accept()
{
    handle_cleanup();
    QDialog::accept();
}

void CPCompare::connectSignalsToSlots()
{
    connect(m_bp,  SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_dlist, SIGNAL(ViewRequest()), this, SLOT(handle_del_request()));
    connect(m_alist, SIGNAL(ViewRequest()), this, SLOT(handle_add_request()));
    connect(m_mlist, SIGNAL(ViewRequest()), this, SLOT(handle_mod_request()));
}
