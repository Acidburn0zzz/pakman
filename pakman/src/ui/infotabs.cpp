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

#include "src/ui/infotabs.h"
#include "ui_infotabs.h"

#include <QDomDocument>
#include <kiconloader.h>
#include "src/data/packagedata.h"
#include "src/distribution/distributioninfo.h"
#include "src/strconstants.h"


InfoTabs::InfoTabs(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::InfoTabs)
{
	ui->setupUi(this);
}

InfoTabs::~InfoTabs()
{
	delete ui;
}

void InfoTabs::showHelp(const QString& help, bool activate)
{
	// update
	QString finalHelpText(help);
	KIconLoader iconLoader;
	const QString iconSecurityHigh = iconLoader.iconPath("security-high", -1, false);
	const QString iconSecurityMedium = iconLoader.iconPath("security-medium", -1, false);
	const QString iconSecurityLow = iconLoader.iconPath("security-low", -1, false);
	finalHelpText.replace(QString("%security-high%"), iconSecurityHigh);
	finalHelpText.replace(QString("%security-medium%"), iconSecurityMedium);
	finalHelpText.replace(QString("%security-low%"), iconSecurityLow);
	ui->infoBrowser->setHtml(finalHelpText);
	// and activate info tab
	if (activate && ui->tabWidget->currentWidget() != ui->tabInfo)
		ui->tabWidget->setCurrentWidget(ui->tabInfo);
}

void InfoTabs::showNews(const QString& news, const DistributionInfo& formatter)
{
	ui->newsBrowser->setHtml(parseRssNews(news, formatter));
}

void InfoTabs::showPackageInfo(const PackageDetailData& pkg, const PackageDetailData* pkgInstalled,
                               const PackageListData*const pkgAurDetails)
{
	// update
	ui->infoBrowser->setHtml(formatPackageInfo(pkg, pkgInstalled, pkgAurDetails));
	// and activate info tab
	if (ui->tabWidget->currentWidget() != ui->tabInfo)
		ui->tabWidget->setCurrentWidget(ui->tabInfo);
}

void InfoTabs::showUpdateReport(const QList<PackageDetailData>& pacmanInstalled,
                                const QList<PackageDetailData>& pacmanRepo,
                                const QList<PackageDetailData>& aurInstalled,
                                const QMap<QString, PackageListData>& aurRepo)
{
	const QLocale locale = QLocale::system();
	QString html;
	html += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">";
	html += "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head><body>";

	html += "<h2>" + strSystemStatusReport() + ":</h2>";
	html += strSystemStatusReportL1().arg(pacmanInstalled.size()+aurInstalled.size()) + "<br><br>"
	        + strSystemStatusReportL2().arg(pacmanInstalled.size());
	// StartOf: Info-table
	html += ":<table border=\"0\" style=\"margin-left:0px; margin-top:3px;\" cellspacing=\"2\" cellpadding=\"0\">";
	if (pacmanInstalled.size()) {
		html += formatPackageInfoRow("<b>"+strName()+"</b>", "<b>"+strInstalled()+"</b>", "<b>"+strRepo()+"</b>",
		                             strRepository(), strCapitalDownloadSize(), strCapitalUpgradeSize());
		for (int x = 0; x < pacmanInstalled.size() && x < pacmanRepo.size(); ++x) {
			const PackageDetailData& pkgI = pacmanInstalled.at(x);
			const PackageDetailData& pkgR = pacmanRepo.at(x);
			const QString dls(locale.toString(pkgR.downloadSize) + " " + strKiB());
			const int iUpgrageSize = pkgR.installedSize - pkgI.installedSize;
			const QString upgradeSize((iUpgrageSize > 0 ? "+" : "") + locale.toString(iUpgrageSize) + " " + strKiB());
			html += formatPackageInfoRow(pkgI.name, pkgI.version, pkgR.version, pkgR.repository, dls, upgradeSize);
		}
	}
	html += "</table>";
	html += "<br><br>" + strSystemStatusReportL3().arg(aurInstalled.size());
	// StartOf: Info-table
	html += ":<table border=\"0\" style=\"margin-left:0px; margin-top:3px;\" cellspacing=\"2\" cellpadding=\"0\">";
	if (aurInstalled.size()) {
		html += formatPackageInfoRow("<b>"+strName()+"</b>", "<b>"+strInstalled()+"</b>", "<b>"+strRepo()+"</b>",
		                             strRepository(), "", "");
		for (int x = 0; x < aurInstalled.size(); ++x) {
			const PackageDetailData& pkgI = aurInstalled.at(x);
			const auto it = aurRepo.find(pkgI.name);
			if (it == aurRepo.end())
				continue;
			const PackageListData&   pkgR = *it;
			html += formatPackageInfoRow(pkgI.name, pkgI.version, pkgR.version, strForeignRepository(), "", "");
		}
	}
	html += "</table>";
	html += "</body></html>";
	ui->infoBrowser->setHtml(html);
	// and activate info tab
	if (ui->tabWidget->currentWidget() != ui->tabInfo)
		ui->tabWidget->setCurrentWidget(ui->tabInfo);
}

/*
 * Parses the raw XML contents from the Distro RSS news feed
 * Creates and returns a string containing a HTML code with latest 10
 *
 * based on Octopi
 */
QString InfoTabs::parseRssNews(const QString& news, const DistributionInfo& formatter)
{
  QString html("<style type=\"text/css\">table { margin-bottom: 30px; } big { font-size : large; }</style>");

	QString lastBuildDate;
	QDomDocument doc("rss");
	int itemCounter=0;

	if (news.isEmpty() || !doc.setContent(news)) {
		return "<p style=\"color:red;\">" + strErrorCanNotLoadNews() + "</p>";
	}

	QDomElement docElem = doc.documentElement();
	QDomNode n = docElem.firstChild().firstChild();

	while (!n.isNull())
	{
		QDomElement elem = n.toElement();

		if(!elem.isNull())
		{
			if (elem.tagName() == "lastBuildDate")
			{
				lastBuildDate = elem.text();
			}
			else if(elem.tagName() == "item")
			{
				//Let's iterate over the 10 lastest "item" news
				if (itemCounter == 10) break;

				QDomNode text = elem.firstChild();

				QString itemTitle;
				QString itemLink;
				QString itemDescription;
				QString itemPubDate;

				while(!text.isNull())
				{
					QDomElement eText = text.toElement();

					if(!eText.isNull())
					{
						if (eText.tagName() == "title")
						{
							itemTitle = eText.text();
						}
						else if (eText.tagName() == "link")
						{
							itemLink = eText.text();
						}
						else if (eText.tagName() == "content:encoded")
						{
							itemDescription = eText.text();
						}
						else if (eText.tagName() == "description" && itemDescription.isEmpty())
						{
							itemDescription = eText.text();
						}
						else if (eText.tagName() == "pubDate")
						{
							itemPubDate = eText.text();
							itemPubDate = itemPubDate.remove(QRegExp("\\n"));
							int pos = itemPubDate.indexOf("+");
							if (pos > -1)
							{
								itemPubDate = itemPubDate.mid(0, pos-1).trimmed();
							}
						}
					}
					text = text.nextSibling();
				}

				html += formatter.formatNews(itemLink, itemTitle, itemPubDate, itemDescription);
				itemCounter++;
			}
		}

		n = n.nextSibling();
	}

	return html;
}

QString InfoTabs::formatPackageInfo(const PackageDetailData& pkg, const PackageDetailData*const pkgInstalled,
                                    const PackageListData*const pkgAurDetails)
{
	const QLocale locale = QLocale::system();
	QString html;
	html += "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">";
	html += "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head><body>";

	// Name, version, build
	html += "<p><span style=\"margin-bottom:5px; font-size:x-large;\"><b>" + pkg.name
	       + "</b></span> (" + strLowercaseVersion() + " " + (pkgAurDetails ? (pkgAurDetails->version+" "+pkgAurDetails->outatedVersion) : pkg.version)
	       + "; " + strBuildFrom() + " " + locale.toString(pkg.buildDate) + ")";
	html += "</p>";
	// Description
	html += "<p><span style=\"font-size:16px;\">" + pkg.description + "</span></p>";
	// StartOf: Info-table
	html += "<table border=\"0\" style=\"margin-left:0px; margin-top:16px;\" cellspacing=\"2\" cellpadding=\"0\">";
	// URL
	html += formatPackageInfoRow(strUrl(), "<a href=\"" + sanitize(pkg.url) + "\">" + sanitize(pkg.url) + "</a>");
	// Installed version
	if (pkgInstalled != nullptr) {
		html += formatPackageInfoRow(strInstalledVersion(), pkgInstalled->version);
	}
	else if (pkgAurDetails != nullptr) {
		html += formatPackageInfoRow(strInstalledVersion(), pkg.version);
	}

	// License
	QString license = pkg.license;
	if (pkgInstalled != nullptr && license != pkgInstalled->license) {
		license += " <b>(";
		license += strItsNewStuff();
		license += ")</b>"; //TODO: it may be cool to see the actual difference formatted here
	}
	html += formatPackageInfoRow(strLicenses(), license);
	// Conflicts
	html += formatPackageInfoRow(strConflictsWith(), pkg.conflictsWith);
	html += formatPackageInfoRow(strReplaces(), pkg.replaces);
	// Groups
	html += formatPackageInfoRow(strGroups(), pkg.group);
	html += formatPackageInfoRow(strProvides(), pkg.provides);
	html += formatPackageInfoRow(strDependsOn(), pkg.dependsOn);
	html += formatPackageInfoRow(strOptDepends(), pkg.optDepends);
	// Required by
	QString requiredBy(pkg.requiredBy), optionalFor(pkg.optionalFor);
	if (requiredBy.isEmpty() && pkgInstalled != nullptr)
		requiredBy = pkgInstalled->requiredBy;
	if (requiredBy.isEmpty() == false) {
		html += formatPackageInfoRow(strRequiredBy(), requiredBy);
	}
	// Optional for
	if (optionalFor.isEmpty() && pkgInstalled != nullptr)
		optionalFor = pkgInstalled->optionalFor;
	if (optionalFor.isEmpty() == false) {
		html += formatPackageInfoRow(strOptFor(), optionalFor);
	}
	// Sizes
	if (pkgAurDetails == nullptr) {
		html += formatPackageInfoRow(strDownloadSize(), locale.toString(pkg.downloadSize) + " " + strKiB());
	}
	html += formatPackageInfoRow(strInstallSize(), locale.toString(pkg.installedSize) + " " + strKiB());
	if (pkgInstalled != nullptr && pkg.version != pkgInstalled->version) {
		const int iUpgrageSize = pkg.installedSize - pkgInstalled->installedSize;
		const QString upgradeSize((iUpgrageSize > 0 ? "+" : "") + locale.toString(iUpgrageSize) + " " + strKiB());
		html += formatPackageInfoRow(strUpDowngrade(), upgradeSize);
	}
	// Arch
	html += formatPackageInfoRow(strArchitecture(), pkg.arch);
	html += formatPackageInfoRow(strPackager(), sanitize(pkg.packager));
	html += "</table>";
	html += "</body></html>";
	return html;
}

// Helper func will create a styled <tr> containing td1 and 2
QString InfoTabs::formatPackageInfoRow(const QString& td1, const QString& td2, const QString& td3, const QString& td4,
                                       const QString& td5, const QString& td6)
{
	QString html;
	html += "<tr>";
	html += "<td>";
	html += "<p style=\"margin-right:20px;\">" + td1 + "</p></td>";
	html += "<td><p style=\"margin-right:10px;\">" + td2 + "</p></td>";
	html += "<td><p style=\"margin-right:20px;\">" + td3 + "</p></td>";
	html += "<td><p style=\"margin-right:10px;\">" + td4 + "</p></td>";
	html += "<td><p style=\"margin-right:10px;\">" + td5 + "</p></td>";
	html += "<td>"+ td6 + "</td></tr>";
	return html;
}

// Helper func will create a styled <tr> containing td1 and 2
QString InfoTabs::formatPackageInfoRow(const QString& td1, const QString td2)
{
	QString html;
	html += "<tr>";
	html += "<td>";
	html += "<p style=\"margin-right:20px;\">" + td1 + "</p></td>";
	html += "<td>";
	html += "<p>" + td2 + "</p></td></tr>";
	return html;
}

QString InfoTabs::sanitize(const QString& input)
{
	QString output = input;
	output = output.replace("<", "&lt;");
  output = output.replace(">", "&gt;");
	return output;
}
