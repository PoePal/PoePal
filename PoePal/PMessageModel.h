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

#include <QAbstractItemModel>

class PMessage;

/**
 * An item model that contains all of the log messages that have been parsed by PoePal.
 * This is great for displaying the messages in various UI elements with easy updating. There really should
 * only be a single instance of this class in the application, see PApplication.
 */
class PMessageModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this model.
	 * @param[in] parent
	 *   The parent of the model.
	 */
	PMessageModel(QObject *parent);

	/**
	 * Destructor.
	 */
	virtual ~PMessageModel();

	/**
	 * Overrides QAbstractItemModel#index
	 */
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	/**
	 * Overrides QAbstractItemModel#parent
	 */
	virtual QModelIndex parent(const QModelIndex &child) const override;

	/**
	 * Overrides QAbstractItemModel#rowCount
	 */
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	/**
	 * Overrides QAbstractItemModel#columnCount
	 */
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	/**
	 * Overrides QAbstractItemModel#data
	 */
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	/**
	 * Retrieves the message from an index.
	 * @param[in] index
	 *   The index for which to retrieve the message.
	 * @return
	 *   The message corresponding to the index or null if the index was invalid.
	 */
	PMessage * GetLogMessage(const QModelIndex &index) const;

private slots:

	/**
	 * Slot called when a new message is read by the log scanner.
	 * @param[in] msg
	 *   The new message.
	 */
	void OnNewMessage(PMessage *msg);
};
