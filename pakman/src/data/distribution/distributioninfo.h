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

#ifndef DISTRIBUTIONINFO_H
#define DISTRIBUTIONINFO_H

#include <QString>
#include <QList>
#include <QMap>
#include <memory>
#include "src/data/packagedata.h"


/**
 * @brief Base class for distribution specific stuff (like news processing)
 */
class DistributionInfo
{
public:
	DistributionInfo();

	/**
	 * @brief will load distribution news from net or cache
	 * @return true if supported. The default implementation will clear the output and return false
	 */
	virtual bool retrieveNews(QString& output) const;

	/**
	 * @brief will load package info for all in list and store in a temp location
	 * @return true if successful
	 */
	virtual bool fetchAurInfoFor(const QList<PackageListData>& list) const;
	/**
	 * @brief returns true if there is no suitable aur info in cache and it should be fetched
	 */
	virtual bool shouldFetchAurInfo() const;
	/**
	 * @brief will return the local aur data from the temp location
	 * @return must NOT be nullptr
	 */
	virtual std::unique_ptr<QMap<QString, PackageListData>> retrieveAurInfo() const;

protected:
	/**
	 * @brief uses curl to retrieve distribution news (will also create a cache for offline access)
	 * @param rssAddress (https)
	 * @return rss style news or empty string
	 */
	static QString retrieveDistroNews(const QString& rssAddress);
};

#endif // DISTRIBUTIONINFO_H
