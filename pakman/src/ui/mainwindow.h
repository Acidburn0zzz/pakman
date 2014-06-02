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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <atomic>
#include <QMainWindow>
#include <QLineEdit>
#include <QItemSelection>
#include "src/ui/statusbar.h"
#include "src/commands/taskprocessor.h"
#include "src/data/packagerepository.h"
#include "src/data/model/packagemodel.h"


namespace Ui {
class MainWindow;
}
class DistributionInfo;


class MainWindow : public QMainWindow, private PackageRepository::IDependency
{
	Q_OBJECT

	////////////////////////
	class ProgressTracker {
	public:
		ProgressTracker()
			: value(0), max_value(0)
		{}

		inline void add(int valueIncrement, int maxIncrement) {
			incMax(maxIncrement);
			incValue(valueIncrement);
		}

		inline void incMax(int maxIncrement) {
			max_value += maxIncrement;
		}

		void incValue(int increment) {
			value += increment;

			// check for 100% completion
			const unsigned int temp = value.load();
			if (temp == max_value.load()) {
				value -= temp;
				max_value -= temp;
			}
		}

		std::pair<int, int> get() const {
			return std::make_pair(value.load(), max_value.load());
		}

	private:
		std::atomic<unsigned int> value;     // Just to make sure it will sum up in the end
		std::atomic<unsigned int> max_value; // Just to make sure it will sum up in the end
	} m_progressTracker;

	////////////////////////

public:
	explicit MainWindow(DistributionInfo& distribution, TaskProcessor& cpu,
	                    QWidget *parent = nullptr);
	~MainWindow();

	// will invalidate Repo-pointers !!!
	void triggerRepoRefresh();
	// will invalidate Repo-pointers !!!
	void updateGroupListAsync();
	// will invalidate Repo-pointers !!!
	void updatePackageListAsync();
	// will invalidate Repo-pointers !!!
	void updateForeignPackageListAsync();

	// will store the information in a temp location
	void fetchAurInformationAsync();

	void updateDistributionNewsAsync();
	// Update, and show help in info tab if %activate
	void updateHelp(bool activate);
	/**
	 * @brief update the InfoTab (bottom) with information about the package (will also switch tabs)
	 * @param packageName may be plain or (only for not-installed) in repo/name notation
	 *
	 * for installed packages additional information will be shown (version, upgrade size etc)
	 */
	void updatePackageInfoTabAsync(const PackageRepository::PackageData& package);

private slots:
	// PackageView selection changed
	void selectionChanged(const QItemSelection&, const QItemSelection&);
	void filterChanged(const DefaultPackageFilter* newFilter);
	void loadGroupMemberList(QString group); // loads only if not already available
	// Search LineEdit
	void searchEditChanged(const QString&);
	// Status Bar
	void updateReportRequested();
	// reload local repo
	void on_actionRefresh_View_triggered();
	// show / hide toolbar
	void on_actionShow_Toolbar_triggered(bool checked);
	// Search mode for LineEdit
	void onActionSearchByX_triggered(QAction* action);
	// Open a root terminal
	void on_actionRoot_Terminal_triggered();
	// Perform system upgrade and manage configs
	void on_actionSystem_Upgrade_triggered();
	// Show help in info tab
	void on_actionHelp_triggered();
	// Sync pacman repos in background
	void on_actionSync_Repo_triggered();
	// Show about dialog
	void on_actionAbout_triggered();
	// Show the dialog to find which package provides a certain file
	void on_actionWhatProvides_triggered();
	// Start gcala's log viewer for pacman
	void on_actionPacman_Log_Viewer_triggered();
	// Synchronize with AUR
	void on_actionAUR_triggered();

private:
	TaskProcessor&    m_cpu;
	PackageRepository m_pkgRepo;
	DistributionInfo& m_distribution;
	Ui::MainWindow*   ui;
	StatusBar*const   m_statusbar;   // WEAK
	QLineEdit*        m_lePkgSearch; // WEAK

private:
	void updateStatusNewTask(int maxIncrement);
	void updateStatusStartOfTask(QString activity);
	void updateStatusRunningTask(int progressIncrement);

	void applyFilterChange(std::function<void(DefaultPackageFilter&)> fnc);
	void applySearchFilter(DefaultPackageFilter& filter, const QString& searchStr);
	void applySearchFilterColumn(DefaultPackageFilter& filter, const int filterColumn);
	QString adaptSearchString(QString searchStr, bool exactMatch);

	// QWidget
	virtual void closeEvent(QCloseEvent* event) override;

	// IDependency interface
public:
	virtual void beginResetRepository(PackageRepository::EResetType) override {}
	virtual void endResetRepository(PackageRepository::EResetType) override;
};

#endif // MAINWINDOW_H
