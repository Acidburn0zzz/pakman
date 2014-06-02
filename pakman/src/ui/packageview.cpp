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

#include "src/ui/packageview.h"
#include "ui_packageview.h"

#include "src/data/model/packagemodel.h"
#include "src/data/packagerepository.h"


PackageView::PackageView(QWidget *parent)
	: QWidget(parent),
	  ui(new Ui::PackageView)
{
	ui->setupUi(this);
	ui->treeView->header()->setClickable(true);

	connect(ui->treeView->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this,
	        SLOT(sort(int,Qt::SortOrder)));
}

PackageView::~PackageView()
{
	delete ui;
}

void PackageView::initialize(PackageRepository& repo)
{
	// Create Model
	m_pkgViewModel.reset(new PackageModel(repo, nullptr));
	repo.registerDependency(*m_pkgViewModel);
	ui->treeView->setModel(m_pkgViewModel.get());

	connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
	        this, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), Qt::DirectConnection);

	// Resize columns
	ui->treeView->setColumnWidth(0, 24);
	ui->treeView->setColumnWidth(1, 400);
	ui->treeView->setColumnWidth(2, 160);
}

void PackageView::beforeshutdown(PackageRepository& repo)
{
	repo.deregisterDependency(*m_pkgViewModel);
}

void PackageView::setFilter(std::unique_ptr<IPackageFilter>&& filter)
{
	m_pkgViewModel->setNewFilter(std::move(filter));
}

int PackageView::getSelectedPackageCount() const
{
	assert(ui->treeView->selectionModel() != nullptr);
	return ui->treeView->selectionModel()->selectedRows().size();
}

const PackageRepository::PackageData* PackageView::getFirstSelectedPackage() const
{
	const QItemSelectionModel*const selectionModel = ui->treeView->selectionModel();
	if (selectionModel == nullptr)
		return nullptr;

	assert(m_pkgViewModel != nullptr);

	const QModelIndexList indexes = selectionModel->selectedRows(PackageModel::ctn_PACKAGE_NAME_COLUMN);
	if (indexes.isEmpty())
		return nullptr;

	const QModelIndex index = indexes.first();

	const PackageRepository::PackageData*const package = m_pkgViewModel->getData(index);
	if (package == nullptr) {
		assert(false);
		return nullptr;
	}
	return package;
}

const PackageRepository::PackageData* PackageView::getLastSelectedPackage() const
{
	const QItemSelectionModel*const selectionModel = ui->treeView->selectionModel();
	if (selectionModel == nullptr)
		return nullptr;

	assert(m_pkgViewModel != nullptr);

	const QModelIndexList indexes = selectionModel->selectedRows(PackageModel::ctn_PACKAGE_NAME_COLUMN);
	if (indexes.isEmpty())
		return nullptr;

	const QModelIndex index = indexes.last();

	const PackageRepository::PackageData*const package = m_pkgViewModel->getData(index);
	if (package == nullptr) {
		assert(false);
		return nullptr;
	}
	return package;
}

void PackageView::sort(int column, Qt::SortOrder order)
{
	// prevent infinite loop
	disconnect(ui->treeView->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this,
	           SLOT(sort(int,Qt::SortOrder)));
	// sort
	ui->treeView->sortByColumn(column, order);

	// reconnect
	connect(ui->treeView->header(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this,
	        SLOT(sort(int,Qt::SortOrder)));

	//TODO: saveSettings ?!
}
