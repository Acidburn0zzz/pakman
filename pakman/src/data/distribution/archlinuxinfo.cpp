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

#include "archlinuxinfo.h"

#include <iostream>
#include <QDir>
#include <QVariantMap>
#include <qjson/parser.h>
#include "src/strconstants.h"
#include "src/commands/curlcommands.h"


ArchLinuxAdapter::ArchLinuxAdapter()
	: m_cachePath(QDir::homePath() + QDir::separator() + strCacheDir()),
	  m_AURInfoFile("aur_info.json")
{
}

bool ArchLinuxAdapter::fetchAurInfoFor(const QList<PackageListData>& list) const
{
	QDir().mkpath(m_cachePath);
	QString tmpAurInfoPath = m_cachePath + ".tmp_aur_info.json";
	QString aurInfoPath    = m_cachePath + m_AURInfoFile;

	QString aurInfoAddress("https://aur.archlinux.org/rpc.php?type=multiinfo");
	foreach(const PackageListData& pkg, list) {
		aurInfoAddress += "&arg%5B%5D=" + pkg.name;
	}

	QString curlCommand = "%1 -o %2";
	curlCommand = curlCommand.arg(aurInfoAddress).arg(tmpAurInfoPath);

	const bool resultOk = CurlCommands::performQuery(curlCommand).isEmpty();
	if (resultOk) {
		QFile fileAurInfo(aurInfoPath);
		/* for now we can always delete the AURInfoFile once we reach this point */
		if (fileAurInfo.exists()) fileAurInfo.remove();
		QFile::rename(tmpAurInfoPath, aurInfoPath);
		return true;
	}

	std::cerr << strAppName() << " " << strErrorDnfAUR404().toStdString() << std::endl;
	return resultOk;
}

bool ArchLinuxAdapter::shouldFetchAurInfo() const
{
	QString aurInfoPath = m_cachePath + m_AURInfoFile;
	return QFile::exists(aurInfoPath) == false;
}

std::unique_ptr<QMap<QString, PackageListData>> ArchLinuxAdapter::retrieveAurInfo() const
{
	QString aurInfoPath = m_cachePath + m_AURInfoFile;

	// Load the file
	QFile fileAurInfo(aurInfoPath);
	if (!fileAurInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return DistributionInfo::retrieveAurInfo();
	}
	else {
		QJson::Parser parser;
		bool ok;

		QVariantMap result = parser.parse(&fileAurInfo, &ok).toMap();
		if (!ok) {
			fileAurInfo.close();
			return DistributionInfo::retrieveAurInfo();
		}
		fileAurInfo.close();

		// start of extraction
		std::unique_ptr<QMap<QString, PackageListData>> data(new QMap<QString, PackageListData>());
		// plausibility checks
		if (result["version"].toString() != "1") return data;
		if (result["type"].toString() != "multiinfo") return data;

		const int count = result["resultcount"].toInt();
		QVariantList results = (result["results"].toList());
		if (results.length() != count) return data;
		// conversion
		foreach(const QVariant& result, results){
			QVariantMap aurPkg = result.toMap();
			QString name    = aurPkg["Name"].toString();
			QString version = aurPkg["Version"].toString();
			if (name.isEmpty() == false && version.isEmpty() == false)
				data->insert(name, PackageListData(name, "", version, "", epkg_FOREIGN));
		}
		return data;
	}
}

/**
 * @brief access to rss news feed
 */
bool ArchLinuxAdapter::retrieveNews(QString& output) const
{
	output = DistributionInfo::retrieveDistroNews("https://www.archlinux.org/feeds/news/");
	return true;
}
