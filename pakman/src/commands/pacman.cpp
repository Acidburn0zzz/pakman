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

#include "pacman.h"

#include <QStringList>
#include <QSet>
#include <QRegExp>
#include "pacmancommands.h"


namespace Pacman {
/*
 * Retrieves the list of all available packages in the database (installed + non-installed)
 *
 * from Octopi
 */
std::unique_ptr<QList<PackageListData>> getPackageList()
{
	//archlinuxfr/yaourt 1.2.2-1 [installed]
	//    A pacman wrapper with extended features and AUR support
	//community/libfm 1.1.0-4 (lxde) [installed: 1.1.0-3]

	QString pkgList = PacmanCommands::getPackageList();
	QString pkgName, pkgRepository, pkgVersion, pkgDescription, pkgOutVersion;
	PackageStatus pkgStatus = epkg_NON_INSTALLED;
	QStringList packageTuples = pkgList.split(QRegExp("\\n"), QString::SkipEmptyParts);
	QList<PackageListData> * res = new QList<PackageListData>();

	pkgDescription = "";
	foreach(QString packageTuple, packageTuples)
	{
		if (!packageTuple[0].isSpace())
		{
			//Do we already have a description?
			if (pkgDescription != "")
			{
				pkgDescription = pkgName + " " + pkgDescription;

				PackageListData pld =
					PackageListData(pkgName, pkgRepository, pkgVersion, pkgDescription, pkgStatus, pkgOutVersion);

				res->append(pld);

				pkgDescription = "";
			}

			//First we get repository and name!
			QStringList parts = packageTuple.split(' ');
			QString repoName = parts[0];
			int a = repoName.indexOf("/");
			pkgRepository = repoName.left(a);
			pkgName = repoName.mid(a+1);
			pkgVersion = parts[1];

			if(packageTuple.indexOf("[installed]") != -1)
			{
				//This is an installed package
				pkgStatus = epkg_INSTALLED;
				pkgOutVersion = "";
			}
			else if (packageTuple.indexOf("[installed:") != -1)
			{
				//This is an outdated installed package
				pkgStatus = epkg_OUTDATED;

				int i = packageTuple.indexOf("[installed:");
				pkgOutVersion = packageTuple.mid(i+11);
				pkgOutVersion = pkgOutVersion.remove(']').trimmed();
			}
			else
			{
				//This is an uninstalled package
				pkgStatus = epkg_NON_INSTALLED;
				pkgOutVersion = "";
			}
		}
		else
		{
			//This is a description!
			if (!packageTuple.trimmed().isEmpty())
				pkgDescription += packageTuple.trimmed();
			else
				pkgDescription += " "; //StrConstants::getNoDescriptionAvailabe();
		}
	}

	//And adds the very last package...
	pkgDescription = pkgName + " " + pkgDescription;
	PackageListData pld =
		PackageListData(pkgName, pkgRepository, pkgVersion, pkgDescription, pkgStatus, pkgOutVersion);
	res->append(pld);

	return std::unique_ptr<QList<PackageListData>>(res);
}

std::unique_ptr<QList<PackageListData> > getPackageListForeign()
{
	QString foreignPkgList = PacmanCommands::getPackageListForeign();
	QStringList packageTuples = foreignPkgList.split(QRegExp("\\n"), QString::SkipEmptyParts);
	QList<PackageListData>*const res = new QList<PackageListData>();

	QString pkglist;
	foreach(QString packageTuple, packageTuples)
	{
		QStringList parts = packageTuple.split(' ');
		pkglist += " " + parts[0];
	}

	auto tResult = getPackageDetails(pkglist, true);
	foreach(const PackageDetailData& pkgDetail, *tResult)
	{
		res->append(PackageListData(pkgDetail.name, pkgDetail.repository, pkgDetail.version,
		                            pkgDetail.name + " " + pkgDetail.description, epkg_FOREIGN));
	}

	return std::unique_ptr<QList<PackageListData>>(res);
}

/*
 * Given a package name and if it is default to the official repositories,
 * returns a string containing all of its information fields
 * (ex: name, description, version, dependsOn...)
 *
 * based on Octopi
 */
std::unique_ptr<QList<PackageDetailData>> getPackageDetails(const QString& pkgName, bool installedPackage)
{
	QString pkgInfoAll(QString::fromUtf8(PacmanCommands::getPackageDetails(pkgName, installedPackage)));
	QStringList pkgInfos = pkgInfoAll.split("\n\n", QString::SkipEmptyParts);

	auto output = new QList<PackageDetailData>();
	for (int x = 0; x < pkgInfos.size(); ++x)
	{
		PackageDetailData data;
		const QString& pkgInfo = pkgInfos[x];

		if (pkgInfo.isEmpty())
			continue;

		data.name = getName(pkgInfo);
		data.repository = getRepository(pkgInfo);
		data.version = getVersion(pkgInfo);
		data.url = getURL(pkgInfo);
		data.license = getLicense(pkgInfo);
		data.group = getGroup(pkgInfo);
		data.provides = getProvides(pkgInfo);
		data.requiredBy = getRequiredBy(pkgInfo);
		data.optionalFor = getOptionalFor(pkgInfo);
		data.dependsOn = getDependsOn(pkgInfo);
		data.optDepends = getOptDepends(pkgInfo);
		data.conflictsWith = getConflictsWith(pkgInfo);
		data.replaces = getReplaces(pkgInfo);
		data.packager = getPackager(pkgInfo);
		data.arch = getArch(pkgInfo);
		data.description = getDescription(pkgInfo);
		data.buildDate = getBuildDate(pkgInfo);
		data.downloadSize = getDownloadSize(pkgInfo);
		data.installedSize = getInstalledSize(pkgInfo);
		output->push_back(data);
	}
	return std::unique_ptr<QList<PackageDetailData>>(output);
}

/*
 * Retrieves the list of all package groups available
 *
 * from Octopi
 */
std::unique_ptr<QStringList> getPackageGroups()
{
	QString packagesFromGroup = PacmanCommands::getPackageGroups();
	QStringList groups = packagesFromGroup.split(QRegExp("\\n"), QString::SkipEmptyParts);
	QStringList* res = new QStringList();

	foreach(QString group, groups)
	{
		group = group.trimmed();
		if (!group.isEmpty())
			res->append(group); //We only return the group name
	}

	res->removeDuplicates();
	res->sort();
	return std::unique_ptr<QStringList>(res);
}

/*
 * Retrieves the list of packages for a specific group
 */
std::unique_ptr<QStringList> getPackageListForGroup(const QString& groupName)
{
	QString packagesOfGroup = PacmanCommands::getPackageListForGroup(groupName);
	QStringList packages = packagesOfGroup.split(QRegExp("\\n"), QString::SkipEmptyParts);
	QStringList* res = new QStringList();

	foreach(QString packageTuple, packages) {
		QStringList parts = packageTuple.split(' ');
		if (parts.size() == 2) {
			res->append(parts[1]);
		}
	}

	return std::unique_ptr<QStringList>(res);
}

/*
 * Retrieves the list of explicitly installed packages (not installed as dependency)
 */
std::unique_ptr<QSet<QString>> getExplicitPackageList()
{
	QString explicitPkgList = PacmanCommands::getExplicitlyInstalledPackageList();
	QStringList packageTuples = explicitPkgList.split(QRegExp("\\n"), QString::SkipEmptyParts);
	QSet<QString>* res = new QSet<QString>();

	foreach(QString packageTuple, packageTuples)
	{
		QStringList parts = packageTuple.split(' ');
		res->insert(parts[0]); //We only return the package name!
		//TODO: check if conflicts by repo/name can occur
	}
	return std::unique_ptr<QSet<QString>>(res);
}

/*
 * Retrieves the list of unrequired packages (those no other packages depends on)
 *
 * from Octopi
 */
std::unique_ptr<QSet<QString>> getUnrequiredPackageList()
{
	QString unrequiredPkgList = PacmanCommands::getUnrequiredPackageList();
	QStringList packageTuples = unrequiredPkgList.split(QRegExp("\\n"), QString::SkipEmptyParts);
	QSet<QString>* res = new QSet<QString>();

	foreach(QString packageTuple, packageTuples)
	{
		QStringList parts = packageTuple.split(' ');
		res->insert(parts[0]); //We only return the package name! Conflicts by repo/name should not, but can probably occur
	}
	return std::unique_ptr<QSet<QString>>(res);
}

/**
 * @brief will sync the repo (-Sy)
 */
void synchronizeRepositories()
{
	PacmanCommands::synchronizeRepositories();
}

/////////// Helper ///////////////////////////////////////////////////////////////////////

/*
 * Given a QString containing the output of pacman -Si/Qi (pkgInfo),
 * this method returns the contents of the given field (ex: description)
 *
 * from Octopi
 */
QString extractFieldFromInfo(const QString& field, const QString& pkgInfo)
{
	int fieldPos = pkgInfo.indexOf(field);
	int fieldEnd, fieldEnd2;
	QString aux;

	if (fieldPos >= 0 && fieldPos + 1 < pkgInfo.size())
	{
		if (field == "Optional Deps")
		{
			fieldPos = pkgInfo.indexOf(":", fieldPos + 1);
			fieldPos += 2;
			aux = pkgInfo.mid(fieldPos);

			fieldEnd = aux.indexOf("Conflicts With");
			fieldEnd2 = aux.indexOf("Required By");

			if (fieldEnd > fieldEnd2 && fieldEnd2 != -1) fieldEnd = fieldEnd2;

			aux = aux.left(fieldEnd).trimmed();
			aux = aux.replace("\n", "<br>");
		}
		else
		{
			fieldPos = pkgInfo.indexOf(":", fieldPos + 1);
			fieldPos += 2;
			aux = pkgInfo.mid(fieldPos);
			fieldEnd = aux.indexOf('\n');
			aux = aux.left(fieldEnd).trimmed();
		}
	}

	return aux;
}

/*
 * Retrieves "Name" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getName(const QString& pkgInfo)
{
	return extractFieldFromInfo("Name", pkgInfo);
}

/*
 * Retrieves "Version" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getVersion(const QString& pkgInfo)
{
	return extractFieldFromInfo("Version", pkgInfo);
}

/*
 * Retrieves "Repository" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getRepository(const QString& pkgInfo)
{
	return extractFieldFromInfo("Repository", pkgInfo);
}

/*
 * Retrieves "URL" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getURL(const QString& pkgInfo)
{
	return extractFieldFromInfo("\nURL", pkgInfo);
}

/*
 * Retrieves "Licenses" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getLicense(const QString& pkgInfo)
{
	return extractFieldFromInfo("Licenses", pkgInfo);
}

/*
 * Retrieves "Groups" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getGroup(const QString& pkgInfo)
{
	return extractFieldFromInfo("Groups", pkgInfo);
}

/*
 * Retrieves "Provides" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getProvides(const QString& pkgInfo)
{
	return extractFieldFromInfo("Provides", pkgInfo);
}

/*
 * Retrieves "Depends On" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getDependsOn(const QString& pkgInfo)
{
	return extractFieldFromInfo("Depends On", pkgInfo);
}

/*
 * Retrieves "Optional Deps" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getOptDepends(const QString& pkgInfo)
{
	return extractFieldFromInfo("Optional Deps", pkgInfo);
}

/*
 * Retrieves "Conflicts With" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getConflictsWith(const QString& pkgInfo)
{
	return extractFieldFromInfo("Conflicts With", pkgInfo);
}

/*
 * Retrieves "Replaces" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getReplaces(const QString& pkgInfo)
{
	return extractFieldFromInfo("Replaces", pkgInfo);
}

/*
 * Retrieves "RequiredBy" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getRequiredBy(const QString& pkgInfo)
{
	return extractFieldFromInfo("Required By", pkgInfo);
}

/*
 * Retrieves "OptionalFor" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getOptionalFor(const QString& pkgInfo)
{
	return extractFieldFromInfo("Optional For", pkgInfo);
}

/*
 * Retrieves "Packager" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getPackager(const QString& pkgInfo)
{
	return extractFieldFromInfo("Packager", pkgInfo);
}

/*
 * Retrieves "Architecture" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getArch(const QString& pkgInfo)
{
	return extractFieldFromInfo("Architecture", pkgInfo);
}

/*
 * Retrieves "Build Date" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QDateTime getBuildDate(const QString& pkgInfo)
{
	QString aux = extractFieldFromInfo("Build Date", pkgInfo);
	return QDateTime::fromString(aux); //"ddd MMM d hh:mm:ss yyyy");
}

/*
 * Retrieves "Download Size" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
double getDownloadSize(const QString& pkgInfo)
{
	QString aux = extractFieldFromInfo("Download Size", pkgInfo);
	aux = aux.section(QRegExp("\\s"), 0, 0);

	bool ok;
	double res = aux.toDouble(&ok);

	if (ok)
		return res;
	else
		return 0;
}

/*
 * Retrieves "Installed Size" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
double getInstalledSize(const QString& pkgInfo)
{
	QString aux = extractFieldFromInfo("Installed Size", pkgInfo);
	aux = aux.section(QRegExp("\\s"), 0, 0);

	bool ok;
	double res = aux.toDouble(&ok);

	if (ok)
		return res;
	else
		return 0;
}

/*
 * Retrieves "Description" field of the given package information string represented by pkgInfo
 *
 * from Octopi
 */
QString getDescription(const QString& pkgInfo)
{
	return extractFieldFromInfo("Description", pkgInfo);
}

/**
 * This function was copied from ArchLinux Pacman project
 *
 * Compare alpha and numeric segments of two versions.
 * return 1: a is newer than b
 *        0: a and b are the same version
 *       -1: b is newer than a
 */
//TODO: checkme
int rpmvercmp(const char *a, const char *b) {
	char oldch1, oldch2;
	char *str1, *str2;
	char *ptr1, *ptr2;
	char *one, *two;
	int rc;
	int isnum;
	int ret = 0;

  /* easy comparison to see if versions are identical */
  if(strcmp(a, b) == 0) return 0;

  str1 = strdup(a);
  str2 = strdup(b);

  one = ptr1 = str1;
  two = ptr2 = str2;

  /* loop through each version segment of str1 and str2 and compare them */
  while (*one && *two) {
    while (*one && !isalnum((int)*one)) one++;
    while (*two && !isalnum((int)*two)) two++;

    /* If we ran to the end of either, we are finished with the loop */
    if (!(*one && *two)) break;

    /* If the separator lengths were different, we are also finished */
    if ((one - ptr1) != (two - ptr2)) {
      return (one - ptr1) < (two - ptr2) ? -1 : 1;
    }

    ptr1 = one;
    ptr2 = two;

    /* grab first completely alpha or completely numeric segment */
    /* leave one and two pointing to the start of the alpha or numeric */
    /* segment and walk ptr1 and ptr2 to end of segment */
    if (isdigit((int)*ptr1)) {
      while (*ptr1 && isdigit((int)*ptr1)) ptr1++;
      while (*ptr2 && isdigit((int)*ptr2)) ptr2++;
      isnum = 1;
    } else {
      while (*ptr1 && isalpha((int)*ptr1)) ptr1++;
      while (*ptr2 && isalpha((int)*ptr2)) ptr2++;
      isnum = 0;
    }

    /* save character at the end of the alpha or numeric segment */
    /* so that they can be restored after the comparison */
    oldch1 = *ptr1;
    *ptr1 = '\0';
    oldch2 = *ptr2;
    *ptr2 = '\0';

    /* this cannot happen, as we previously tested to make sure that */
    /* the first string has a non-null segment */
    if (one == ptr1) {
      ret = -1;       /* arbitrary */
      goto cleanup;
    }

    /* take care of the case where the two version segments are */
    /* different types: one numeric, the other alpha (i.e. empty) */
    /* numeric segments are always newer than alpha segments */
    /* XXX See patch #60884 (and details) from bugzilla #50977. */
    if (two == ptr2) {
      ret = isnum ? 1 : -1;
      goto cleanup;
    }

    if (isnum) {
      /* this used to be done by converting the digit segments */
      /* to ints using atoi() - it's changed because long  */
      /* digit segments can overflow an int - this should fix that. */

      /* throw away any leading zeros - it's a number, right? */
      while (*one == '0') one++;
      while (*two == '0') two++;

      /* whichever number has more digits wins */
      if (strlen(one) > strlen(two)) {
        ret = 1;
        goto cleanup;
      }
      if (strlen(two) > strlen(one)) {
        ret = -1;
        goto cleanup;
      }
    }

    /* strcmp will return which one is greater - even if the two */
    /* segments are alpha or if they are numeric.  don't return  */
    /* if they are equal because there might be more segments to */
    /* compare */
    rc = strcmp(one, two);
    if (rc) {
      ret = rc < 1 ? -1 : 1;
      goto cleanup;
    }

    /* restore character that was replaced by null above */
    *ptr1 = oldch1;
    one = ptr1;
    *ptr2 = oldch2;
    two = ptr2;
  }

  /* this catches the case where all numeric and alpha segments have */
  /* compared identically but the segment separating characters were */
  /* different */
  if ((!*one) && (!*two)) {
    ret = 0;
    goto cleanup;
  }

  /* the final showdown. we never want a remaining alpha string to
         * beat an empty string. the logic is a bit weird, but:
         * - if one is empty and two is not an alpha, two is newer.
         * - if one is an alpha, two is newer.
         * - otherwise one is newer.
         * */
  if ( (!*one && !isalpha((int)*two))
       || isalpha((int)*one) ) {
    ret = -1;
  } else {
    ret = 1;
  }

cleanup:
  free(str1);
  free(str2);
  return ret;
}

}
