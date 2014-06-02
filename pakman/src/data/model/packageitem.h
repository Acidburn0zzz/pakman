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

#ifndef PACMANQT_PACKAGEITEM_H
#define PACMANQT_PACKAGEITEM_H

#include <memory>
#include <vector>

#include "src/data/packagerepository.h"

/**
 * @brief Holds package navigation information for a dependency treeview
 */
class PackageItem
{
public:
  typedef const PackageRepository::PackageData::TDependencyVec TPkgVec;

  /**
   * @brief Parent item %EDirection% child items (e.g parent DEPENDS_ON child)
   */
  enum EDirection {
    REQUIRED_BY,
    DEPENDS_ON
  };

public:
  /**
   * @brief Root item constructor
   * @param children (the package list)
   * @param mode (see EDirection)
   *
   * Make sure dependencies are available already before calling. Will
   * fetch 2 levels to make incremental fetch from QTreeview possible.
   */
  PackageItem(TPkgVec& children, EDirection mode);

  /**
   * @brief Child item constructor (will not fetch)
   * @param parent (the parent which requires or depends on this item)
   * @param package (the package to show in that line)
   * @param mode (see EDirection)
   */
  PackageItem(const PackageItem& parent, const PackageRepository::PackageData& package);

  /**
   * @brief will cascade clean up memory acquired. all child items will be invalid
   */
  ~PackageItem();

  /**
   * @brief checks if fetch function should be called. will test the first child only.
   * @param mode of operation (depends or required)
   * @return true if fetch should be called
   */
  bool canFetchChildren(EDirection mode) const;

  /**
   * @brief will fetch depencies for children according to mode
   * @param mode (depends or required)
   *
   * will create PackageItems for children if possible. make sure the dependency
   * information for all immediate children of this node is available before calling.
   * keep in mind that canFetchChildren() will only check the first child.
   */
  void fetchDepencies(EDirection mode);

  // Getter
public:
  inline const PackageItem& getParent() const {
    return m_parent;
  }

  inline const PackageRepository::PackageData& getPackage() const {
    return m_package;
  }

  inline int getChildCount() const {
    if (m_childVec.get() == NULL) return 0;
    return m_childVec->size();
  }

  inline const PackageItem* getChildAt(std::size_t row) const {
    if (m_childVec.get() == NULL) return NULL;
    return m_childVec->at(row);
  }

private:
  /**
   * @brief Will create child items according to mode and put them in m_childVec
   * @param children (in repository format, either dependsOn or requiredBy)
   * @param mode will only be used to fetch children of the newly created child-items
   */
  void fetchDepenciesFrom(TPkgVec& children);
  /**
   * @brief will cascade clean up memory acquired and set m_childVec to NULL.
   */
  void _deleteChildPackageItems();

private:
  const PackageItem&                         m_parent;   // may still be NULL but only for root items
  const PackageRepository::PackageData&      m_package;  // may still be NULL but only for root items
  std::auto_ptr<std::vector<PackageItem*> >  m_childVec; // all child items of that row according to EDirection setting
};

#endif // PACMANQT_PACKAGEITEM_H
