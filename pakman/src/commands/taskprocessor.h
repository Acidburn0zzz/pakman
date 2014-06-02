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

#ifndef PACMANQT_TASKPROCESSOR_H
#define PACMANQT_TASKPROCESSOR_H

#include <functional>
#include <memory>
#include <mutex>
#include <deque>

#include <QObject>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>


/**
 * @brief will schedule tasks and execute sequentially in two steps each (1. async exec, 2. exec in qt context)
 *
 * To use with Qt-Ui it must be created in the main thread
 */
class TaskProcessor : public QObject
{
	Q_OBJECT

public:
	enum ETaskType {
		eTaskUnspecified,     // general purpose, intended for use with pushback insert mode only
		eTaskShutdown,        // will be the last task accepted
		eTaskFetchPackageListForeign,
		eTaskSynchronizeRepo,
		eTaskSystemUpgrade,
		eTaskUpdateDistributionNews,
		eTaskUpdateGroupList,
		eTaskUpdateGroupMembers,
		eTaskUpdatePackageInfoTab,
		eTaskUpdatePackageList,
		eTaskUpdatePackageListForeign,
		eTaskUpdateReportInfoTab
	};
	enum ETaskInsertMode {
		OnlyOne,                   // There may be only one task of that type queued (or running)
		PushBack,                  // Default mode (push back)
		RemoveFirstOfTypePushBack, // Remove first task of same type and push back (will not remove running tasks)
		RemoveFirstOfTypeOverwrite // Remove first task of same type by overwrite (will not remove running tasks)
	};

private:
	/**
	 * @brief container for a single task, must not be visible to users
	 */
	class TTask {
	public:
		TTask(ETaskType type, const std::function<std::function<void()>()>& function);

		void run();

		/// public data
		const ETaskType m_type;
		const std::function<std::function<void()>()> m_exec;
		std::function<void()> m_followUp;
	};

private:
	typedef std::deque<std::unique_ptr<TTask>> TTaskQueue;

	////////////////////////

public:
	explicit TaskProcessor(QObject* parent = nullptr);

	/**
	 * @brief will schedule %function for sequential execution
	 * @param mode (push back, remove first of same type and push back or overwrite first)
	 * @param function (2 stage function)
	 * @param type of the task (important for RFPB and RFO)
	 * @return true if task was scheduled AND no overwrite occurred
	 */
	bool schedule(const ETaskInsertMode mode, const std::function<std::function<void()>()>& function,
	              const ETaskType type);

	bool hasTasks();

private:
	TaskProcessor::TTaskQueue::iterator findFirstOf(const bool onlyOneMode, const TaskProcessor::ETaskType type);
	void start(TTask& task);

signals:
	void finished(TTask*const task);

protected slots:
	void finishedSlot();

private:
	bool       m_shutdown;
	std::mutex m_sync;
	TTaskQueue m_tasks;

	QFutureWatcher<void> m_watch;
};

#endif // PACMANQT_TASKPROCESSOR_H
