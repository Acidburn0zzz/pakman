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

#ifndef PACMANQT_PACKAGEREPOSITORY_H
#define PACMANQT_PACKAGEREPOSITORY_H

#include <vector>
#include <memory>
#include <cassert>
#include <QSet>

#include "src/commands/pacman.h"


/**
 * @brief Central data storage for package data
 */
class PackageRepository
{
public:
	class PackageData;
	typedef std::vector<PackageData*> TListOfPackages;

public:
	enum EResetType {
		eResetRepository,  // All Package ptr have been reset (basically a full reset)
		eResetGroupList,   // The list of package groups has been reset (including members)
		eResetGroupMembers // The members of existing groups (packages) have changed
	};

	////////////////////////
	/**
	 * @brief The IDependency class used for notification of dependent models
	 */
	class IDependency {
	public:
		virtual void beginResetRepository(EResetType) = 0;
		virtual void endResetRepository(EResetType) = 0;
	};

	////////////////////////
	class PackageGuard;

	////////////////////////
	/**
	 * @brief Holds data of one package + a few convenience functions
	 */
	class PackageData {
	public:
		friend class PackageRepository::PackageGuard;
		typedef TListOfPackages TDependencyVec;

	public:
		/**
		 * @brief PackageData constructor
		 * @param package    = parsed data from pacman (e.g.)
		 * @param isRequired = false if package is not required by other packages installed, or true otherwise
		 */
		PackageData(const PackageListData& package, const bool isRequired, const bool isManagedByYaourt,
								const bool wasExplicitlyInstalled);

		inline bool installed() const {
			return status != epkg_NON_INSTALLED;
		}
		inline bool outdated() const {
			return status == epkg_OUTDATED || status == epkg_NEWER || status == epkg_FOREIGN_OUTDATED;
		}

		inline const TDependencyVec* getDependsOn() const {
			return dependsOn.get();
		}

		inline const TDependencyVec* getRequiredBy() const {
			return requiredBy.get();
		}

		private:
		inline void setDependsOn(const TDependencyVec* packages) {
	//      std::cout << "set dependencies for " << name.toStdString() << " to " << dependencies << std::endl;
			this->dependsOn.reset(packages);
		}
		inline void resetRequiredBy() {
			this->requiredBy.reset(new TDependencyVec());
		}
		inline void addRequiredBy(PackageData& pkg) {
			assert(this->requiredBy.get() != NULL);
			this->requiredBy->push_back(&pkg);
		}

		public:
		const bool    required;
		const bool    managedByYaourt; // yaourt packages must not be in any group
		const bool    explicitlyInstalled;
		const QString name;
		const QString repository;
		const QString version;
		const QString description;
		const QString outdatedVersion;
	//	const double  downloadSize;
		const PackageStatus status;
	//	const int     popularity; // -1 for non AUR
	//	const QString popularityString;

		private:
		std::auto_ptr<const TDependencyVec> dependsOn;
		std::auto_ptr<TDependencyVec>       requiredBy;
	};

	////////////////////////
	/**
	 * @brief The PackageGuard class enables access to specific private functions
	 */
	class PackageGuard {
		friend class PackageRepository;

		inline static void setDependencies(PackageData& pkg, const PackageData::TDependencyVec*const dependencies);
		inline static void resetRequirements(PackageData& pkg);
		inline static void addRequirement(PackageData& pkg, PackageData& dependsOnPkg);
	};

	////////////////////////
	/**
	 * @brief The Group class holds name and members of a package group
	 */
	class Group {
	public:
		Group(const QString& name);

		const QString& getName();
		bool memberListEquals(const QStringList& packagelist);
		void addPackage(PackageData& package);
		void invalidateList();

		const TListOfPackages* getPackageList() const;

	private:
		QString name;
		TListOfPackages* m_listOfPackages; // STRONG ptr list, WEAK ptr PackageData*
	private:
		static std::unique_ptr<TListOfPackages> NO_PACKAGES;
	};
	////////////////////////

public:
	PackageRepository();
	~PackageRepository();

	void registerDependency(IDependency& depends);
	void deregisterDependency(IDependency& depends);
	void setData(const QList<PackageListData>*const listOfPackages, const QSet<QString>& unrequiredPackages,
	             const QSet<QString>& explicitlyInstalledPackages);
	void setAURData(/*inout*/QList<PackageListData>*const listOfForeignPackages, const QSet<QString>& unrequiredPackages,
	                const QMap<QString, PackageListData>*const aurPackageData);
	void checkAndSetGroups(const QStringList& listOfGroups);
	void checkAndSetMembersOfGroup(const QString& group, const QStringList& members);

	const TListOfPackages& getPackageList() const;
	const TListOfPackages& getPackageList(const QString& group) const;
	PackageData*           getFirstPackageByName(const QString name) const;

	const std::vector<Group*>& getGroupList() const;
	const QSet<QString>    getRepos() const;

	std::size_t countTotal() const;
	std::size_t countInstalled() const;
	std::size_t countOutdated() const;

private:
	std::vector<IDependency*> m_dependingModels;
	QSet<QString>             m_setOfRepos;           // Set of all available Repositories
	TListOfPackages           m_listOfPackages;       // sorted qlist of all packages
	std::vector<Group*>       m_listOfGroups;         // sorted list of all pacman package groups
	bool memberListOfGroupsEquals(const QStringList& listOfGroups);
};


void PackageRepository::PackageGuard::setDependencies(PackageData& pkg, const PackageData::TDependencyVec*const dependencies) {
	pkg.setDependsOn(dependencies);
}
void PackageRepository::PackageGuard::resetRequirements(PackageData& pkg) {
	pkg.resetRequiredBy();
}
void PackageRepository::PackageGuard::addRequirement(PackageData& pkg, PackageData& dependsOnPkg) {
	pkg.addRequiredBy(dependsOnPkg);
}

#endif // PACMANQT_PACKAGEREPOSITORY_H
