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

#include "defaultpackagefilter.h"

#include "packagemodel.h"


DefaultPackageFilter::DefaultPackageFilter()
	: m_filterExplicitlyInstalled(true), m_filterImplicitlyInstalled(true),
    m_filterRequired(true), m_filterNotRequired(true), m_filterPackagesNotInThisGroup(),
	  m_filterColumn(-1), m_filterRegExp("", Qt::CaseInsensitive, QRegExp::RegExp)
{
	m_filterPackageStatus.fill(true);
}

const PackageRepository::TListOfPackages& DefaultPackageFilter::getBasePackageList(const PackageRepository& repo)
{
	return repo.getPackageList(m_filterPackagesNotInThisGroup);
}

bool DefaultPackageFilter::mustFilterPackage(const PackageRepository::PackageData& package)
{
	if ((m_filterImplicitlyInstalled == false && package.explicitlyInstalled == false
	     && package.status != epkg_NON_INSTALLED)
	    || (m_filterExplicitlyInstalled == false && package.explicitlyInstalled == true))
		return true;

	if ((m_filterRequired == false && package.required == true && package.status != epkg_NON_INSTALLED)
	    || (m_filterNotRequired == false && package.required == false))
		return true;

	if (m_filterPackageStatus[package.status] == false)
		return true;

	if (m_filterRegExp.isEmpty() == false) {
		switch (m_filterColumn) {
		case PackageModel::ctn_PACKAGE_NAME_COLUMN:
			if (m_filterRegExp.indexIn(package.name) == -1)
				return true;
			break;
		case PackageModel::ctn_PACKAGE_DESCRIPTION_FILTER_NO_COLUMN:
			if (m_filterRegExp.indexIn(package.description) == -1)
				return true;
			break;
		default:
			assert(false);
			break;
		}
	}
	return mustFilterPackageByRepo(package);
}

void DefaultPackageFilter::applyFilter(const bool explicitsVisible, const bool implicitsVisible,
                                       const bool requiredVisible, const bool notRequiredVisible,
                                       const TStatusFilter& filter)
{
	m_filterExplicitlyInstalled = explicitsVisible;
	m_filterImplicitlyInstalled = implicitsVisible;
	m_filterRequired = requiredVisible;
	m_filterNotRequired = notRequiredVisible;
	m_filterPackageStatus = filter;
}

bool DefaultPackageFilter::applyFilter(QSet<QString>&& filter)
{
	if (filter == m_filterRepo) return false;

	m_filterRepo = std::move(filter);
	return true;
}

void DefaultPackageFilter::applyGroupFilter(const QString& group)
{
	m_filterPackagesNotInThisGroup = group;
}

void DefaultPackageFilter::applySearchFilter(const int filterColumn)
{
	applySearchFilter(filterColumn, m_filterRegExp.pattern());
}

void DefaultPackageFilter::applySearchFilter(const QString& filterExp)
{
	applySearchFilter(m_filterColumn, filterExp);
}

void DefaultPackageFilter::applySearchFilter(const int filterColumn, const QString& filterExp)
{
	assert(filterExp.isNull() == false);
//  std::cout << "apply new column filter " << filterColumn << ", " << filterExp.toStdString() << std::endl;
	m_filterColumn = filterColumn;
	m_filterRegExp.setPattern(filterExp);
}
