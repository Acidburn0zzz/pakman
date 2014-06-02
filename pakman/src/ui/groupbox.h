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

#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <QWidget>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include "src/data/packagerepository.h"
#include "src/data/model/defaultpackagefilter.h"


namespace Ui {
class GroupBox;
}

class GroupBox : public QWidget, private PackageRepository::IDependency
{
	Q_OBJECT

	// TreeWidget Item class for groups
	class GroupItem : public QTreeWidgetItem {
	public:
		GroupItem(QString name, QString displayName)
			: QTreeWidgetItem(QStringList(displayName)), m_name(name)
		{}

		inline const QString& getName() const {
			return m_name;
		}

	private:
		QString m_name;
	};

	// For Filter and Repos Tabs
	class SettingsGroup {
	public:
		SettingsGroup(QTableWidget& widget, int head, int count)
		  : tw(widget), headerIndex(head), itemCount(count)
		{}

		/**
		 * Example: forEachItem([](QTableWidget& widget, int row){ widget.showRow(row); });
		 */
		template<class TFunc>
		void forEachItem(TFunc fnc) {
			for (int x = headerIndex + 1; x <= headerIndex + itemCount; ++x) {
				fnc(tw, x);
			}
		}

		inline int getHeaderIndex() const {
			return headerIndex;
		}

		inline int getItemCount() const {
			return itemCount;
		}

		inline bool isIndexIncluded(const int row) const {
			return (row > headerIndex && row <= headerIndex + itemCount);
		}

		inline bool isFirstItemHidden() const {
			return tw.isRowHidden(headerIndex + 1);
		}

		inline void show() {
			forEachItem([](QTableWidget& widget, int row){
				widget.showRow(row);
			});
		}

		inline void hide() {
			forEachItem([](QTableWidget& widget, int row){
				widget.hideRow(row);
			});
		}

	private:
		QTableWidget& tw;
		const int headerIndex;
		const int itemCount;
	};

public:
	explicit GroupBox(QWidget *parent = 0);
	~GroupBox();

	// will depend on repo till destruction
	void initialize(PackageRepository& repo);
	void beforeshutdown(PackageRepository& repo);

signals:
	void filterUpdate(const DefaultPackageFilter* newFilter);
	void updateViewSignal(bool invalidate);
	void groupSelected(QString groupName); // will not notify on all group or other special purpose groups / deselection

private slots:
	// invalidate true will clear only, false will rebuild
	void updateView(bool invalidate);
	void groupSelectionChanged(const QItemSelection&, const QItemSelection&);
	void reposCellClicked(int row, int column);
	void reposCellDoubleClicked(int row, int column);
	void filterCellClicked(int row, int column);
	void filterCellDoubleClicked(int row, int column);

private:
	/**
	 * @brief will try to add group as subitem to lastItem or one of its parents prefixed with [...]
	 * @param the supposed paret item (nullptr also possible)
	 * @param group (name)
	 * @return the future parent for the next round or nullptr if it must be added as first level item
	 */
	GroupItem* updateView_tryAddGroup(GroupItem*const lastItem, const QString& group);
	/**
	 * @brief invalidates Groups and Repos. Filter settings will remain
	 */
	void invalidateView();
	/**
	 * @brief will add settings (checkboxes + items) including a header row (icon + name) to a tablewidget tw
	 * @param tw    (add to this table widget)
	 * @param icon  (settings inluded will be grouped using this icon)
	 * @param name  (and this name)
	 * @param items (contains the settings descriptions/names)
	 * @param initial (initial state of all checkboxes created)
	 * @return object for easier maintenance and value access
	 */
	std::unique_ptr<SettingsGroup> addSettingsGroup(QTableWidget& tw, QIcon icon, QString name,
	                                                std::list<QString> items, Qt::CheckState initial);

private:
	Ui::GroupBox *ui;
	PackageRepository* m_repo; // valid after initialize

	std::unique_ptr<SettingsGroup> m_repoFilter;
	std::unique_ptr<SettingsGroup> m_installedFilter;
	std::unique_ptr<SettingsGroup> m_outdatedFilter;
	std::unique_ptr<SettingsGroup> m_necessaryFilter;
	DefaultPackageFilter m_filter;

	// IDependency interface
private:
	virtual void beginResetRepository(PackageRepository::EResetType) override;
	virtual void endResetRepository(PackageRepository::EResetType) override;
};

#endif // GROUPBOX_H
