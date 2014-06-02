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

#include "pacmancommands.h"

#include <QProcess>


PacmanCommands::PacmanCommands()
{
}


/*
 * Performs a pacman query
 *
 * from Octopi
 */
QByteArray PacmanCommands::performQuery(const bool asRoot, const QString &args,
                                        const bool localized, const bool fallbackToStderr)
{
	QByteArray result;
	QProcess pacman;

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	if (localized == false) {
		env.insert("LANG", "C");
		env.insert("LC_MESSAGES", "C");
		env.insert("LC_ALL", "C");
		pacman.setProcessEnvironment(env);
	}

	QString cmd(asRoot ? "/bin/kdesu " : ""); //TODO: kdesu should be configurable, also may need dbus-launch
	cmd += "/bin/pacman ";
	cmd += args;

	pacman.start(cmd);
	pacman.waitForFinished(-1);
	result = pacman.readAllStandardOutput();
	if (result.isEmpty() && fallbackToStderr) result = pacman.readAllStandardError();

	pacman.close();
	return result;
}

/*
 * Returns a string with the list of all packages available in all repositories
 * (installed + not installed)
 *
 * from Octopi
 */
QByteArray PacmanCommands::getPackageList()
{
	QString args("-Ss");
	QByteArray result = performQuery(false, args, false, false);
	return result;
}

/**
 * @brief returns a list of packages not found in the official repos
 */
QByteArray PacmanCommands::getPackageListForeign()
{
	QString args("-Qm");
	QByteArray result = performQuery(false, args, false, false);
	return result;
}

/*
 * Given a package name and if it is default to the official repositories,
 * returns a string containing all of its information fields
 * (ex: name, description, version, dependsOn...)
 *
 * from Octopi
 */
QByteArray PacmanCommands::getPackageDetails(const QString& pkgName, bool installedPackage)
{
	QString args = (installedPackage ? "-Qi" : "-Si");

	if (pkgName.isEmpty() == false) { // enables get for all ("")
		args += " ";
		args += pkgName;
	}

	QByteArray result = performQuery(false, args, false, false);
	return result;
}

/*
 * Retrives the list of package groups
 *
 * from Octopi
 */
QByteArray PacmanCommands::getPackageGroups()
{
	QString args("-Spg");
	QByteArray res = performQuery(false, args, false, false);
	return res;
}

/*
 * Retrives the list of package for a specific groups (name\nname...)
 */
QByteArray PacmanCommands::getPackageListForGroup(const QString& groupName)
{
	QString args("-Spg ");
	args.append(groupName);
	QByteArray result = performQuery(false, args, false, false);
	return result;
}

/*
 * Returns a string containing all packages which were explicitly installed
 */
QByteArray PacmanCommands::getExplicitlyInstalledPackageList()
{
	QString args("-Qe");
	QByteArray result = performQuery(false, args, false, false);
	return result;
}

/*
 * Returns a string containing all packages no one depends on
 *
 * from Octopi
 */
QByteArray PacmanCommands::getUnrequiredPackageList()
{
	QString args("-Qt");
	QByteArray result = performQuery(false, args, false, false);
	return result;
}

/**
 * @return Provider for executable file "-Qo"
 */
QByteArray PacmanCommands::getProviderFor(const QString& executable)
{
	QString args("-Qo \"" + executable + "\"");
	QByteArray result = performQuery(false, args, true, true);
	return result;
}

QByteArray PacmanCommands::synchronizeRepositories()
{
	QString args("-Sy");
	QByteArray result = performQuery(true, args, false, false);
	return result;
}
