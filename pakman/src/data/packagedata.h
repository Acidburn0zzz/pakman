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

#ifndef PACKAGEDATA_H
#define PACKAGEDATA_H

#include <QString>
#include <QDateTime>


/**
 * @brief Status of a package (pacman or other). Order of entries is important for sorting
 */
enum PackageStatus {
	epkg_OUTDATED,         // installed but outdated
	epkg_NEWER,            // installed and newer than repo
	epkg_INSTALLED,        // installed and in sync
	epkg_NON_INSTALLED,    // not installed
	epkg_FOREIGN,          // installed, but not in the official repos
	epkg_FOREIGN_OUTDATED  // installed, but not in the official repos and outdated
};


/**
 * @brief For intermediate data storage / transport
 *
 * from Octopi
 */
struct PackageListData {
	QString name;           // may contain alphanumeric and @._+-, should be lower case
	QString repository;
	QString version;        // may contain alphanumeric and _.-
	QString description;
	QString outatedVersion; // may contain alphanumeric and _.-
//	int    popularity;    // votes
	PackageStatus status;   // see description of PackageStatus

	PackageListData(QString n, QString r, QString v, QString d, PackageStatus pkgStatus, QString outVersion="")
		: name(n), repository(r), version(v), description(d), outatedVersion(outVersion.trimmed()),
			status(pkgStatus)
	{}
};

//TODO more documentation
/**
 * @brief For intermediate data storage / transport of package details
 *
 * will be imported as UTF8
 *
 * from Octopi
 */
 struct PackageDetailData {
	QString name;          // may contain alphanumeric and @._+-
	QString repository;
	QString version;       // may contain alphanumeric and _.-
	QString url;
	QString license;
	QString group;
	QString provides;      // may contain all of name and <>=
	QString requiredBy;
	QString optionalFor;
	QString dependsOn;     // may contain all of name and <>=
	QString optDepends;
	QString conflictsWith; // may contain all of name and <>=
	QString replaces;
	QString packager;      // may contain non ascii and special chars (>,<,ö...)
	QString arch;
	QString description;
	QDateTime buildDate;
	double downloadSize;   // expected in KiB
	double installedSize;  // expected in KiB

	PackageDetailData()
	: downloadSize(0.0), installedSize(0.0)
	{}
};

#endif // PACKAGEDATA_H
