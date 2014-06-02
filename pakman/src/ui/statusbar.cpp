/*
* This file is part of pakman, an open-source GUI for pacman.
* Copyright (C) 2014 Thomas Binkau
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

#include "statusbar.h"

#include "src/strconstants.h"


StatusBar::StatusBar(QWidget *parent)
	: QStatusBar(parent), m_labelPkgCount(crtNlbl()), m_labelSeparator1(crtNlbl()),
	  m_labelPkgInstalled(crtNlbl()), m_labelSeparator2(crtNlbl()), m_labelPkgOutdated(crtNlbl()),
	  m_labelSeparator3(crtNlbl()), m_labelSelPkgCount(crtNlbl()),
	  m_labelActivity(new QLabel(strStatusLoading(), this)), m_progress(new QProgressBar(this))
{
	setContentsMargins(QMargins(9,0,0,0));
	m_labelSeparator1->setFrameStyle(QFrame::VLine|QFrame::Plain);
	m_labelSeparator2->setFrameStyle(QFrame::VLine|QFrame::Plain);
	m_labelSeparator3->setFrameStyle(QFrame::VLine|QFrame::Plain);

	connect(this, SIGNAL(changeStatusSignal(QString,int,int)),
	        this, SLOT(changeStatusImpl(QString,int,int)), Qt::QueuedConnection);
	connect(this, SIGNAL(changeSelectedSignal(int)),
	        this, SLOT(changeSelected(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(changePackagesInfoSignal(int,int,int)),
	        this, SLOT(changePackagesInfo(int,int,int)), Qt::QueuedConnection);

	m_progress->setMaximumWidth(75);
	addWidget(m_labelSelPkgCount);
	addWidget(m_labelSeparator1);
	addWidget(m_labelPkgInstalled);
	addWidget(m_labelSeparator2);
	addWidget(m_labelPkgOutdated);
	addWidget(m_labelSeparator3);
	addWidget(m_labelPkgCount);

	connect(m_labelPkgOutdated, SIGNAL(linkActivated(QString)), this, SIGNAL(updateReportRequested()));

	updateSelected(0);
	updatePackagesInfo(0,0,0);

	addPermanentWidget(m_labelActivity);
	addPermanentWidget(m_progress);
}

void StatusBar::changeStatusImpl(QString activity, int val, int max)
{
	if (val == 0 && max == 0 && activity.isNull()) {
		// task done
		activity = strStatusReady();
		val = 1;
		max = 1;
	}
	if (activity.isNull() == false) m_labelActivity->setText(activity);
	m_progress->setMaximum(max);
	m_progress->setValue(val);
}

/// "%number package(s) selected"
void StatusBar::changeSelected(int selected)
{
	m_labelSelPkgCount->setText(QString::number(selected) + " "
	                            + (selected == 1 ? strPackage() : strPackages())
	                            + " " + strPackageSelected());
}

/// "%number installed"AND "%number outdated (report)" AND "%number total"
void StatusBar::changePackagesInfo(int installed, int outdated, int total)
{
	//TODO: this could actually show filtered/visible packages as well
	m_labelPkgInstalled->setText(QString::number(installed) + " " + strPackageInstalled());
	if (outdated == 0)
		m_labelPkgOutdated->setText(QString::number(outdated) + " " + strPackageOutdated());
	else {
		m_labelPkgOutdated->setText("<b>" + QString::number(outdated) + "</b> " + strPackageOutdated()
		                            + " (<a href=\"#\">report</a>)");
	}
	m_labelPkgCount->setText(QString::number(total) + " " + strPackageTotal());
}
