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

#include "curlcommands.h"

#include <QProcess>


/**
 * @brief Executes the CURL command with $args
 * @return the StandardError Output, if result code != 0.
 */
QByteArray CurlCommands::performQuery(const QString& args)
{
	QByteArray result;
	QProcess curl;

	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	env.insert("LANG", "C");
	env.insert("LC_MESSAGES", "C");
	env.insert("LC_ALL", "C");
	curl.setProcessEnvironment(env);

	curl.start("/bin/curl " + args);
	curl.waitForFinished(-1);

	if (curl.exitCode() != 0)
		result = curl.readAllStandardError();

	curl.close();
	return result;
}
