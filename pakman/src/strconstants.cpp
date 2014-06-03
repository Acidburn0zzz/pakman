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

#include "strconstants.h"


/**
 * @brief used for identification of the app / in error messages
 */
const char* strAppName()
{
	return "pakman";
}

/**
 * @brief used for identification of the app / in error messages
 */
const char* strAppVersion()
{
	return "0.0.0.1";
}

/**
 * @brief used for homepage links e.g. in about box
 */
const char* strAppHomepage()
{
	return "https://github.com/tbinkau";
}

/**
 * @brief used for display and identification of the foreign repository (AUR)
 */
QString strForeignRepository()
{
	return "AUR";
}

/**
 * @brief used for the root node of all pacman package groups / to show all packages of pacman
 */
QString strPacmanGroup()
{
	return "pacman";
}

/**
 * @brief used for temporary files like rss news-feed
 */
QString strCacheDir()
{
	return QString(".cache/%1/").arg(strAppName());
}

/**
 * @brief used for static application data (e.g. the help)
 */
QString strDocumentationDir()
{
	return QString("/usr/share/doc/%1/").arg(strAppName());
}

/**
 * @brief used for application scripts (e.g. the system-update script)
 */
QString strScriptsDir()
{
	return QString("/usr/bin/");
}

/**
 * @brief for configurable script name of the system-update bash script
 */
const char* strSystemUpdateScript()
{
	return "system-update";
}

/**
 * @brief used for about box title
 */
QString strAbout()
{
	return QObject::tr("About");
}

/**
 * @brief used for about box
 */
QString strAppDescription()
{
	return QObject::tr("KDE pacman frontend");
}

/**
 * @brief used for about box
 */
QString strLicense()
{
	return QObject::tr("license");
}

/**
 * @brief used for about box
 */
QString strTechnologyUsed()
{
	return QObject::tr("Qt and KDE technology");
}

/**
 * @brief used for PackageView header
 */
QString strName()
{
	return QObject::tr("Name");
}

/**
 * @brief used for PackageView header
 */
QString strVersion()
{
	return QObject::tr("Version");
}

/**
 * @brief used for PackageView header
 */
QString strRepository()
{
	return QObject::tr("Repository");
}

/**
 * @brief used as ToolTip in the Groups tab for pacman
 */
QString strPacmanGroupToolTip()
{
	return QObject::tr("contains all pacman groups");
}

/**
 * @brief used for Repos filter tab
 */
QString strRepositories()
{
	return QObject::tr("Repositories");
}

/**
 * @brief used for Filter tab
 */
QString strFilterInstallation()
{
	return QObject::tr("Installation");
}

/**
 * @brief used for Filter tab
 */
QString strNotInstalled()
{
	return QObject::tr("not installed");
}

/**
 * @brief used for Filter tab
 */
QString strExplicitlyInstalled()
{
	return QObject::tr("explicitly installed");
}

/**
 * @brief used for Filter tab
 */
QString strImplicitlyInstalled()
{
	return QObject::tr("implicitly installed");
}

/**
 * @brief used for Filter tab
 */
QString strFilterObsolescence()
{
	return QObject::tr("Obsolescence");
}

/**
 * @brief used for Filter tab
 */
QString strInSync()
{
	return QObject::tr("in sync");
}

/**
 * @brief used for Filter tab
 */
QString strOutdated()
{
	return QObject::tr("outdated");
}

/**
 * @brief used for Filter tab
 */
QString strNewer()
{
	return QObject::tr("newer");
}

/**
 * @brief used for Filter tab
 */
QString strFilterNeeded()
{
	return QObject::tr("Needed");
}

/**
 * @brief used for Filter tab
 */
QString strNecessary()
{
	return QObject::tr("necessary");
}

/**
 * @brief used for Filter tab
 */
QString strNotNecessary()
{
	return QObject::tr("not necessary");
}

/**
 * @brief kilobyte (1024) :-)
 */
QString strKiB()
{
	return QObject::tr("KiB");
}

/**
 * @brief used side by side with package name
 */
QString strLowercaseVersion()
{
	return QObject::tr("version");
}

/**
 * @brief strBuildFrom %date
 */
QString strBuildFrom()
{
	return QObject::tr("build from");
}

/**
 * @brief header in info tabs
 */
QString strUrl()
{
	return QObject::tr("Website / URL");
}

/**
 * @brief header in info tabs
 */
QString strLicenses()
{
	return QObject::tr("Licenses");
}

/**
 * @brief header in info tabs
 */
QString strDependsOn()
{
	return QObject::tr("Depends On");
}

/**
 * @brief header in info tabs
 */
QString strDownloadSize()
{
	return QObject::tr("Download size");
}

/**
 * @brief header in info tabs
 */
QString strInstallSize()
{
	return QObject::tr("Installed size");
}

/**
 * @brief header in info tabs
 */
QString strPackager()
{
	return QObject::tr("Packager");
}

/**
 * @brief header in info tabs
 */
QString strArchitecture()
{
	return QObject::tr("Architecture");
}

/**
 * @brief header in info tabs
 */
QString strItsNewStuff()
{
	return QObject::tr("new");
}

/**
 * @brief header in info tabs
 */
QString strInstalledVersion()
{
	return QObject::tr("Installed version");
}

/**
 * @brief header in info tabs
 */
QString strConflictsWith()
{
	return QObject::tr("Conflicts with");
}

/**
 * @brief header in info tabs
 */
QString strReplaces()
{
	return QObject::tr("Replaces");
}

/**
 * @brief header in info tabs
 */
QString strGroups()
{
	return QObject::tr("Groups");
}

/**
 * @brief header in info tabs
 */
QString strProvides()
{
	return QObject::tr("Provides");
}

/**
 * @brief header in info tabs
 */
QString strOptDepends()
{
	return QObject::tr("Opt. depends");
}

/**
 * @brief header in info tabs
 */
QString strRequiredBy()
{
	return QObject::tr("Required by");
}

/**
 * @brief header in info tabs
 */
QString strOptFor()
{
	return QObject::tr("Opt. for");
}

/**
 * @brief header in info tabs
 */
QString strUpDowngrade()
{
	return QObject::tr("Up-/Downgrade");
}

/**
 * @brief Caption followed by :
 */
QString strSystemStatusReport()
{
	return QObject::tr("System status report");
}

/**
 * @brief summary line
 */
QString strSystemStatusReportL1()
{
	return QObject::tr("Your system reports %1 outdated packages.");
}

/**
 * @brief pacman package line
 */
QString strSystemStatusReportL2()
{
	return QObject::tr("%1 packages can be updated via system upgrade (Ctrl+U)");
}

/**
 * @brief AUR package line
 */
QString strSystemStatusReportL3()
{
	return QObject::tr("%1 packages must be updated manually (e.g. via makepkg or yaourt)");
}

/**
 * @brief column header in system status report
 */
QString strInstalled()
{
	return QObject::tr("Installed");
}

/**
 * @brief column header in system status report
 */
QString strRepo()
{
	return QObject::tr("Repo");
}

/**
 * @brief column header in system status report
 */
QString strCapitalDownloadSize()
{
	return QObject::tr("Download Size");
}

/**
 * @brief column header in system status report
 */
QString strCapitalUpgradeSize()
{
	return QObject::tr("Upgrade Size");
}

/**
 * @brief used for status bar
 */
QString strTaskLoadingForeignPackages()
{
	return QObject::tr("loading foreign packages");
}

/**
 * @brief used for status bar
 */
QString strTaskLoadingGroups()
{
	return QObject::tr("loading groups");
}

/**
 * @brief used for status bar
 * @return
 */
QString strTaskLoadingNews()
{
	return QObject::tr("loading distribution news");
}

/**
 * @brief used for status bar
 */
QString strTaskLoadingPackages()
{
	return QObject::tr("loading packages");
}

/**
 * @brief used for status bar when running system-update
 */
QString strTaskSystemUpgrade()
{
	return QObject::tr("running system upgrade");
}

/**
 * @brief used for status bar
 */
QString strTaskSynchronizeRepo()
{
	return QObject::tr("synchronizing repo");
}

/**
 * @brief used for status bar
 */
QString strTaskUpdateAurInfo()
{
	return QObject::tr("synchronizing foreign repo");
}

/**
 * @brief used for status bar
 */
QString strTaskUpdateGroupMembers()
{
	return QObject::tr("updating group");
}

/**
 * @brief used for status bar when fetching data for the info tab
 */
QString strTaskUpdatePackageInfo()
{
	return QObject::tr("updating package information");
}

/**
 * @brief used for status bar when processing data for the info tab
 */
QString strTaskUpdateReport()
{
	return QObject::tr("generating update report");
}

/**
 * @brief initial task shown in status bar
 */
QString strStatusLoading()
{
	return QObject::tr("loading");
}

/**
 * @brief shown in status bar after completing all tasks (as in i am "ready")
 */
QString strStatusReady()
{
	return QObject::tr("ready");
}

/**
 * @brief packages total shown as: %number package selected
 */
QString strPackage()
{
	return QObject::tr("package");
}

/**
 * @brief packages total shown as: %number packages selected
 */
QString strPackages()
{
	return QObject::tr("packages");
}

/**
 * @brief packages total shown as: %number total
 */
QString strPackageTotal()
{
	return QObject::tr("total");
}

/**
 * @brief packages installed shown as: %number installed
 */
QString strPackageInstalled()
{
	return QObject::tr("installed");
}

/**
 * @brief packages outdated shown as: %number outdated
 */
QString strPackageOutdated()
{
	return QObject::tr("outdated");
}

/**
 * @brief packages selected shown as: %number package(s) selected
 */
QString strPackageSelected()
{
	return QObject::tr("selected");
}

/**
 * @brief only one instance of the app may be run at one time
 */
QString strErrorAlreadyRunning()
{
	return "already running";
}

/**
 * @brief will be shown if the distibution news can not be shown (non available, processing error)
 */
QString strErrorCanNotLoadNews()
{
	return QObject::tr("Unable to load distribution news.");
}

/**
 * @brief will be used if tasks are still running when the user requested a shutdown
 */
QString strErrorCanNotShutDown()
{
	return QObject::tr("Can not shut down");
}

/**
 * @brief the app must not be run as root
 */
QString strErrorDoNotRunAsRoot()
{
	return QString("critical: ") + strAppName() + " was not designed to be run as root.";
}

/**
 * @brief error message "could not retrieve AUR information / all necessary information from AUR"
 */
QString strErrorDnfAUR404() {
	return "could not retrieve AUR information";
}

/**
 * @brief error message "could not retrieve distribution news"
 */
QString strErrorDnfDistributionNews404() {
	return "could not retrieve distribution news";
}

/**
 * @brief error message used when help file was not found
 */
QString strErrorDnfInstallationOrDataFile()
{
	return "was not installed correctly. please re-install.";
}

/**
 * @brief error message "did not find package group" %x
 */
QString strErrorDnfPackageGroup() {
	return "did not find package group";
}

/**
 * @brief error message (indicating programming error)
 */
QString strErrorDnfDependencyInfo(const QString& packageName) {
	return QString("package %1 is missing package dependencies. Should have been fetched already.").arg(packageName);
}

/**
 * @brief error message (indicating programming error)
 */
QString strErrorDependencyInfoAlreadyFetched()
{
	return "tried to fetch dependency information twice.";
}

/**
 * @brief used in quit-error-dialog if tasks are still running
 */
QString strDlgRunningTransactions()
{
	return QObject::tr("Some transactions are still running.");
}

/**
 * @brief used in quit-error-dialog if tasks are still running
 */
QString strDlgQQuitAfterCompletion()
{
	return QObject::tr("Do you want to quit after completion?");
}
