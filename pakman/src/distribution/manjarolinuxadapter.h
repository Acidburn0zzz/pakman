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

#ifndef MANJAROLINUXADAPTER_H
#define MANJAROLINUXADAPTER_H

#include "src/distribution/archlinuxadapter.h"


class ManjaroLinuxAdapter : public ArchLinuxAdapter
{
public:
	ManjaroLinuxAdapter();

	// DistributionInfo interface
public:
	/**
	 * @brief No own implementation, just overrides the url
	 */
	virtual bool retrieveNews(QString& output) const override;
	/**
	 * @brief will format a rss news item
	 * @return news item as html
	 */
	virtual QString formatNews(const QString& link, const QString& title, const QString& date,
	                           const QString& text) const override;
};

#endif // MANJAROLINUXADAPTER_H
