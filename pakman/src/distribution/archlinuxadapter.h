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

#ifndef ARCHLINUXINFO_H
#define ARCHLINUXINFO_H

#include "src/distribution/distributioninfo.h"


/**
 * @brief Arch Linux specific functionality
 */
class ArchLinuxAdapter : public DistributionInfo
{
public:
	ArchLinuxAdapter();

	// DistributionInfo interface
public:
	/**
	 * @brief No own implementation, just overrides the url
	 */
	virtual bool retrieveNews(QString& output) const override;
	/**
	 * @brief Will query the AUR for updates on all packages in list and store in cachedir
	 */
	virtual bool fetchAurInfoFor(const QList<PackageListData>& list) const override;
	/**
	 * @brief returns true if there is no suitable aur info in cache
	 */
	virtual bool shouldFetchAurInfo() const override;
	/**
	 * @brief will return the local aur data from the temp location
	 */
	virtual std::unique_ptr<QMap<QString, PackageListData>> retrieveAurInfo() const override;

private:
	const QString m_cachePath;
	const QString m_AURInfoFile;
};

#endif // ARCHLINUXINFO_H
