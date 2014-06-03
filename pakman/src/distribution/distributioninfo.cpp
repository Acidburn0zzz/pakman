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

#include "distributioninfo.h"

#include <iostream>
#include <QDir>
#include <QTextStream>
#include "src/strconstants.h"
#include "src/commands/curlcommands.h"


DistributionInfo::DistributionInfo()
{
}

/**
 * @brief news is not supported without specialization
 */
bool DistributionInfo::retrieveNews(QString& output) const
{
	output.clear();
	return false;
}

/**
 * @brief default style is the Arch Linux style
 * @param link  (url to the news item)
 * @param title (headline)
 * @param date  (creation date of the entry)
 * @param text
 * @return as html (as table per item)
 */
QString DistributionInfo::formatNews(const QString& link, const QString& title, const QString& date,
                                     const QString& text) const
{
	return "<table style=\"margin-top:5px;\"><tr><td><a href=\"" + link + "\"><b><big>" + title
	    + "</b></big></a>  (" + date + ")" + text + "</td></tr></table>";
}

/**
 * @brief AUR is not supported without specialization
 */
bool DistributionInfo::fetchAurInfoFor(const QList<PackageListData>& ) const
{
	return false;
}

/**
 * @brief AUR is not supported without specialization
 */
bool DistributionInfo::shouldFetchAurInfo() const
{
	return false;
}

/**
 * @brief AUR is not supported without specialization
 */
std::unique_ptr<QMap<QString, PackageListData>> DistributionInfo::retrieveAurInfo() const
{
	return std::unique_ptr<QMap<QString, PackageListData>>(new QMap<QString, PackageListData>());
}

/*
 * Retrieves the distro RSS news feed from its respective site
 * If it fails to connect to the internet, uses the available "./.config/octopi/distro_rss.xml"
 * The result is a QString containing the RSS News Feed XML code
 *
 * based on Octopi
 */
QString DistributionInfo::retrieveDistroNews(const QString& rssAddress)
{
	QString result;
	const QString path(QDir::homePath() + QDir::separator() + strCacheDir());
	QDir().mkpath(path);
	QString tmpRssPath = path + ".tmp_distro_rss.xml";
	QString rssPath    = path + "distro_rss.xml";

	QString curlCommand = "%1 -o %2";
	curlCommand = curlCommand.arg(rssAddress).arg(tmpRssPath);

	QFile fileTmpRss(tmpRssPath);
	QFile fileRss(rssPath);

	if (CurlCommands::performQuery(curlCommand).isEmpty())
	{
		// Load the new file
		if (!fileTmpRss.open(QIODevice::ReadOnly | QIODevice::Text)) {
			result = "";
		}
		else {
			QTextStream in2(&fileTmpRss);
			result = in2.readAll();
			fileTmpRss.close();
		}
		/* For now we can always delete the rssFile once we reach this point,
		 * but this isnt supposed to be the final solution
		 */
		if (fileRss.exists()) fileRss.remove();
	}
	else {
		// curl failed (std::err), probably no internet connection
		std::cerr << strAppName() << " " << strErrorDnfDistributionNews404().toStdString() << std::endl;
	}

	// the rss from tmpFile is now in result. Result is empty on fail
	if (!fileRss.exists()) // first download
	{
		if (result.isEmpty() == false) {
			fileTmpRss.rename(tmpRssPath, rssPath);
		}
		else {
			// no old rss and no new, this path should produce an empty string
		}
	}
	else
	{
		//A old rss file exists. We have to make a diff
		QString contentsOldRss;
		if (!fileRss.open(QIODevice::ReadOnly | QIODevice::Text)) {
			contentsOldRss = "";
		}
		else {
			QTextStream in2(&fileRss);
			contentsOldRss = in2.readAll();
			fileRss.close();
		}
		if (result.isEmpty()) {
			// probably internet connection is not available
			result = contentsOldRss;
		}
		else {
			// not required atm
		}
	}

	return result;
}
