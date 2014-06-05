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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <functional>
#include <iostream>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include "src/ui/lineedit.h"
#include "src/ui/whatprovidesme.h"
#include "src/commands/pacman.h"
#include "src/strconstants.h"
#include "src/distribution/distributioninfo.h"
#include "src/commands/terminal.h"
#include "src/commands/pacmanlogviewer.h"


MainWindow::MainWindow(DistributionInfo& distribution, TaskProcessor& cpu,
                       QWidget *parent)
	: QMainWindow(parent), m_cpu(cpu), m_pkgRepo(), m_distribution(distribution),
	  ui(new Ui::MainWindow), m_statusbar(new StatusBar())
{
	ui->setupUi(this);
	setWindowTitle(QString(strAppName()) + " v." + strAppVersion());
	setStatusBar(m_statusbar);
	ui->actionShow_Toolbar->setChecked(true);

	// Data
	ui->packageView->initialize(m_pkgRepo);
	ui->groupBox->initialize(m_pkgRepo);
	m_pkgRepo.registerDependency(*this);

	// PackageView stage2 - connect signals (package selection)
	connect(ui->packageView, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
	        this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
	connect(ui->packageView, SIGNAL(requestContextMenu(QPoint, QList<const PackageRepository::PackageData*>*)),
	        this, SLOT(onRequestForContextMenu(QPoint, QList<const PackageRepository::PackageData*>*)),
	        Qt::DirectConnection);
	connect(ui->actionInstall_now, SIGNAL(triggered()), this, SLOT(actionInstallNow_triggered()));
	connect(ui->actionRemove_now, SIGNAL(triggered()), this, SLOT(actionRemoveNow_triggered()));
	// GroupBox stage2 - connect signals (group selection, deferred loading)
	connect(ui->groupBox, SIGNAL(filterUpdate(const DefaultPackageFilter*)),
	        this, SLOT(filterChanged(const DefaultPackageFilter*)));
	connect(ui->groupBox, SIGNAL(groupSelected(QString)),
	        this, SLOT(loadGroupMemberList(QString)));

	//TODO prototype: Searchbar
	QWidget* menuWidget = new QWidget();
	QHBoxLayout* layout = new QHBoxLayout(menuWidget);
	layout->setMargin(1);
	m_lePkgSearch = new LineEdit(menuWidget);
	layout->addWidget(ui->menuBar, 0, Qt::AlignVCenter);
	layout->addWidget(m_lePkgSearch);
	layout->addStretch(10);
	setMenuWidget(menuWidget);

	connect(m_lePkgSearch, SIGNAL(textChanged(const QString&)),
	        this, SLOT(searchEditChanged(const QString&)));

	// MenuBar
	QActionGroup* actionGroup = new QActionGroup(this); // WEAK
	actionGroup->addAction(ui->actionSearchByDescription);
	actionGroup->addAction(ui->actionSearchByName);
	actionGroup->setExclusive(true);

	connect(actionGroup, SIGNAL(triggered(QAction*)),
	        this, SLOT(onActionSearchByX_triggered(QAction*)));
	ui->actionSearchByName->trigger(); //TODO: make this configurable (default by description, or by name)

	// StatusBar
	connect(m_statusbar, SIGNAL(updateReportRequested()), this, SLOT(updateReportRequested()));

	// Load data
	triggerRepoRefresh();
	updateDistributionNewsAsync();
	updateHelp(false);

	// Optional commands
	if (PacmanLogViewer::isAvailable()) ui->actionPacman_Log_Viewer->setEnabled(true);
	if (m_distribution.shouldFetchAurInfo()) {
		fetchAurInformationAsync();
	}
}

MainWindow::~MainWindow()
{
	ui->packageView->beforeshutdown(m_pkgRepo);
	ui->groupBox->beforeshutdown(m_pkgRepo);
	m_pkgRepo.deregisterDependency(*this);
	delete ui;
}

/**
 * @brief confirmation before close if there is a pending transaction
 */
void MainWindow::closeEvent(QCloseEvent* event)
{
	// Check for pending / running transactions
	if (m_cpu.hasTasks()) {
		event->ignore();
		int res = QMessageBox::question(this, strErrorCanNotShutDown(),
		                                strDlgRunningTransactions() + "\n" +
		                                strDlgQQuitAfterCompletion(),
		                                QMessageBox::Yes | QMessageBox::No,
		                                QMessageBox::No);

		if (res != QMessageBox::Yes)
		{
			return;
		}
		m_cpu.schedule(TaskProcessor::PushBack, [this](){
					return [this](){
							this->close();
					};
			}, TaskProcessor::eTaskShutdown);
	}
	else
	{
		// Schedule shutdown
		QByteArray windowSize = saveGeometry();
		event->accept();
	}
}

void MainWindow::triggerRepoRefresh()
{
	updateGroupListAsync(); //TODO: this does actually kill the group selection
	updatePackageListAsync();
	updateForeignPackageListAsync();
}

void MainWindow::updateGroupListAsync()
{
	if (m_cpu.schedule(TaskProcessor::RemoveFirstOfTypePushBack, [this](){
			updateStatusStartOfTask(strTaskLoadingGroups());
			auto list = Pacman::getPackageGroups().release();
			return [this, list](){
					m_pkgRepo.checkAndSetGroups(*list);
					updateStatusRunningTask(10);
					delete list;
			};
	}, TaskProcessor::eTaskUpdateGroupList)) {
	//then
		updateStatusNewTask(10);
	}
}

void MainWindow::updatePackageListAsync()
{
	if (m_cpu.schedule(TaskProcessor::RemoveFirstOfTypePushBack, [this](){
			updateStatusStartOfTask(strTaskLoadingPackages());
			auto list = Pacman::getPackageList().release();
			auto unrequired = Pacman::getUnrequiredPackageList().release();
			auto explicits = Pacman::getExplicitPackageList().release();
			updateStatusRunningTask(90);
			return [this, list, unrequired, explicits](){
					m_pkgRepo.setData(list, *unrequired, *explicits);
					updateStatusRunningTask(10);
					delete list;
					delete unrequired;
					delete explicits;
			};
	}, TaskProcessor::eTaskUpdatePackageList)) {
	//then
		updateStatusNewTask(100);
	}
}

void MainWindow::updateForeignPackageListAsync()
{
	if (m_cpu.schedule(TaskProcessor::RemoveFirstOfTypePushBack, [this](){
			updateStatusStartOfTask(strTaskLoadingForeignPackages());
			auto list = Pacman::getPackageListForeign().release();
			auto unrequired = Pacman::getUnrequiredPackageList().release();
			auto aur = m_distribution.retrieveAurInfo().release();
			updateStatusRunningTask(40);
			return [this, list, unrequired, aur](){
					m_pkgRepo.setAURData(list, *unrequired, aur);
					updateStatusRunningTask(10);
					delete list;
					delete unrequired;
					delete aur;
			};
	}, TaskProcessor::eTaskUpdatePackageListForeign)) {
	//then
		updateStatusNewTask(50);
	}
}

void MainWindow::fetchAurInformationAsync()
{
	if (m_cpu.schedule(TaskProcessor::OnlyOne, [this](){
			updateStatusStartOfTask(strTaskUpdateAurInfo());
			auto list = Pacman::getPackageListForeign();
			m_distribution.fetchAurInfoFor(*list);
			updateStatusRunningTask(49);
			return [this](){
					updateForeignPackageListAsync();
					updateStatusRunningTask(1);
			};
	}, TaskProcessor::eTaskFetchPackageListForeign)) {
	//then
		updateStatusNewTask(50);
	}
}

void MainWindow::updateDistributionNewsAsync()
{
	if (m_cpu.schedule(TaskProcessor::RemoveFirstOfTypeOverwrite, [this](){
			updateStatusStartOfTask(strTaskLoadingNews());
			QString news;
			const bool supported = m_distribution.retrieveNews(news);
			updateStatusRunningTask(99);
			return [this, news, supported](){
					ui->infoTabs->showNews(news, m_distribution);
					updateStatusRunningTask(1);
			};
	}, TaskProcessor::eTaskUpdateDistributionNews)) {
	//then
		updateStatusNewTask(100);
	}
}

void MainWindow::updateHelp(bool activate)
{
	QFile file(strDocumentationDir() + strAppName() + ".html");
	if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(&file);
		in.setCodec("UTF-8");
		QString result = in.readAll();
		file.close();
		ui->infoTabs->showHelp(result, activate);
	}
	else std::cerr << strAppName() << " " << strErrorDnfInstallationOrDataFile().toStdString() << std::endl;
}

void MainWindow::updatePackageInfoTabAsync(const PackageRepository::PackageData& package)
{
	const bool installed = package.installed();
	QString packageName(package.name);
	QString packageRepo(package.repository);
	PackageListData *aurData = nullptr;
	if (package.managedByYaourt)
		aurData = new PackageListData(package.name, package.repository, package.version, "", package.status);

	if (m_cpu.schedule(TaskProcessor::RemoveFirstOfTypePushBack, [this, packageName, packageRepo, installed, aurData](){
			updateStatusStartOfTask(strTaskUpdatePackageInfo());
			QList<PackageDetailData>* list, * listInstalled;
			if (aurData) {
				list          = Pacman::getPackageDetails(packageName, true).release();
				listInstalled = nullptr;
			}
			else {
				list          = Pacman::getPackageDetails(packageRepo + "/" + packageName, false).release();
				listInstalled = installed ? Pacman::getPackageDetails(packageName, true).release() : nullptr;
			}
			updateStatusRunningTask(19);
			return [this, list, listInstalled, aurData](){
					if (list->empty() == false) {
						const PackageDetailData* pkgInstalled = nullptr;
						if (listInstalled != nullptr && listInstalled->isEmpty() == false)
							pkgInstalled = &listInstalled->at(0);
						ui->infoTabs->showPackageInfo(list->at(0), pkgInstalled, aurData);
					}
					updateStatusRunningTask(1);
					delete list;
					if (listInstalled) delete listInstalled;
					if (aurData) delete aurData;
			};
	}, TaskProcessor::eTaskUpdatePackageInfoTab)) {
	//then
		updateStatusNewTask(20);
	}
}

void MainWindow::selectionChanged(const QItemSelection&, const QItemSelection&)
{
	// update Status Bar
	m_statusbar->updateSelected(ui->packageView->getSelectedPackageCount());

	// update Info Tab
	const PackageRepository::PackageData*const package = ui->packageView->getLastSelectedPackage();
	if (package != nullptr) {
		updatePackageInfoTabAsync(*package);
	}
}

void MainWindow::filterChanged(const DefaultPackageFilter* newFilter)
{
	DefaultPackageFilter*const filter = new DefaultPackageFilter(*newFilter);
	applySearchFilterColumn(*filter,
	                        ui->actionSearchByName->isChecked()
	                        ? PackageModel::ctn_PACKAGE_NAME_COLUMN
	                        : PackageModel::ctn_PACKAGE_DESCRIPTION_FILTER_NO_COLUMN);
	applySearchFilter(*filter, m_lePkgSearch->text());
	ui->packageView->setFilter(std::unique_ptr<IPackageFilter>(filter));
	m_statusbar->updateSelected(ui->packageView->getSelectedPackageCount());
}

void MainWindow::loadGroupMemberList(QString group)
{
	if (m_pkgRepo.getPackageList(group).empty()) {
		if (m_cpu.schedule(TaskProcessor::RemoveFirstOfTypePushBack, [this, group](){
				updateStatusStartOfTask(strTaskUpdateGroupMembers());
				auto list = Pacman::getPackageListForGroup(group).release();
				return [this, group, list](){
						m_pkgRepo.checkAndSetMembersOfGroup(group, *list);
						updateStatusRunningTask(40);
						delete list;
				};
		}, TaskProcessor::eTaskUpdateGroupMembers)) {
		//then
			updateStatusNewTask(40);
		}
	}
}

void MainWindow::onRequestForContextMenu(QPoint pt, QList<const PackageRepository::PackageData*>* list)
{
	if (list->isEmpty())
		return;

	QMenu* menu = new QMenu(this);

	bool install = true;
	bool remove  = true;
	foreach (const PackageRepository::PackageData* package, *list) {
		if (package->installed()) {
			install = false;
		}
		else {
			if (package->managedByYaourt) return;
			remove = false;
		}
	}

	if (install) {
		ui->actionInstall_now->setEnabled(true);
		menu->addAction(ui->actionInstall_now);
	} else {
		if (remove) {
			menu->addAction(ui->actionRemove_now);
		}
		else {
			ui->actionInstall_now->setEnabled(false);
			menu->addAction(ui->actionInstall_now);
		}
	}
	menu->exec(pt);
}

void MainWindow::searchEditChanged(const QString& text)
{
	auto fnc = std::bind(&MainWindow::applySearchFilter, this, std::placeholders::_1, text);
	applyFilterChange(fnc);
}

void MainWindow::updateReportRequested()
{
	QString packageNames;
	QString pmPackages;
	QString aurPackages;
	for (auto& pkg : m_pkgRepo.getPackageList()) {
		switch (pkg->status) {
		case epkg_FOREIGN_OUTDATED:
			aurPackages += " " + pkg->name;
			break;
		case epkg_NEWER:
		case epkg_OUTDATED:
			packageNames += " " + pkg->name;
			pmPackages   += " " + pkg->repository + "/" + pkg->name;
			break;
		default:
			continue;
		}
	}

	if (m_cpu.schedule(TaskProcessor::RemoveFirstOfTypePushBack, [this, packageNames, pmPackages, aurPackages](){
			updateStatusStartOfTask(strTaskUpdateReport());
			QList<PackageDetailData>* listPmRepo, * listPmInstalled, * listAurInstalled;
			if (packageNames.isEmpty()) {
				listPmRepo      = new QList<PackageDetailData>();
				listPmInstalled = new QList<PackageDetailData>();
			}
			else {
				listPmRepo      = Pacman::getPackageDetails(pmPackages, false).release();
				listPmInstalled = Pacman::getPackageDetails(packageNames, true).release();
			}
			updateStatusRunningTask(15);
			if (aurPackages.isEmpty()) {
				listAurInstalled = new QList<PackageDetailData>();
			}
			else listAurInstalled = Pacman::getPackageDetails(aurPackages, true).release();
			auto aurInfo = m_distribution.retrieveAurInfo().release();
			updateStatusRunningTask(10);
			return [this, listPmInstalled, listPmRepo, listAurInstalled, aurInfo](){
					ui->infoTabs->showUpdateReport(*listPmInstalled, *listPmRepo, *listAurInstalled, *aurInfo);
					updateStatusRunningTask(5);
					delete listPmInstalled;
					delete listPmRepo;
					delete listAurInstalled;
					delete aurInfo;
			};
	}, TaskProcessor::eTaskUpdateReportInfoTab)) {
	//then
		updateStatusNewTask(30);
	}
}

void MainWindow::on_actionRefresh_View_triggered()
{
	triggerRepoRefresh();
}

void MainWindow::on_actionShow_Toolbar_triggered(bool checked)
{
	if (checked) ui->mainToolBar->show();
	else ui->mainToolBar->hide();
}

void MainWindow::onActionSearchByX_triggered(QAction* action)
{
	// determine filter column
	int temp = -1;
	if (action == ui->actionSearchByName) {
		temp = PackageModel::ctn_PACKAGE_NAME_COLUMN;
	}
	else if (action == ui->actionSearchByDescription) {
		temp = PackageModel::ctn_PACKAGE_DESCRIPTION_FILTER_NO_COLUMN;
	}
	// bind filter
	auto fnc = std::bind(&MainWindow::applySearchFilterColumn,
	                     this, std::placeholders::_1, temp);
	applyFilterChange(fnc);
}

void MainWindow::updateStatusNewTask(int maxIncrement)
{
	m_progressTracker.incMax(maxIncrement);
	auto progress = m_progressTracker.get();
	m_statusbar->updateStatus(QString(), progress.first, progress.second);
}

void MainWindow::updateStatusStartOfTask(QString activity)
{
	auto progress = m_progressTracker.get();
	m_statusbar->updateStatus(activity, progress.first, progress.second);
}

void MainWindow::updateStatusRunningTask(int progressIncrement)
{
	m_progressTracker.incValue(progressIncrement);
	auto progress = m_progressTracker.get();
	m_statusbar->updateStatus(QString(), progress.first, progress.second);
}

void MainWindow::applyFilterChange(std::function<void (DefaultPackageFilter&)> fnc)
{
	// Filter change must be model synchronized with Qt
	ui->packageView->syncFilterChange([=](IPackageFilter& filter){
		fnc(static_cast<DefaultPackageFilter&>(filter));
	});
	// Filter may change package selection
	m_statusbar->updateSelected(ui->packageView->getSelectedPackageCount());
}

void MainWindow::applySearchFilter(DefaultPackageFilter& filter, const QString& searchStr)
{
	filter.applySearchFilter(adaptSearchString(searchStr, false));
}

void MainWindow::applySearchFilterColumn(DefaultPackageFilter& filter, const int filterColumn)
{
	filter.applySearchFilter(filterColumn);
}

/*
 * Returns a modified RegExp-based string given the string entered by the user
 *
 * from Octopi
 */
QString MainWindow::adaptSearchString(QString searchStr, bool exactMatch)
{
  if (searchStr.indexOf("*.") == 0){
    searchStr = searchStr.remove(0, 2);
    searchStr.insert(0, "\\S+\\.");
  }

  if (searchStr.indexOf("*") == 0){
    searchStr = searchStr.remove(0, 1);
    searchStr.insert(0, "\\S+");
  }

  if (searchStr.endsWith("*")){
    searchStr.remove(searchStr.length()-1, 1);
    searchStr.append("\\S*");
  }

  if (searchStr.indexOf("^") == -1 && searchStr.indexOf("\\S") != 0){
    if (!exactMatch) searchStr.insert(0, "\\S*");
    else searchStr.insert(0, "^");
  }

  if (searchStr.indexOf("$") == -1){
    if (!exactMatch && !searchStr.endsWith("\\S*")) searchStr.append("\\S*");
    else searchStr.append("$");
  }

  searchStr.replace("?", ".");
  return searchStr;
}

void MainWindow::endResetRepository(PackageRepository::EResetType type)
{
	if (type == PackageRepository::eResetGroupList)
		return;

	m_statusbar->updatePackagesInfo(m_pkgRepo.countInstalled(), m_pkgRepo.countOutdated(false), m_pkgRepo.countTotal());
}

void MainWindow::on_actionRoot_Terminal_triggered()
{
	Terminal::openRootTerminal();
}

void MainWindow::on_actionSystem_Upgrade_triggered()
{
	// Does not prevent all execution events (just block for the time being and exec later), analyse if acceptable
	// If outdated packages exist, just run pacman -Su on current status and do not sync repo
	QString parameters;
	if (m_pkgRepo.countOutdated(true)) parameters = " -u";

	if (m_cpu.schedule(TaskProcessor::OnlyOne, [this, parameters](){
			updateStatusStartOfTask(strTaskSystemUpgrade());
			Terminal::runSyncInRootTerminal(strScriptsDir() + strSystemUpdateScript() + parameters);
			return [this](){
					triggerRepoRefresh();
					updateStatusRunningTask(500);
			};
	}, TaskProcessor::eTaskPacman)) {
	//then
		updateStatusNewTask(500);
	}
	else {
		//TODO: error notif
	}
}

void MainWindow::on_actionHelp_triggered()
{
	updateHelp(true);
}

void MainWindow::on_actionSync_Repo_triggered()
{
	if (m_cpu.schedule(TaskProcessor::OnlyOne, [this](){
			updateStatusStartOfTask(strTaskSynchronizeRepo());
			Pacman::synchronizeRepositories();
			return [this](){
					triggerRepoRefresh();
					updateStatusRunningTask(100);
			};
	}, TaskProcessor::eTaskSynchronizeRepo)) {
	//then
		updateStatusNewTask(100);
	}
	else {
		//TODO: error notif
	}
}

void MainWindow::on_actionAbout_triggered()
{
	QString aboutText;
	aboutText = QString("\
<b>%1 - %2</b><br>%3<br><br>&copy; 2014 by Thomas Binkau<br>\
<a href=\"%4\">%4</a><br><a href=\"%8\">%8</a><br><br><b>uses:</b><br>	%5<br>	QJson, pacman, curl, vim<br><br>\
%6:<a href=\"%7\">%7</a><br><b>%1 comes with ABSOLUTELY NO WARRANTY</b>")
	    .arg(strAppName()).arg(strAppVersion()).arg(strAppDescription())
	    .arg(strGitHomepage()).arg(strTechnologyUsed()).arg(strLicense()).arg("http://www.gnu.org/licenses/gpl-2.0.html")
	    .arg(strAppHomepage());

	QMessageBox::about(this, strAbout(), aboutText);
}

void MainWindow::on_actionWhatProvides_triggered()
{
	DlgWhatProvidesMe* dlg = new DlgWhatProvidesMe(this);
	dlg->show();
}

void MainWindow::on_actionPacman_Log_Viewer_triggered()
{
	PacmanLogViewer::launch();
}

void MainWindow::on_actionAUR_triggered()
{
	fetchAurInformationAsync();
}

void MainWindow::actionInstallNow_triggered()
{
	QString parameters(" -i \\\"");
	parameters += ui->packageView->getSelectedPackageNames(true) + "\\\"";

	if (m_cpu.schedule(TaskProcessor::OnlyOne, [this, parameters](){
			updateStatusStartOfTask(strTaskSystemInstall());
			Terminal::runSyncInRootTerminal(strScriptsDir() + strSystemInstallScript() + parameters);
			return [this](){
					triggerRepoRefresh();
					updateStatusRunningTask(100);
			};
	}, TaskProcessor::eTaskPacman)) {
	//then
		updateStatusNewTask(100);
	}
	else {
		//TODO: error notif
	}
}

void MainWindow::actionRemoveNow_triggered()
{
	QString parameters(" -r \\\"");
	parameters += ui->packageView->getSelectedPackageNames(false) + "\\\"";

	if (m_cpu.schedule(TaskProcessor::OnlyOne, [this, parameters](){
			updateStatusStartOfTask(strTaskSystemInstall());
			Terminal::runSyncInRootTerminal(strScriptsDir() + strSystemInstallScript() + parameters);
			return [this](){
					triggerRepoRefresh();
					updateStatusRunningTask(100);
			};
	}, TaskProcessor::eTaskPacman)) {
	//then
		updateStatusNewTask(100);
	}
	else {
		//TODO: error notif
	}
}
