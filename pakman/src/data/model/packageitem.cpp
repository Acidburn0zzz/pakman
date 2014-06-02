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

#include "packageitem.h"

#include <iostream>
#include <cassert>

#include "src/strconstants.h"


/*
 * Will create root item and fetch 2 levels to make incremental fetch possible
 */
PackageItem::PackageItem(PackageItem::TPkgVec& children, EDirection mode)
 : m_parent(*static_cast<PackageItem*>(NULL)),
	 m_package(*static_cast<PackageRepository::PackageData*>(NULL))
{
	fetchDepenciesFrom(children);
	fetchDepencies(mode);
}

PackageItem::PackageItem(const PackageItem& parent, const PackageRepository::PackageData& package)
 : m_parent(parent), m_package(package)
{
}

PackageItem::~PackageItem()
{
	_deleteChildPackageItems();
}

bool PackageItem::canFetchChildren(EDirection mode) const
{
	if (m_childVec.get() != NULL && m_childVec->empty() == false) {
		PackageItem* NEXT = m_childVec->at(0);
		return (NEXT->m_childVec.get() == NULL
		        && (mode == DEPENDS_ON ? NEXT->m_package.getDependsOn() : NEXT->m_package.getRequiredBy()) != NULL);
	}
	//  includes (m_childVec.get() == NULL)
	return false;
}

void PackageItem::fetchDepencies(EDirection mode)
{
	if (m_childVec.get() != NULL) {
		for (std::size_t x = 0; x < m_childVec->size(); ++x) {
			const PackageRepository::PackageData& childPkg = m_childVec->at(x)->m_package;
			const TPkgVec* deps2 = (mode == DEPENDS_ON ? childPkg.getDependsOn() : childPkg.getRequiredBy());
			if (deps2 != NULL) {
				m_childVec->at(x)->fetchDepenciesFrom(*deps2);
			}
		}
	}
	else std::cerr << strAppName() << " " << strErrorDnfDependencyInfo(m_package.name).toStdString() << std::endl;
}

/**
 * @brief Will create child items according to mode and put them in m_childVec
 */
void PackageItem::fetchDepenciesFrom(PackageItem::TPkgVec& children)
{
	if (m_childVec.get() != NULL) {
		std::cerr << strAppName() << " " << strErrorDependencyInfoAlreadyFetched().toStdString() << std::endl;
		return; // since QTreeview doesnt support this we dont either
	}

	m_childVec.reset(new std::vector<PackageItem*>());
	m_childVec->reserve(children.size());

	for (std::size_t x = 0; x < children.size(); ++x) {
		m_childVec->push_back(new PackageItem(*this, *children[x]));
	}
}

/**
 * @brief will cascade clean up memory acquired and set m_childVec to NULL.
 */
void PackageItem::_deleteChildPackageItems()
{
	if (m_childVec.get() == NULL) return;

	for (std::size_t x = 0; x < m_childVec->size(); ++x) {
		PackageItem* pkg = (*m_childVec)[x];
		if (pkg != NULL) delete pkg;
	}
	m_childVec.reset();
}
