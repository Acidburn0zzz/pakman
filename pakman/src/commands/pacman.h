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

#ifndef PACMAN_H
#define PACMAN_H

#include <memory>
#include <QList>
#include <QString>

#include "src/data/packagedata.h"


//TODO: more documentation
/**
 * @brief collection of all available pacman commands, will deliver parsed data
 *
 * see pacmancommands.h for retrieval of raw data
 */
namespace Pacman
{
	/**
	 * @brief Package Information "-Ss"
	 */
	std::unique_ptr<QList<PackageListData>>   getPackageList();
	/**
	 * @brief Package Information "-Qm"
	 */
	std::unique_ptr<QList<PackageListData>>   getPackageListForeign();
	/**
	 * @brief Package Detail Information "-Si %pkgName" or "-Qi %pkgName" for installed
	 * @param pkgName may be plain or (only for not-installed) in repo/name notation
	 */
	std::unique_ptr<QList<PackageDetailData>> getPackageDetails(const QString& pkgName, bool installedPackage = false);
	/**
	 * @brief List of all distinct package groups sorted by name "-Spg"
	 */
	std::unique_ptr<QStringList>   getPackageGroups();
	std::unique_ptr<QStringList>   getPackageListForGroup(const QString& groupName);
	std::unique_ptr<QSet<QString>> getExplicitPackageList();
	std::unique_ptr<QSet<QString>> getUnrequiredPackageList();
	void synchronizeRepositories();

	// Helper functions
	int rpmvercmp(const char* a, const char* b);
	QString extractFieldFromInfo(const QString& field, const QString& pkgInfo);
	QString getName(const QString& pkgInfo);
	QString getDescription(const QString& pkgInfo);
	QString getVersion(const QString& pkgInfo);
	QString getRepository(const QString& pkgInfo);
	QString getURL(const QString& pkgInfo);
	QString getLicense(const QString& pkgInfo);
	QString getGroup(const QString& pkgInfo);
	QString getProvides(const QString& pkgInfo);
	QString getDependsOn(const QString& pkgInfo);
	QString getOptDepends(const QString& pkgInfo);
	QString getConflictsWith(const QString& pkgInfo);
	QString getReplaces(const QString& pkgInfo);
	QString getRequiredBy(const QString& pkgInfo);
	QString getOptionalFor(const QString& pkgInfo);
	QString getPackager(const QString& pkgInfo);
	QString getArch(const QString& pkgInfo);
	QDateTime getBuildDate(const QString& pkgInfo);
	double getDownloadSize(const QString& pkgInfo);
	double getInstalledSize(const QString& pkgInfo);
};

#endif // PACMAN_H
