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

#ifndef STRCONSTANTS_H
#define STRCONSTANTS_H

#include <QObject>
#include <QString>


/// Application (not translated)
const char* strAppName();
const char* strAppVersion();
const char* strAppHomepage();
QString     strForeignRepository();
QString     strPacmanGroup();
QString     strCacheDir();
QString     strDocumentationDir();
QString     strScriptsDir();
const char* strSystemUpdateScript();

/// Application (translated)
QString strAbout();
QString strAppDescription();
QString strLicense();
QString strTechnologyUsed();

/// PackageView header
QString strName();
QString strRepository();
QString strVersion();

/// Groups
QString strPacmanGroupToolTip();

/// Repos
QString strRepositories();

/// Filter
QString strFilterInstallation();
QString strNotInstalled();
QString strExplicitlyInstalled();
QString strImplicitlyInstalled();
QString strFilterObsolescence();
QString strInSync();
QString strOutdated();
QString strNewer();
QString strFilterNeeded();
QString strNecessary();
QString strNotNecessary();

/// Info-Tabs
QString strKiB();
QString strArchitecture();
QString strBuildFrom();
QString strConflictsWith();
QString strDependsOn();
QString strDownloadSize();
QString strGroups();
QString strInstallSize();
QString strInstalledVersion();
QString strItsNewStuff();
QString strLicenses();
QString strLowercaseVersion();
QString strOptDepends();
QString strOptFor();
QString strPackager();
QString strProvides();
QString strReplaces();
QString strRequiredBy();
QString strUpDowngrade();
QString strUrl();
QString strSystemStatusReport();
QString strSystemStatusReportL1();
QString strSystemStatusReportL2();
QString strSystemStatusReportL3();
QString strInstalled();
QString strRepo();
QString strCapitalDownloadSize();
QString strCapitalUpgradeSize();

/// Tasks
QString strTaskLoadingForeignPackages();
QString strTaskLoadingGroups();
QString strTaskLoadingNews();
QString strTaskLoadingPackages();
QString strTaskSystemUpgrade();
QString strTaskSynchronizeRepo();
QString strTaskUpdateAurInfo();
QString strTaskUpdateGroupMembers();
QString strTaskUpdatePackageInfo();
QString strTaskUpdateReport();

/// StatusBar
QString strPackage();
QString strPackages();
QString strPackageInstalled();
QString strPackageOutdated();
QString strPackageSelected();
QString strPackageTotal();
QString strStatusLoading();
QString strStatusReady();

/// Error messages (not translated unless stated otherwise)
QString strErrorAlreadyRunning();
QString strErrorCanNotLoadNews(); // translated
QString strErrorCanNotShutDown(); // translated
QString strErrorDoNotRunAsRoot();
QString strErrorDependencyInfoAlreadyFetched();
QString strErrorDnfAUR404();
QString strErrorDnfDistributionNews404();
QString strErrorDnfDependencyInfo(const QString& packageName);
QString strErrorDnfInstallationOrDataFile();
QString strErrorDnfPackageGroup();

/// Confirmation Dialog
QString strDlgRunningTransactions();
QString strDlgQQuitAfterCompletion();

#endif // STRCONSTANTS_H
