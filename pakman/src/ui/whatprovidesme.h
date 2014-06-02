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

#ifndef WHATPROVIDESME_H
#define WHATPROVIDESME_H

#include <QDialog>

namespace Ui {
class DlgWhatProvidesMe;
}

class DlgWhatProvidesMe : public QDialog
{
	Q_OBJECT

public:
	explicit DlgWhatProvidesMe(QWidget *parent = 0);
	~DlgWhatProvidesMe();


private slots:
	void on_pushButton_clicked();

private:
	Ui::DlgWhatProvidesMe *ui;
};

#endif // WHATPROVIDESME_H
