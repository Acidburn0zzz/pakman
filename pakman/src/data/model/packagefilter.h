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

#ifndef PACKAGEFILTER_H
#define PACKAGEFILTER_H

#include <QRegExp>
#include "src/data/packagerepository.h"


class IPackageFilter {
public:
	virtual ~IPackageFilter() {}

	// this function should be as fast as possible
	// return value should be provided sorted by name (should already be done by repo)
	virtual const PackageRepository::TListOfPackages& getBasePackageList(const PackageRepository& repo) = 0;
	// this function will be called for each item in the base package list
	virtual bool mustFilterPackage(const PackageRepository::PackageData& package) = 0;
};

#endif // PACKAGEFILTER_H
