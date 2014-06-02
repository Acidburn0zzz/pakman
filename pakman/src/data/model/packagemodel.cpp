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

#include "packagemodel.h"

#include <cassert>
#include "src/strconstants.h"
#include "src/icons.h"


PackageModel::PackageModel(const PackageRepository& repo, QObject *parent)
: QAbstractItemModel(parent), m_packageRepo(repo), m_displayMode(FLAT),
  m_rootItem(createDummyRoot()),
  m_sortOrder(Qt::AscendingOrder), m_sortColumn(1),
  m_filter(new DefaultPackageFilter()),
  m_iconNotInstalled(iconPkgNotInstalled()), m_iconInstalled(iconPkgInstalled()),
  m_iconInstalledUnrequired(iconPkgInstalledUnrequired()),
  m_iconNewer(iconPkgInstalledNewer()), m_iconOutdated(iconPkgInstalledOutdated()),
  m_iconInstalledByUser(iconPkgExplicitlyInstalled()),
  m_iconInstalledUnrequiredByUser(iconPkgExplicitlyInstalledUnrequired()),
  m_iconNewerByUser(iconPkgExplicitlyInstalledNewer()),
  m_iconOutdatedByUser(iconPkgExplicitlyInstalledOutdated()),
  m_iconForeign(iconPkgInstalledAUR()), m_iconForeignOutdated(iconPkgInstalledOutdatedAUR())
{}

QModelIndex PackageModel::index(int row, int column, const QModelIndex &parent) const
{
	switch (m_displayMode) {
	case FLAT: {
		if (!hasIndex(row, column, parent))
			return QModelIndex();

		if (!parent.isValid()) {
			const int adaptedRow = transformRowIndex(row, m_columnSortedlistOfPackages.size());
			if (adaptedRow >= 0 && static_cast<size_t>(adaptedRow) < m_columnSortedlistOfPackages.size()) {
				return createIndex(row, column, (void*)m_columnSortedlistOfPackages.at(adaptedRow));
			}
		}
		return QModelIndex();
	}
	default:
		assert(false);
		return QModelIndex();
	}
}

QModelIndex PackageModel::parent(const QModelIndex& ) const
{
	switch (m_displayMode) {
	case FLAT:
		return QModelIndex();

	default:
		assert(false);
		return QModelIndex();
	}
}

int PackageModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid() == false) {
		return m_columnSortedlistOfPackages.size();
	}
	else return 0;
}

int PackageModel::columnCount(const QModelIndex&) const
{
	switch (m_displayMode) {
	case FLAT:
		return 4;
	default:
		return 0;
	}
}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid()) {
		switch (role) {
		case Qt::DisplayRole: {
			const PackageRepository::PackageData*const package = getData(index);
			if (package == NULL)
				return QVariant();

			switch (index.column()) {
			case ctn_PACKAGE_ICON_COLUMN:
				if (m_displayMode != FLAT) return QVariant(package->name);
				break;
			case ctn_PACKAGE_NAME_COLUMN:
				if (m_displayMode == FLAT) return QVariant(package->name);
				break;
			case ctn_PACKAGE_VERSION_COLUMN:
				return QVariant(package->version);
			case ctn_PACKAGE_REPOSITORY_COLUMN:
				return QVariant(package->repository);
			case ctn_PACKAGE_POPULARITY_COLUMN:
//			if (package->popularity >= 0)
//			  return QVariant(package->popularityString);
				break;
			default:
				assert(false);
			}
			break;
		}
		case Qt::DecorationRole: {
			const PackageRepository::PackageData*const package = getData(index);
			if (package == NULL)
				return QVariant();

			if (index.column() == ctn_PACKAGE_ICON_COLUMN) {
				return QVariant(getIconFor(*package));
			}
			break;
		}
		case Qt::ToolTipRole:
			break;
		case Qt::StatusTipRole:
			break;
		default:
			break;
		}
	}
	return QVariant();
}

QVariant PackageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
			case ctn_PACKAGE_ICON_COLUMN:
				if (m_displayMode == FLAT) return QVariant();
				break;
			case ctn_PACKAGE_NAME_COLUMN:
				if (m_displayMode == FLAT) return QVariant(strName());
				return QVariant();
			case ctn_PACKAGE_VERSION_COLUMN:
				return QVariant(strVersion());
			case ctn_PACKAGE_REPOSITORY_COLUMN:
				return QVariant(strRepository());
			default:
				break;
			}
		}
		return QVariant(section);
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

bool PackageModel::canFetchMore(const QModelIndex& parent) const
{
	if (parent.isValid() == false || parent.row() < 0)
	return false;

	switch (m_displayMode) {
	case FLAT:
	default:
		return false;
	}
}

void PackageModel::fetchMore(const QModelIndex& )
{
	switch (m_displayMode) {
	case FLAT:
	default:
		return;
	}
}

void PackageModel::sort(int column, Qt::SortOrder order)
{
//  std::cout << "sort column " << column << " in order " << order << std::endl;

	if (column != m_sortColumn || order != m_sortOrder) {
		if (m_displayMode == FLAT)
			emit layoutAboutToBeChanged();
		m_sortColumn = column;
		m_sortOrder  = order;
		sort();
		if (m_displayMode == FLAT)
			emit layoutChanged();
	}
}

void PackageModel::beginResetRepository(PackageRepository::EResetType)
{
	beginResetModel();
	m_listOfPackages.clear();
	m_columnSortedlistOfPackages.clear();
}

void PackageModel::endResetRepository(PackageRepository::EResetType)
{
	const PackageRepository::TListOfPackages& data = m_filter->getBasePackageList(m_packageRepo);
	m_listOfPackages.reserve(data.size());
	for (PackageRepository::TListOfPackages::const_iterator it = data.begin(); it != data.end(); ++it) {
		if (m_filter->mustFilterPackage(**it)) continue;
		m_listOfPackages.push_back(*it);
	}
	m_columnSortedlistOfPackages.reserve(data.size());
	m_columnSortedlistOfPackages = m_listOfPackages;
	sort();
	if (m_displayMode == FLAT)
		m_rootItem.reset(createDummyRoot());
	endResetModel();
}

int PackageModel::getPackageCount() const
{
	return m_listOfPackages.size();
}

const PackageRepository::PackageData* PackageModel::getData(const QModelIndex& index) const
{
	if (index.isValid() == false || index.internalPointer() == NULL)
		return NULL;

	switch (m_displayMode) {
	case FLAT: {
		return static_cast<const PackageRepository::PackageData*const>(index.internalPointer());
	}
	default:
		assert(false);
		return NULL;
	}
}

void PackageModel::switchDisplayMode(PackageModel::EDisplayMode newMode)
{
	beginResetRepository(PackageRepository::eResetRepository);
	m_displayMode = newMode;
	endResetRepository(PackageRepository::eResetRepository);
}

void PackageModel::setNewFilter(std::unique_ptr<IPackageFilter>&& filter)
{
	beginResetRepository(PackageRepository::eResetRepository);
	m_filter = std::move(filter);
	endResetRepository(PackageRepository::eResetRepository);
}

PackageItem& PackageModel::getPackageItem(const QModelIndex& index) const
{
	if (index.isValid()) {
		PackageItem*const branch = static_cast<PackageItem*const>(index.internalPointer());
		if (branch) return *branch;
	}
	return *m_rootItem;
}

const QIcon& PackageModel::getIconFor(const PackageRepository::PackageData& package) const
{
	switch (package.status)
	{
	case epkg_FOREIGN:
		return m_iconForeign;
	case epkg_FOREIGN_OUTDATED:
		return m_iconForeignOutdated;
	case epkg_OUTDATED:
		if (package.explicitlyInstalled) return m_iconOutdatedByUser;
		return m_iconOutdated;
	case epkg_NEWER:
		if (package.explicitlyInstalled) return m_iconNewerByUser;
		return m_iconNewer;
	case epkg_INSTALLED:
		// Does no other package depend on this package ? (unrequired package list)
		if (package.required)
		{
			if (package.explicitlyInstalled) return m_iconInstalledByUser;
			return m_iconInstalled;
		}
		else
		{
			if (package.explicitlyInstalled) return m_iconInstalledUnrequiredByUser;
			return m_iconInstalledUnrequired;
		}
		break;
	case epkg_NON_INSTALLED:
//      if (package.required == false) std::cout << "not installed not required" << std::endl; // doesn't happen with pacman
		return m_iconNotInstalled;
	default:
		assert(false);
	}
}


struct TSort0 {
	bool operator()(const PackageRepository::PackageData* a, const PackageRepository::PackageData* b) const {
		if (a->status < b->status) return true;
		if (a->status == b->status) {
			return a->name < b->name;
		}
		return false;
	}
};

struct TSort2 {
	bool operator()(const PackageRepository::PackageData* a, const PackageRepository::PackageData* b) const {
		const int cmp = Pacman::rpmvercmp(a->version.toLatin1().data(), b->version.toLatin1().data());
		if (cmp < 0) return true;
		if (cmp == 0) {
			return a->name < b->name;
		}
		return false;
	}
};

struct TSort3 {
	bool operator()(const PackageRepository::PackageData* a, const PackageRepository::PackageData* b) const {
		if (a->repository < b->repository) return true;
		if (a->repository == b->repository) {
			return a->name < b->name;
		}
		return false;
	}
};

struct TSort4 {
	bool operator()(const PackageRepository::PackageData* a, const PackageRepository::PackageData* b) const {
//	if (a->popularity > b->popularity) return true;
//	if (a->popularity == b->popularity) {
		return a->name < b->name;
//	}
//	return false;
	}
};

void PackageModel::sort()
{
	switch (m_sortColumn) {
	case ctn_PACKAGE_NAME_COLUMN:
		m_columnSortedlistOfPackages = m_listOfPackages;
		return;
	case ctn_PACKAGE_ICON_COLUMN:
		qSort(m_columnSortedlistOfPackages.begin(), m_columnSortedlistOfPackages.end(), TSort0());
		return;
	case ctn_PACKAGE_VERSION_COLUMN:
		qSort(m_columnSortedlistOfPackages.begin(), m_columnSortedlistOfPackages.end(), TSort2());
		return;
	case ctn_PACKAGE_REPOSITORY_COLUMN:
		qSort(m_columnSortedlistOfPackages.begin(), m_columnSortedlistOfPackages.end(), TSort3());
		return;
	case ctn_PACKAGE_POPULARITY_COLUMN:
		qSort(m_columnSortedlistOfPackages.begin(), m_columnSortedlistOfPackages.end(), TSort4());
		return;
	default:
		return;
	}
}

int PackageModel::transformRowIndex(int row, int rowCount) const
{
	switch (m_sortOrder) {
	case Qt::AscendingOrder:
		return row;
	case Qt::DescendingOrder:
		return rowCount - row - 1;
	}
	return -1;
}

PackageItem*PackageModel::createDummyRoot()
{
	return new PackageItem(PackageItem::TPkgVec(), PackageItem::DEPENDS_ON);
}
