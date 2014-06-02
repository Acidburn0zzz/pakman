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

#ifndef PACMANCOMMANDS_H
#define PACMANCOMMANDS_H

#include <QByteArray>


/**
 * @brief collection of all available pacman commands, will deliver raw data
 *
 * see pacman.h for retrieval of parsed data
 */
class PacmanCommands
{
public:
	PacmanCommands();

	/**
	 * @brief will execute pacman in default lang with $args
	 * @param asRoot will start the process as root (true) or current user (false)
	 * @param args e.g "-Syu"
	 * @param localized (if false the query will be executed with LANG C etc.)
	 * @param fallbackToStderr (if true stderr will be returned if stdout is empty)
	 * @return raw data stdout
	 */
	static QByteArray performQuery(const bool asRoot, const QString& args,
	                               const bool localized, const bool fallbackToStderr);

	/**
	 * @brief Repo-PackageList "-Ss"
	 */
	static QByteArray getPackageList();
	/**
	 * @brief Repo-PackageList "-Qm"
	 */
	static QByteArray getPackageListForeign();
	/**
	 * @brief Package Detail Information "-Si %pkgName" or "-Qi %pkgName" for installed
	 */
	static QByteArray getPackageDetails(const QString& pkgName, bool installedPackage = false);
	/**
	 * @brief Repo-PackageGroups "-Spg"
	 */
	static QByteArray getPackageGroups();
	/**
	 * @brief PackageGroup-Members "-Spg %group"
	 */
	static QByteArray getPackageListForGroup(const QString& groupName);
	/**
	 * @brief ExplicitlyInstalledPackageList "-Qe" (not as dependency)
	 */
	static QByteArray getExplicitlyInstalledPackageList();
	/**
	 * @brief UnrequiredPackageList "-Qt" (noone depends on)
	 */
	static QByteArray getUnrequiredPackageList();
	/**
	 * @brief Provider for executable file "-Qo"
	 */
	static QByteArray getProviderFor(const QString& executable);
	/**
	 * @brief Sync the repos "-Sy"
	 */
	static QByteArray synchronizeRepositories();
};

#endif // PACMANCOMMANDS_H
