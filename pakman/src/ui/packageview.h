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

#ifndef PACKAGEVIEW_H
#define PACKAGEVIEW_H

#include <memory>
#include <cassert>

#include <QWidget>
#include <QItemSelection>
#include <QPoint>
#include "src/data/model/packagemodel.h"


namespace Ui {
class PackageView;
}

class PackageView : public QWidget
{
	Q_OBJECT

public:
	explicit PackageView(QWidget *parent = 0);
	~PackageView();

	void initialize(PackageRepository& repo);
	void beforeshutdown(PackageRepository& repo);
	void setFilter(std::unique_ptr<IPackageFilter>&& filter);
	template<class Functor>
	inline void syncFilterChange(Functor fnc) {
		assert(m_pkgViewModel.get() != nullptr);
		m_pkgViewModel->syncFilterChange(fnc);
	}

	int getSelectedPackageCount() const;
	QString getSelectedPackageNames(const bool qualified);

	const PackageRepository::PackageData* getFirstSelectedPackage() const;
	const PackageRepository::PackageData* getLastSelectedPackage() const;

signals:
	void selectionChanged(const QItemSelection&, const QItemSelection&);
	/**
	 * @brief request for ContextMenu
	 * List must be deleted by receiver, only for one receiver and for direct connection !
	 */
	void requestContextMenu(QPoint, QList<const PackageRepository::PackageData*>*);

public slots:
	void sort(int column, Qt::SortOrder order);

private slots:
	void customContextMenuRequested(QPoint);

private:
	Ui::PackageView *ui;
	std::unique_ptr<PackageModel> m_pkgViewModel;
};

#endif // PACKAGEVIEW_H
