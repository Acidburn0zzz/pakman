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

#ifndef INFOTABS_H
#define INFOTABS_H

#include <QWidget>

namespace Ui {
class InfoTabs;
}

class PackageDetailData;
class PackageListData;


/**
 * @brief manages tabs shown in the bottom area
 */
class InfoTabs : public QWidget
{
	Q_OBJECT

public:
	explicit InfoTabs(QWidget *parent = 0);
	~InfoTabs();

	/**
	 * @brief will show %help in the infoBrowser
	 * @param help (html style)
	 * @param activate (the info tab)
	 */
	void showHelp(const QString& help, bool activate);
	/**
	 * @brief will format news and update the text of the newsBrowser
	 * @param news (rss style)
	 */
	void showNews(const QString& news);
	/**
	 * @brief will format detail information about a package and show in the infoBrowser
	 * @param pkg (usually obtained via Pacman::getPackageDetails)
	 * @param pkgInstalled (may be null, else additional info like size- and version-diff will be shown)
	 * @param pkgAurDetails (may be null, else pkgInstalled should be nullptr and pkg contain the installed data instead)
	 */
	void showPackageInfo(const PackageDetailData& pkg, const PackageDetailData* pkgInstalled = nullptr,
	                     const PackageListData*const pkgAurDetails = nullptr);
	/**
	 * @brief will show a report about packages out of sync in the info browser
	 */
	void showUpdateReport(const QList<PackageDetailData>& pacmanInstalled, const QList<PackageDetailData>& pacmanRepo,
	                      const QList<PackageDetailData>& aurInstalled, const QMap<QString, PackageListData>& aurRepo);

private:
	/**
	 * @brief format rss style news
	 * @param rss style news
	 * @return html like news
	 */
	QString parseRssNews(const QString& news);
	/**
	 * @brief will format detail information about a single package for display in the infoBrowser
	 * @param pkg detail information
	 * @param pkgInstalled (may be null, else additional info like size- and version-diff will be shown)
	 * @param pkgAurDetails (may be null, else pkgInstalled should be nullptr and pkg contain the installed data instead)
	 * @return html like info
	 */
	QString formatPackageInfo(const PackageDetailData& pkg, const PackageDetailData*const pkgInstalled = nullptr,
	                          const PackageListData*const pkgAurDetails = nullptr);
	/**
	 * @brief will format data as 2 columns for display in the package info table
	 * @param td1 is the html-ready content of the left column
	 * @param td2 is the html-ready content of the right column
	 * @return html row (<tr>...</tr>)
	 */
	QString formatPackageInfoRow(const QString& td1, const QString td2);
	/**
	 * @brief same as above, just more columns
	 */
	QString formatPackageInfoRow(const QString& td1, const QString& td2, const QString& td3, const QString& td4,
	                             const QString& td5, const QString& td6);

public:
	/**
	 * @brief sanitize will mask < and >
	 * @param UTF8 input string
	 * @return sanitized input string
	 */
	static QString sanitize(const QString& input);

private:
	Ui::InfoTabs *ui;
};

#endif // INFOTABS_H
