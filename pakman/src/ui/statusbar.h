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

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>


class StatusBar : public QStatusBar
{
	Q_OBJECT
public:
	explicit StatusBar(QWidget *parent = nullptr);

	inline void updateStatus(const QString& activity, int val, int max) {
		emit changeStatusSignal(activity, val, max);
	}
	inline void updateSelected(int selected) {
		emit changeSelectedSignal(selected);
	}
	inline void updatePackagesInfo(int installed, int outdated, int total) {
		emit changePackagesInfoSignal(installed, outdated, total);
	}

private:
	QLabel*const  m_labelPkgCount;     //WEAK
	QLabel*const  m_labelSeparator1;   //WEAK
	QLabel*const  m_labelPkgInstalled; //WEAK
	QLabel*const  m_labelSeparator2;   //WEAK
	QLabel*const  m_labelPkgOutdated;  //WEAK
	QLabel*const  m_labelSeparator3;   //WEAK
	QLabel*const  m_labelSelPkgCount;  //WEAK

	QLabel*const  m_labelActivity;     //WEAK
	QProgressBar*const m_progress;     //WEAK

signals:
	void changeStatusSignal(QString activity, int val, int max);
	void changeSelectedSignal(int selected);
	void changePackagesInfoSignal(int installed, int outdated, int total);
	void updateReportRequested();

private slots:
	void changeStatusImpl(QString activity, int val, int max);
	void changeSelected(int selected);
	void changePackagesInfo(int installed, int outdated, int total);

private:
	inline QLabel* crtNlbl() {
		return new QLabel(this);
	}
};

#endif // STATUSBAR_H
