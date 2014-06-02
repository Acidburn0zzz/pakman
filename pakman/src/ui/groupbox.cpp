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

#include "src/ui/groupbox.h"
#include "ui_groupbox.h"

#include <QTableWidgetItem>
#include "src/strconstants.h"
#include "src/icons.h"


GroupBox::GroupBox(QWidget *parent)
	: QWidget(parent), ui(new Ui::GroupBox), m_repo(nullptr)
{
	ui->setupUi(this);
}

GroupBox::~GroupBox()
{
	delete ui;
}

void GroupBox::initialize(PackageRepository& repo)
{
	// Group Filter Widget
	m_repo = &repo;
	repo.registerDependency(*this);
	connect(this, SIGNAL(updateViewSignal(bool)), this, SLOT(updateView(bool)), Qt::QueuedConnection);
	connect(ui->twGroups->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
	        this, SLOT(groupSelectionChanged(QItemSelection,QItemSelection)), Qt::AutoConnection);

	// Repo Filter Widget
	ui->twRepos->setColumnWidth(0, 26);
	connect(ui->twRepos, SIGNAL(cellClicked(int, int)),
	        this, SLOT(reposCellClicked(int, int)));
	connect(ui->twRepos, SIGNAL(cellDoubleClicked(int, int)),
	        this, SLOT(reposCellDoubleClicked(int, int)));

	// Filter Widget
	ui->twFilter->setColumnWidth(0, 26);

	m_installedFilter = addSettingsGroup(*ui->twFilter, iconSettingInstallation(), strFilterInstallation(),
	                                     {strNotInstalled(), strExplicitlyInstalled(), strImplicitlyInstalled()},
	                                     Qt::Checked);
	m_outdatedFilter  = addSettingsGroup(*ui->twFilter, iconSettingObsolescence(), strFilterObsolescence(),
	                                     {strInSync(), strOutdated(), strNewer()},
	                                     Qt::Checked);
	m_necessaryFilter = addSettingsGroup(*ui->twFilter, iconSettingNeeded(), strFilterNeeded(),
	                                     {strNecessary(), strNotNecessary()},
	                                     Qt::Checked);

	connect(ui->twFilter, SIGNAL(cellClicked(int, int)),
	        this, SLOT(filterCellClicked(int, int)));
	connect(ui->twFilter, SIGNAL(cellDoubleClicked(int, int)),
	        this, SLOT(filterCellDoubleClicked(int, int)));
}

void GroupBox::beforeshutdown(PackageRepository& repo)
{
	repo.deregisterDependency(*this);
}

void GroupBox::updateView(bool invalidate)
{
	invalidateView();
	if (invalidate == false) // rebuild
	{
		// Group Filter Widget
		QString pacmanString(strPacmanGroup()); // per default pacman is the root node for package groups
		GroupItem*const pacman = new GroupItem(pacmanString, pacmanString);
		pacman->setToolTip(0, strPacmanGroupToolTip());
		ui->twGroups->addTopLevelItem(pacman);

		GroupItem* lastGroupItem = nullptr;
		auto listOfGroups = m_repo->getGroupList();
		for (auto it = listOfGroups.cbegin(); it != listOfGroups.cend(); ++it) {
			lastGroupItem = updateView_tryAddGroup(lastGroupItem, (*it)->getName());
			if (lastGroupItem == nullptr) {
				lastGroupItem = new GroupItem((*it)->getName(), (*it)->getName());
				pacman->addChild(lastGroupItem);
			}
		}
		ui->twGroups->expandItem(pacman);

		// Repo Filter Widget
		auto repoList = m_repo->getRepos().toList().toStdList();
		repoList.sort();
		m_repoFilter = addSettingsGroup(*ui->twRepos, iconRepository(), strRepositories(),
		                                repoList, Qt::Unchecked);

		reposCellClicked(1, 0); // sync filter
	}
}

void GroupBox::groupSelectionChanged(const QItemSelection&, const QItemSelection&)
{
	auto list = ui->twGroups->selectedItems();
	if (list.isEmpty() == false) {
		GroupItem* item = static_cast<GroupItem*>(list.first());
		if (item->parent()) {
			emit groupSelected(item->getName());
			m_filter.applyGroupFilter(item->getName());
			emit filterUpdate(&m_filter);
			return;
		}
	}

	// no group or artificial group (like pacman) selected -> no filter
	m_filter.applyGroupFilter(QString());
	emit filterUpdate(&m_filter);
}

void GroupBox::reposCellClicked(int row, int column)
{
	if (column != 0 || m_repoFilter == nullptr || m_repoFilter->isIndexIncluded(row) == false)
		return; // only check-states are relevant

	QSet<QString> repos;
	m_repoFilter->forEachItem([&](QTableWidget& widget, int row){
		QTableWidgetItem*const item = widget.item(row, 0);
		assert(item != nullptr);
		const Qt::CheckState check = item->checkState();
		if (check == Qt::Checked)
			repos << widget.item(row, 1)->text();
	});

	if (repos.size() == m_repoFilter->getItemCount()) {
		// all repos included
		if (m_filter.applyFilter(QSet<QString>()) == false) return;
	}
	else {
		// construct filter
		if (m_filter.applyFilter(std::move(repos)) == false) return;
	}

	emit filterUpdate(&m_filter);
}

void GroupBox::reposCellDoubleClicked(int row, int)
{
	if (m_repoFilter && row == m_repoFilter->getHeaderIndex()) {
		if (m_repoFilter->isFirstItemHidden())
			m_repoFilter->show();
		else m_repoFilter->hide();
	}
}

void GroupBox::filterCellClicked(int, int column)
{
	if (column != 0)
		return; // only check-states are relevant

	bool updateFilter    = false;
	bool showImplicitlyInstalled = true;
	bool showExplicitlyInstalled = true;
	bool showRequired    = true;
	bool showNotRequired = true;
	DefaultPackageFilter::TStatusFilter filterSettings;
	filterSettings.fill(true);
	// now build the filter settings
	if (m_installedFilter != nullptr) {
		const int startIndex = m_installedFilter->getHeaderIndex();
		m_installedFilter->forEachItem([&](QTableWidget& widget, int row){
			QTableWidgetItem*const item = widget.item(row, 0);
			assert(item != nullptr);
			const Qt::CheckState check = item->checkState();
			if (check != Qt::Checked) {
				switch (row - startIndex) {
				case 1:
					filterSettings[epkg_NON_INSTALLED] = false;
					break;
				case 2:
					showExplicitlyInstalled = false;
					break;
				case 3:
					showImplicitlyInstalled = false;
					break;
				default:
					throw std::out_of_range("invalid install filter index");
				}
			}
		});
		updateFilter = true;
	}
	if (m_outdatedFilter != nullptr) {
		const int startIndex = m_outdatedFilter->getHeaderIndex();
		m_outdatedFilter->forEachItem([&](QTableWidget& widget, int row){
			QTableWidgetItem*const item = widget.item(row, 0);
			assert(item != nullptr);
			const Qt::CheckState check = item->checkState();
			if (check != Qt::Checked) {
				switch (row - startIndex) {
				case 1:
					filterSettings[epkg_INSTALLED] = false;
					filterSettings[epkg_FOREIGN] = false;
					break;
				case 2:
					filterSettings[epkg_OUTDATED] = false;
					filterSettings[epkg_FOREIGN_OUTDATED] = false;
					break;
				case 3:
					filterSettings[epkg_NEWER] = false;
					break;
				default:
					throw std::out_of_range("invalid obsolescence filter index");
				}
			}
		});
		updateFilter = true;
	}
	if (m_necessaryFilter != nullptr) {
		const int startIndex = m_necessaryFilter->getHeaderIndex();
		m_necessaryFilter->forEachItem([&](QTableWidget& widget, int row){
			QTableWidgetItem*const item = widget.item(row, 0);
			assert(item != nullptr);
			const Qt::CheckState check = item->checkState();
			if (check != Qt::Checked) {
				switch (row - startIndex) {
				case 1:
					showRequired = false;
					break;
				case 2:
					showNotRequired = false;
					break;
				default:
					throw std::out_of_range("invalid needed filter index");
				}
			}
		});
		updateFilter = true;
	}

	if (updateFilter) {
		m_filter.applyFilter(showExplicitlyInstalled, showImplicitlyInstalled,
		                     showRequired, showNotRequired, filterSettings);
		emit filterUpdate(&m_filter);
	}
}

void GroupBox::filterCellDoubleClicked(int row, int)
{
	if (m_installedFilter && row == m_installedFilter->getHeaderIndex()) {
		if (m_installedFilter->isFirstItemHidden())
			m_installedFilter->show();
		else m_installedFilter->hide();
	}
	else if (m_outdatedFilter && row == m_outdatedFilter->getHeaderIndex()) {
		if (m_outdatedFilter->isFirstItemHidden())
			m_outdatedFilter->show();
		else m_outdatedFilter->hide();
	}
	else if (m_necessaryFilter && row == m_necessaryFilter->getHeaderIndex()) {
		if (m_necessaryFilter->isFirstItemHidden())
			m_necessaryFilter->show();
		else m_necessaryFilter->hide();
	}
}

GroupBox::GroupItem* GroupBox::updateView_tryAddGroup(GroupItem*const lastItem, const QString& group)
{
	if (lastItem == nullptr) return nullptr;

	QString lastGroup = lastItem->getName();
	if (group.startsWith(lastGroup)) {
		QString displayName("[...]" + group.right(group.size() - lastGroup.size()));
		GroupItem*const groupItem = new GroupItem(group, displayName);
		lastItem->addChild(groupItem);
		return groupItem;
	}
	else {
		if (lastItem->parent())
			return updateView_tryAddGroup(static_cast<GroupItem*>(lastItem->parent()), group);
		else return nullptr;
	}
}

void GroupBox::invalidateView()
{
	ui->twGroups->clear();
	ui->twRepos->clear();
	ui->twRepos->setRowCount(0);
	m_repoFilter.reset();
}

std::unique_ptr<GroupBox::SettingsGroup> GroupBox::addSettingsGroup(QTableWidget& tw, QIcon icon, QString name,
                                                                    std::list<QString> items, Qt::CheckState initial)
{
	const int row = tw.rowCount();
	tw.setRowCount(row + items.size() + 1);
	QTableWidgetItem*const head = new QTableWidgetItem(name);
	head->setIcon(icon);
	head->setBackgroundColor(Qt::black);
	head->setForeground(Qt::white);
	QFont font(head->font());
	font.setBold(true);
	head->setFont(font);
	tw.setItem(row, 0, head);
	tw.setSpan(row, 0, 1, 2);

	int x = 1;
	for (const QString& item : items) {
		QTableWidgetItem*const first = new QTableWidgetItem(name);
		first->setCheckState(initial);
		tw.setItem(row + x, 0, first);
		tw.setItem(row + x, 1, new QTableWidgetItem(item));
		++x;
	}
	return std::unique_ptr<SettingsGroup>(new SettingsGroup(tw, row, items.size()));
}

void GroupBox::beginResetRepository(PackageRepository::EResetType type)
{
	switch (type) {
	default:
		emit updateViewSignal(true);
	case PackageRepository::eResetGroupMembers:
		//DO NOTHING
		return;
	}
}

void GroupBox::endResetRepository(PackageRepository::EResetType type)
{
	switch (type) {
	default:
		emit updateViewSignal(false);
	case PackageRepository::eResetGroupMembers:
		//DO NOTHING
		return;
	}
}
