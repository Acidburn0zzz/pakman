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

#include "manjarolinuxadapter.h"

#include <QRegExp>


ManjaroLinuxAdapter::ManjaroLinuxAdapter()
{
}

/**
 * @brief access to rss news feed
 */
bool ManjaroLinuxAdapter::retrieveNews(QString &output) const
{
	output = DistributionInfo::retrieveDistroNews("http://manjaro.org/feed/");
	return true;
}

/**
 * @brief custom style for Manjaro Linux
 * @param link  (url to the news item)
 * @param title (headline)
 * @param date  (creation date of the entry)
 * @param text
 * @return as html (as table per item)
 */
QString ManjaroLinuxAdapter::formatNews(const QString& link, const QString& title, const QString& date,
                                        const QString& text) const
{
	QRegExp remImg;
	remImg.setMinimal(true);
	remImg.setPattern("<img.*>");
	QString news = text;
	news.replace(remImg, QString());
	return "<table style=\"margin-top:5px;\"><tr><td><a href=\"" + link + "\"><b><big>" + title
	    + "</b></big></a>  (" + date + ")<br>" + news + "</td></tr></table>";
}
