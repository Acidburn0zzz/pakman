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

#include "packagefilter.h"

#include "packagemodel.h"


DefaultPackageFilter::DefaultPackageFilter()
	: m_filterPackagesNotInstalled(false), m_filterPackagesNotInThisGroup(""),
		m_filterColumn(-1), m_filterRegExp("", Qt::CaseInsensitive, QRegExp::RegExp)
{}

const PackageRepository::TListOfPackages& DefaultPackageFilter::getBasePackageList(const PackageRepository& repo)
{
	return repo.getPackageList(m_filterPackagesNotInThisGroup);
}

bool DefaultPackageFilter::mustFilterPackage(const PackageRepository::PackageData& package)
{
	if (m_filterPackagesNotInstalled == false || package.installed()) {
		if (m_filterRegExp.isEmpty()) {
			return false;
		}
		else {
			switch (m_filterColumn) {
			case PackageModel::ctn_PACKAG_NAME_COLUMN:
				if (m_filterRegExp.indexIn(package.name) != -1) return false;
				else return true;
			case PackageModel::ctn_PACKAGE_DESCRIPTION_FILTER_NO_COLUMN:
				if (m_filterRegExp.indexIn(package.description) != -1) return false;
				else return true;
			default:
				return false;
			}
		}
	}
}

void DefaultPackageFilter::applyFilter(bool packagesNotInstalled, const QString& group)
{
//  std::cout << "apply new group filter " << (packagesNotInstalled ? "true" : "false") << ", " << group.toStdString() << std::endl;
	m_filterPackagesNotInstalled   = packagesNotInstalled;
	m_filterPackagesNotInThisGroup = group;
}

void DefaultPackageFilter::applyFilter(const int filterColumn)
{
	applyFilter(filterColumn, m_filterRegExp.pattern());
}

void DefaultPackageFilter::applyFilter(const QString& filterExp)
{
	applyFilter(m_filterColumn, filterExp);
}

void DefaultPackageFilter::applyFilter(const int filterColumn, const QString& filterExp)
{
	assert(filterExp.isNull() == false);
//  std::cout << "apply new column filter " << filterColumn << ", " << filterExp.toStdString() << std::endl;
	m_filterColumn = filterColumn;
	m_filterRegExp.setPattern(filterExp);
}
