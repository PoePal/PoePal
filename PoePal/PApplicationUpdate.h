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
#pragma once

#include <QJsonObject>

class QUrl;

/**
 * Class provides data about an available application update.
 */
class PApplicationUpdate
{
public:

	/**
	 * Creates a new invalid application update.
	 */
	PApplicationUpdate();

	/**
	 * Creates a new application update from JSON.
	 * @param[in] json
	 *   The JSON describing the update.
	 */
	PApplicationUpdate(const QJsonObject &json);

	/**
	 * Copy constructor.
	 * @param[in] other
	 *   The other application update to copy.
	 */
	PApplicationUpdate(const PApplicationUpdate &other);

	/**
	 * Assignment operator.
	 * @param[in] other
	 *   The other application update to copy.
	 * @return
	 *   This application update.
	 */
	PApplicationUpdate & operator=(const PApplicationUpdate &other);

	/**
	 * Destructor.
	 */
	~PApplicationUpdate();

	/**
	 * Indicates if the application update is valid.
	 * @return
	 *   true if the application update is valid, false otherwise.
	 */
	bool IsValid() const;

	/**
	 * Retrieves the version of the update.
	 * @return
	 *   The version in the format 'x.y.z'
	 */
	QString GetVersion() const;

	/**
	 * Tests if the update version is newer than the application version.
	 * @return
	 *   true if the update is version is newer, false otherwise.
	 */
	bool IsNewer() const;

	/**
	 * Retrieves the name of the release.
	 * @return
	 *   The name of the release.
	 */
	QString GetName() const;

	/**
	 * Retrieves the body of the release.
	 * @return
	 *   The body of the release.
	 */
	QString GetBody() const;

	/**
	 * Retrieves the download URL for the release.
	 * @return
	 *   The download URL for the release.
	 */
	QUrl GetDownloadUrl() const;

private:

	/**
	 * The JSON describing the update.
	 */
	QJsonObject _Json;
};

