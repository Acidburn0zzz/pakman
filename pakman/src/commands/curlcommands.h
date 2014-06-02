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

#ifndef CURLCOMMANDS_H
#define CURLCOMMANDS_H

#include <QByteArray>


/**
 * @brief collection of all available curl commands, will deliver raw data
 */
class CurlCommands
{
public:

	/**
	 * @brief will execute curl in default lang with $args
	 * @param args e.g "http://abc.de/file -o /home/blub"
	 * @return raw data from stderr
	 */
	static QByteArray performQuery(const QString& args);
};

#endif // CURLCOMMANDS_H
