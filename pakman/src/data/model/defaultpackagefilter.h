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

#ifndef DEFAULTPACKAGEFILTER_H
#define DEFAULTPACKAGEFILTER_H

#include "src/data/model/packagefilter.h"


class DefaultPackageFilter : public IPackageFilter {
public:
	typedef std::array<bool, PackageStatus::epkg_FOREIGN_OUTDATED+1> TStatusFilter;

public:
	DefaultPackageFilter();

	// this function should be as fast as possible
	virtual const PackageRepository::TListOfPackages& getBasePackageList(const PackageRepository& repo) override;
	// this function will be called for each item in the base package list
	virtual bool mustFilterPackage(const PackageRepository::PackageData& package) override;

	/// do not call any of the methods in this block unsynchronized
	void applyFilter(const bool explicitsVisible, const bool implicitsVisible,
	                 const bool requiredVisible, const bool notRequiredVisible, const TStatusFilter& filter);
	bool applyFilter(QSet<QString>&& filter);
	void applyGroupFilter(const QString& group); // NULL String == no filter
	void applySearchFilter(const int filterColumn);
	void applySearchFilter(const QString& filterExp);
	void applySearchFilter(const int filterColumn, const QString& filterExp);

private:
	inline bool mustFilterPackageByRepo(const PackageRepository::PackageData& package) const {
		return m_filterRepo.empty() == false && m_filterRepo.contains(package.repository) == false;
	}

private:
	// Filter attributes, for all bool true = visible
	bool          m_filterExplicitlyInstalled;
	bool          m_filterImplicitlyInstalled;
	bool          m_filterRequired;
	bool          m_filterNotRequired;
	TStatusFilter m_filterPackageStatus; // true = visible
	QString       m_filterPackagesNotInThisGroup;
	int           m_filterColumn;
	QSet<QString> m_filterRepo;          // contained = visible
	QRegExp       m_filterRegExp;
};

#endif // DEFAULTPACKAGEFILTER_H
