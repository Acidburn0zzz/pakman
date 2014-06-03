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

#include "ui/mainwindow.h"

#include <iostream>
#include <memory>
#include <unistd.h>
#include <QFile>
#include "external/qt-solutions/QtSingleApplication"
#include "src/commands/taskprocessor.h"
#include "src/data/distribution/archlinuxinfo.h"
#include "src/data/distribution/manjarolinuxadapter.h"
#include "src/strconstants.h"


int main(int argc, char *argv[])
{
	if (geteuid() == 0) { // Root
		std::cerr << strErrorDoNotRunAsRoot().toStdString() << std::endl;
		return 0;
	}

	QtSingleApplication app(argc, argv);

	if (app.isRunning()) {
		std::cerr << strErrorAlreadyRunning().toStdString() << std::endl;
		const bool result = !app.sendMessage("show");
		usleep(500000); //TODO seems to help qtsingleapp, look into it later
		return result;
	}

	TaskProcessor cpu;
	// initialize distribution specific adapter
	std::unique_ptr<DistributionInfo> distribution;
	{
		QFile file("/etc/os-release");
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QString contents = file.readAll();
			file.close();

			if (contents.contains(QRegExp("Arch Linux"))) {
				distribution.reset(new ArchLinuxAdapter());
			} else if (contents.contains(QRegExp("Manjaro"))) {
				distribution.reset(new ManjaroLinuxAdapter());
			}
		}
		if (distribution == nullptr) {
			distribution.reset(new ArchLinuxAdapter); // fallback is Arch Linux
		}
	}
	MainWindow w(*distribution, cpu);
	app.setActivationWindow(&w);
	app.connect(&app, SIGNAL(messageReceived(const QString &)), &app, SLOT(activateWindow()));
	w.show();

	return app.exec();
}
