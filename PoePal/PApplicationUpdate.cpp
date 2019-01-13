/**
 * PoePal - A companion application to Path of Exile.
 * Copyright (C) 2019 Phillip Doup (https://github.com/douppc)
 *
 * PoePal is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) 
 * any later version.
 *
 * PoePal is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along with PeoPal.  If not, see 
 * <https://www.gnu.org/licenses/>.
 */
#include "PApplicationUpdate.h"
#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QUrl>

PApplicationUpdate::PApplicationUpdate()
{
}


PApplicationUpdate::PApplicationUpdate(const QJsonObject &json):
_Json(json)
{

}

PApplicationUpdate::PApplicationUpdate(const PApplicationUpdate &other):
_Json(other._Json)
{

}

PApplicationUpdate & PApplicationUpdate::operator=(const PApplicationUpdate &other)
{
	if (this == &other) return *this;
	_Json = other._Json;
	return *this;
}

PApplicationUpdate::~PApplicationUpdate()
{
}

bool PApplicationUpdate::IsValid() const
{
	return !_Json.isEmpty();
}

QString PApplicationUpdate::GetVersion() const
{
	return _Json.value(QStringLiteral("tag_name")).toString().mid(1);
}

bool PApplicationUpdate::IsNewer() const
{
	auto thisVersion = GetVersion();
	auto appVersion = qApp->applicationVersion();
	auto thisVerTokens = thisVersion.split('.');
	auto appVerTokens = appVersion.split('.');
	if (thisVerTokens.length() == 3 && appVerTokens.length() == 3)
	{
		for (int v = 0; v < 3; ++v)
		{
			if (appVerTokens.at(v).toInt() < thisVerTokens.at(v).toInt()) return true;
		}
	}
	else qDebug() << "Version strings don't look right. Update Version: " << thisVersion <<
		"  Application Version: " << appVersion;
	return false;
}

QString PApplicationUpdate::GetName() const
{
	return _Json.value(QStringLiteral("name")).toString();
}

QString PApplicationUpdate::GetBody() const
{
	return _Json.value(QStringLiteral("body")).toString();
}

QUrl PApplicationUpdate::GetDownloadUrl() const
{
	auto assets = _Json.value(QStringLiteral("assets")).toArray();
	if (assets.count() > 0)
	{
		return QUrl(assets.at(0).toObject().value(QStringLiteral("browser_download_url")).toString());
	}
	return QUrl();
}
