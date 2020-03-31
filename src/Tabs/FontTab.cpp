/************************************************************************
**
**  Copyright (C) 2019-2020 Kevin B. Hendricks, Stratford, Ontario Canada
**  Copyright (C) 2012      John Schember <john@nachtimwald.com>
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

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtWidgets/QLayout>
#include <QGuiApplication>
#include <QApplication>
#include "MainUI/MainWindow.h"
#include "Widgets/FontView.h"
#include "Misc/Utility.h"

#include "Tabs/FontTab.h"

FontTab::FontTab(Resource *resource, QWidget *parent)
    : ContentTab(resource, parent),
      m_fv(new FontView(this))
{
    m_Layout->addWidget(m_fv);
    ShowFont();
    ConnectSignalsToSlots();
}

void FontTab::ShowFont()
{
    m_fv->ShowFont(m_Resource->GetFullPath());
}

void FontTab::RefreshContent()
{
    m_fv->ReloadViewer();
}

void FontTab::ConnectSignalsToSlots()
{
    connect(m_Resource, SIGNAL(ResourceUpdatedOnDisk()), this, SLOT(RefreshContent()));
    connect(m_Resource, SIGNAL(Deleted(const Resource*)), this, SLOT(Close()));
}

