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

#ifndef PACMANQT_ICONS_H
#define PACMANQT_ICONS_H

#include <QIcon>


////< General >////

// clear edit control
static inline QIcon iconClearRTL() { //TODO: find a replacement icon (fallback)
	return QIcon::fromTheme("edit-clear-locationbar-rtl", QIcon(":/pakman/resource/icons/help.png"));
}

////< PackageView >////

// not installed
static inline QIcon iconPkgNotInstalled() {
	return QIcon::fromTheme("", QIcon(":/pakman/resource/icons/notinstalled.png")); //TODO: find matching theme icon
}
// installed and up to date
static inline QIcon iconPkgInstalled() {
	return QIcon::fromTheme("security-high", QIcon(":/pakman/resource/icons/installed.png"));
}
// installed by user and up to date
static inline QIcon iconPkgExplicitlyInstalled() {
	return QIcon::fromTheme("security-high", QIcon(":/pakman/resource/icons/installed_user.png"));
}
// installed and up to date but not required
static inline QIcon iconPkgInstalledUnrequired() {
	return QIcon::fromTheme("security-high", QIcon(":/pakman/resource/icons/unrequired.png"));
}
// installed by user and up to date but not required
static inline QIcon iconPkgExplicitlyInstalledUnrequired() {
	return QIcon::fromTheme("security-high", QIcon(":/pakman/resource/icons/unrequired_user.png"));
}
// installed and outdated
static inline QIcon iconPkgInstalledOutdated() {
	return QIcon::fromTheme("security-low", QIcon(":/pakman/resource/icons/outdated.png"));
}
// installed by user and outdated
static inline QIcon iconPkgExplicitlyInstalledOutdated() {
	return QIcon::fromTheme("security-low", QIcon(":/pakman/resource/icons/outdated_user.png"));
}
// installed and newer than repo
static inline QIcon iconPkgInstalledNewer() {
	return QIcon::fromTheme("security-medium", QIcon(":/pakman/resource/icons/newer.png"));
}
// installed by user and newer than repo
static inline QIcon iconPkgExplicitlyInstalledNewer() {
	return QIcon::fromTheme("security-medium", QIcon(":/pakman/resource/icons/newer_user.png"));
}
// installed from AUR by user
static inline QIcon iconPkgInstalledAUR() {
	return QIcon::fromTheme("", QIcon(":/pakman/resource/icons/foreign_green.png"));
}
// installed from AUR by user and outdated
static inline QIcon iconPkgInstalledOutdatedAUR() {
	return QIcon::fromTheme("", QIcon(":/pakman/resource/icons/foreign_red.png"));
}

////< Filter Tabs >////

// installation filter setting
static inline QIcon iconRepository() { //TODO: find a replacement icon (fallback)
	return QIcon::fromTheme("repository", QIcon(":/pakman/resource/icons/help.png"));
}

// installation filter setting
static inline QIcon iconSettingInstallation() {
	return QIcon::fromTheme("security-high", QIcon(":/pakman/resource/icons/installed.png"));
}

// obsolescence filter setting
static inline QIcon iconSettingObsolescence() {
	return QIcon::fromTheme("security-medium", QIcon(":/pakman/resource/icons/newer.png"));
}

// needed filter setting
static inline QIcon iconSettingNeeded() {
	return QIcon::fromTheme("dialog-information", QIcon(":/pakman/resource/icons/help.png"));
}

#endif // ICONS_H
