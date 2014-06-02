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

#include "packagerepository.h"

#include <cassert>
#include <iostream>

#include <QSet>
#include <QMap>
#include <QStringList>

#include "src/strconstants.h"
#include "src/commands/pacman.h"


PackageRepository::PackageRepository()
{
}

PackageRepository::~PackageRepository()
{
	// delete items in groups list
	for (std::vector<Group*>::const_iterator it = m_listOfGroups.begin(); it != m_listOfGroups.end(); ++it) {
		if (*it != NULL) delete *it;
	}
	// delete items in list
	for (TListOfPackages::const_iterator it = m_listOfPackages.begin(); it != m_listOfPackages.end(); ++it) {
		if (*it != NULL) delete *it;
	}

}

void PackageRepository::registerDependency(PackageRepository::IDependency &depends)
{
	m_dependingModels.push_back(&depends);
}

void PackageRepository::deregisterDependency(PackageRepository::IDependency& depends)
{
	std::vector<IDependency*>::iterator it = std::find(m_dependingModels.begin(), m_dependingModels.end(), &depends);
	if (it != m_dependingModels.end())
		m_dependingModels.erase(it);
}

struct BeginResetModel {
	BeginResetModel(PackageRepository::EResetType type)
	  : m_type(type)
	{}

	inline void operator()(PackageRepository::IDependency* depends) {
		assert(depends != nullptr);
		depends->beginResetRepository(m_type);
	}
	const PackageRepository::EResetType m_type;
};

struct TSort {
	bool operator()(const PackageRepository::PackageData* a, const PackageRepository::PackageData* b) const {
		return a->name < b->name;
	}
};

struct EndResetModel {
	EndResetModel(PackageRepository::EResetType type)
	  : m_type(type)
	{}

	inline void operator()(PackageRepository::IDependency* depends) {
		assert(depends != nullptr);
		depends->endResetRepository(m_type);
	}
	const PackageRepository::EResetType m_type;
};

void PackageRepository::setData(const QList<PackageListData>*const listOfPackages,
                                const QSet<QString>& unrequiredPackages,
                                const QSet<QString>& explicitlyInstalledPackages)
{
//  std::cout << "received new package list" << std::endl;

	std::for_each(m_dependingModels.begin(), m_dependingModels.end(), BeginResetModel(eResetRepository));

	// delete items in groups list
	for (std::vector<Group*>::const_iterator it = m_listOfGroups.begin(); it != m_listOfGroups.end(); ++it) {
		if (*it != nullptr) (*it)->invalidateList();
	}
	// delete items in list
	for (TListOfPackages::const_iterator it = m_listOfPackages.begin(); it != m_listOfPackages.end(); ++it) {
		if (*it != nullptr) delete *it;
	}
	m_listOfPackages.clear();

	m_listOfPackages.reserve(listOfPackages->size());
	for (QList<PackageListData>::const_iterator it = listOfPackages->begin(); it != listOfPackages->end(); ++it) {
		PackageData*const data = new PackageData(*it, unrequiredPackages.contains(it->name) == false, false,
		                                         explicitlyInstalledPackages.contains(it->name) == true);
		m_listOfPackages.push_back(data);
		m_setOfRepos << data->repository;
	}

	qSort(m_listOfPackages.begin(), m_listOfPackages.end(), TSort());
	std::for_each(m_dependingModels.begin(), m_dependingModels.end(), EndResetModel(eResetRepository));
}

void PackageRepository::setAURData(/*inout*/QList<PackageListData>*const listOfForeignPackages,
                                   const QSet<QString>& unrequiredPackages,
                                   const QMap<QString, PackageListData>*const aurPackageData)
{
	//  std::cout << "received new foreign package list" << std::endl;

	std::for_each(m_dependingModels.begin(), m_dependingModels.end(), BeginResetModel(eResetRepository));

	// delete yaourt items in list
	for (TListOfPackages::iterator it = m_listOfPackages.begin(); it != m_listOfPackages.end(); ++it) {
		if (*it != NULL && (*it)->managedByYaourt) {
			delete *it;
			it = m_listOfPackages.erase(it);
		}
	}

	m_listOfPackages.reserve(m_listOfPackages.size() + listOfForeignPackages->size());
	for (QList<PackageListData>::iterator it = listOfForeignPackages->begin();
			 it != listOfForeignPackages->end(); ++it)
	{
		/// correct and enhance list data
		auto aurPkg = aurPackageData->find(it->name);
		if (aurPkg != aurPackageData->end()) {
			it->repository = strForeignRepository();
			if (it->version != aurPkg->version) {
				it->outatedVersion = it->version;
				it->version = aurPkg->version;
				it->status = epkg_FOREIGN_OUTDATED;
			}
		}
		// explicitly installed is always true for AUR packages
		//TODO: this is not true, AUR packages can be installed as dep, e.g. when being dropped to AUR later on
		PackageData*const pkg = new PackageData(*it, unrequiredPackages.contains(it->name) == false, true, true);
		m_listOfPackages.push_back(pkg);
		m_setOfRepos << pkg->repository;
	}

	qSort(m_listOfPackages.begin(), m_listOfPackages.end(), TSort());
	std::for_each(m_dependingModels.begin(), m_dependingModels.end(), EndResetModel(eResetRepository));
}

/**
 * @brief if the repository groups differ from %listOfGroups they will be reset
 * @param listOfGroups == group names
 */
void PackageRepository::checkAndSetGroups(const QStringList& listOfGroups)
{
	if (memberListOfGroupsEquals(listOfGroups) == false) {
		std::for_each(m_dependingModels.begin(), m_dependingModels.end(), BeginResetModel(eResetGroupList));
		for (std::vector<Group*>::const_iterator it = m_listOfGroups.begin(); it != m_listOfGroups.end(); ++it) {
			if (*it != NULL) delete *it;
		}
		m_listOfGroups.clear();

		for (QStringList::const_iterator it = listOfGroups.begin(); it != listOfGroups.end(); ++it) {
			m_listOfGroups.push_back(new Group(*it));
		}
		std::for_each(m_dependingModels.begin(), m_dependingModels.end(), EndResetModel(eResetGroupList));
	}
}

struct TComp {
	bool operator()(const PackageRepository::PackageData* a, const QString& b) const {
		return a->name < b;
	}
	bool operator()(const QString& b, const PackageRepository::PackageData* a) const {
		return b < a->name;
	}
};

/**
 * @brief checks the PackageRepository if the members of %groupName differ from %members and replaces with %members if necessary
 * @param groupName (name of the group)
 * @param members (should be output of Package::getPackagesOfGroup)
 */
void PackageRepository::checkAndSetMembersOfGroup(const QString& groupName, const QStringList& members)
{
	std::vector<Group*>::const_iterator groupIt = m_listOfGroups.begin();
	for (; groupIt != m_listOfGroups.end(); ++groupIt) {
		if (*groupIt != NULL && (*groupIt)->getName() == groupName) {
			break;
		}
	}
	if (groupIt != m_listOfGroups.end()) {
		Group& group = **groupIt;
	//    std::cout << "processing group " << group.getName().toStdString() << std::endl;
		if (group.memberListEquals(members) == false) {

			// invalidate and register all group members if lists are different
			std::for_each(m_dependingModels.begin(), m_dependingModels.end(), BeginResetModel(eResetGroupMembers));
			group.invalidateList();

			for (QStringList::const_iterator it = members.begin(); it != members.end(); ++it) {
				typedef TListOfPackages::const_iterator TIter;
				std::pair<TIter, TIter> packageIt =  std::equal_range(m_listOfPackages.begin(), m_listOfPackages.end(), *it, TComp());
				for (TIter iter = packageIt.first; iter != packageIt.second; ++iter) {
					if ((*iter)->managedByYaourt == false) {
						group.addPackage(**iter);
						break;
					}
				}
			}
			std::for_each(m_dependingModels.begin(), m_dependingModels.end(), EndResetModel(eResetGroupMembers));
		}
	}
	else {
		// app did not find package group %name
		std::cerr << strAppName() << " " << strErrorDnfPackageGroup().toStdString() << " " << groupName.toStdString() << std::endl;
		assert(false);
	}
}

const PackageRepository::TListOfPackages& PackageRepository::getPackageList() const
{
	return m_listOfPackages;
}

const PackageRepository::TListOfPackages& PackageRepository::getPackageList(const QString& group) const
{
//  std::cout << "get package list for group " << group.toStdString() << std::endl;

	if (!group.isEmpty()) {
		std::vector<Group*>::const_iterator groupIt = m_listOfGroups.begin();
		for (; groupIt != m_listOfGroups.end(); ++groupIt) {
			if (*groupIt != NULL && (*groupIt)->getName() == group) {
				break;
			}
		}
		if (groupIt != m_listOfGroups.end()) {
			Group& group = **groupIt;
			const TListOfPackages* list = group.getPackageList();
			if (list != NULL) return *list;
		}
	}

	// if no group found or not loaded yet. default to all packages
	return m_listOfPackages;
}

PackageRepository::PackageData* PackageRepository::getFirstPackageByName(const QString name) const
{
	for (TListOfPackages::const_iterator it = m_listOfPackages.begin(); it != m_listOfPackages.end(); ++it) {
		if ((*it)->name == name)
			return *it;
	}
	return NULL;
}

const std::vector<PackageRepository::Group*>& PackageRepository::getGroupList() const
{
	return m_listOfGroups;
}

const QSet<QString> PackageRepository::getRepos() const
{
	return m_setOfRepos;
}

std::size_t PackageRepository::countTotal() const
{
	return getPackageList().size();
}

std::size_t PackageRepository::countInstalled() const
{
	return std::count_if(m_listOfPackages.cbegin(), m_listOfPackages.cend(), [](const TListOfPackages::value_type& t){
		return t->installed();
	});
}

std::size_t PackageRepository::countOutdated() const
{
	return std::count_if(m_listOfPackages.cbegin(), m_listOfPackages.cend(), [](const TListOfPackages::value_type& t){
		return t->outdated();
	});
}

/**
 * @brief checks if the repository groups are up to date
 * @param listOfGroups == group-names
 * @return true if the groups in this repository are the same as the %listOfGroups
 */
bool PackageRepository::memberListOfGroupsEquals(const QStringList& listOfGroups)
{
	if (m_listOfGroups.size() != static_cast<std::size_t>(listOfGroups.size()))
		return false;

	QStringList::const_iterator it2 = listOfGroups.begin();
	for (std::vector<Group*>::const_iterator it = m_listOfGroups.begin(); it != m_listOfGroups.end(); ++it, ++it2) {
		if ((*it)->getName() != *it2)
			return false;
	}

	return true;
}

//////// PackageRepository::PackageData //////////////////////////////

/**
 * @brief conversion from pkg will default the repository to the foreign repo name
 */
PackageRepository::PackageData::PackageData(const PackageListData& pkg, const bool isRequired,
                                            const bool isManagedByYaourt,const bool wasExplicitlyInstalled)
	: required(isRequired), managedByYaourt(isManagedByYaourt),
	  explicitlyInstalled(wasExplicitlyInstalled), name(pkg.name),
	  repository(pkg.repository),
	  version(pkg.version), description(pkg.description), outdatedVersion(pkg.outatedVersion),
	  status(pkg.status != epkg_OUTDATED ?
	    pkg.status :
	      (Pacman::rpmvercmp(pkg.outatedVersion.toLatin1().data(), pkg.version.toLatin1().data()) == 1 ?
	        epkg_NEWER : epkg_OUTDATED))
{
}

//////// PackageRepository::Group //////////////////////////////
std::unique_ptr<PackageRepository::TListOfPackages>
		PackageRepository::Group::NO_PACKAGES(new PackageRepository::TListOfPackages());

PackageRepository::Group::Group(const QString& grpName)
	: name(grpName), m_listOfPackages(NO_PACKAGES.get())
{
}

const QString& PackageRepository::Group::getName()
{
	return name;
}

bool PackageRepository::Group::memberListEquals(const QStringList& packagelist)
{
	if (m_listOfPackages == NO_PACKAGES.get() || m_listOfPackages->size() != static_cast<std::size_t>(packagelist.size()))
		return false;

	QStringList::const_iterator it2 = packagelist.begin();
	for (TListOfPackages::const_iterator it = m_listOfPackages->begin(); it != m_listOfPackages->end(); ++it, ++it2) {
		if ((*it)->name != *it2)
			return false;
	}

	return true;
}

void PackageRepository::Group::addPackage(PackageRepository::PackageData& package)
{
	if (m_listOfPackages == NO_PACKAGES.get())
		m_listOfPackages = new TListOfPackages();

	m_listOfPackages->push_back(&package);
}

void PackageRepository::Group::invalidateList()
{
//  std::cout << "invalidate group " << name.toStdString() << std::endl;

	if (m_listOfPackages == NO_PACKAGES.get())
		return;

	delete m_listOfPackages;
	m_listOfPackages = NO_PACKAGES.get();
}

const PackageRepository::TListOfPackages* PackageRepository::Group::getPackageList() const
{
	return m_listOfPackages;
}
