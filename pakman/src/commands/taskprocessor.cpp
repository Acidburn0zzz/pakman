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

#include "taskprocessor.h"

#include <cassert>


TaskProcessor::TaskProcessor(QObject* parent)
	: QObject(parent), m_shutdown(false)
{
	connect(&m_watch, SIGNAL(finished()), this, SLOT(finishedSlot()));
}

TaskProcessor::TTaskQueue::iterator TaskProcessor::findFirstOf(const bool onlyOneMode,
                                                               const TaskProcessor::ETaskType type)
{
	if (onlyOneMode == false && m_tasks.size() <= 1) return m_tasks.end();

	auto start = m_tasks.begin();
	if (onlyOneMode == false)
		++start;

	auto t = std::find_if(start, m_tasks.end(), [type](const std::unique_ptr<TTask>& item){
		                      return item->m_type == type;
	                      });
	return t;
}

bool TaskProcessor::schedule(const TaskProcessor::ETaskInsertMode mode, const std::function<std::function<void ()> ()>& function, const TaskProcessor::ETaskType type) {
	m_sync.lock();
	// Duplicate / shutdown check
	if ((mode == OnlyOne && m_tasks.empty() == false && findFirstOf(true, type) != m_tasks.cend()) || m_shutdown)
	{
		m_sync.unlock();
		return false;
	}
	// Task will be inserted
	bool returnValue = true;
	if (type == eTaskShutdown) m_shutdown = true;
	TTask*const task = new TTask(type, function);
	switch (mode) {
	case RemoveFirstOfTypePushBack: {
		auto it = findFirstOf(false, type);
		if (it != m_tasks.end()) {
			returnValue = false;
			m_tasks.erase(it);
		}
	}
		//FALLTHROUGH
	case OnlyOne:
	case PushBack:
		m_tasks.emplace_back(task);
		break;
	case RemoveFirstOfTypeOverwrite: {
		auto it = findFirstOf(false, type);
		if (it != m_tasks.end()) {
			returnValue = false;
			it->reset(task);
		}
		else m_tasks.emplace_back(task);
		break;
	}
	default:
		assert(false);
		break;
	}
	const int size = m_tasks.size();
	m_sync.unlock();
	if (size == 1) start(*task);
	return returnValue;
}

bool TaskProcessor::hasTasks()
{
	std::lock_guard<std::mutex> lock(m_sync);
	const std::size_t size = m_tasks.size();
	return size > 1 || (m_shutdown == false && size > 0);
}

void TaskProcessor::start(TaskProcessor::TTask& task) {
	QFuture<void> fut = QtConcurrent::run(&task, &TTask::run);
	m_watch.setFuture(fut);
}

/**
 * @brief TaskProcessor::finishedSlot must be executed in Qt context
 */
void TaskProcessor::finishedSlot() {
	m_sync.lock();
	const std::unique_ptr<TTask>& task = m_tasks.front();
	m_sync.unlock();

	task->m_followUp();

	m_sync.lock();
	m_tasks.pop_front();
	if (m_tasks.empty()) {
		m_sync.unlock();
		return;
	}
	const std::unique_ptr<TTask>& nextTask = m_tasks.front();
	m_sync.unlock();
	start(*nextTask.get());
}


TaskProcessor::TTask::TTask(TaskProcessor::ETaskType type, const std::function<std::function<void ()> ()>& function)
  : m_type(type), m_exec(function)
{}

void TaskProcessor::TTask::run() {
	m_followUp = m_exec();
}
