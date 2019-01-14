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

#include <QSortFilterProxyModel>
#include "PLogMessage.h"

/**
 * Item model that filters another model based on the channel and sender.
 */
class PLogFilterModel : public QSortFilterProxyModel
{
	Q_OBJECT

	/**
	 * The list of channels that are included in the model.
	 */
	Q_PROPERTY(int channels READ GetChannels WRITE SetChannels NOTIFY ChannelsChanged)

	/**
	 * The list of subjects that are included in the model.
	 */
	Q_PROPERTY(QStringList subjects READ GetSubjects WRITE SetSubjects NOTIFY SubjectsChanged)

public:

	/**
	 * Creates a new instance of this model.
	 * @param[in] parent
	 *   The parent of the model.
	 */
	PLogFilterModel(QObject *parent);

	/**
	 * Destructor.
	 */
	virtual ~PLogFilterModel();

	/**
	 * Retrieves the list of channels included in the model.
	 * @param[in] channels
	 *   The list of channels included in the model.
	 */
	PLogMessage::Channels GetChannels() const;

	/**
	 * Sets the list of channels included in the model.
	 * @param[in] channels
	 *   The new list of channels to be included in the model.
	 */
	void SetChannels(const PLogMessage::Channels &channels);

	/**
	 * Retrieves the list of subjects included in the model.
	 * @return
	 *   The list of subjects included in the model.
	 */
	QStringList GetSubjects() const;

	/**
	 * Sets the list of subjects included in the model.
	 * @param[in] subjects
	 *   The new list of subjects included in the model.
	 */
	void SetSubjects(const QStringList &subjects);

	/**
	 * Retrieves the log message for an index in this model.
	 * @param[in] index
	 *   The index in this model.
	 * @return
	 *   The corresponding message.
	 */
	PLogMessage * GetMessage(const QModelIndex &index) const;

signals:

	/**
	 * Signal sent when the accepted channels changes.
	 * @param[in] channels
	 *   The new set of channels.
	 */
	void ChannelsChanged(int channels);

	/**
	 * Signal sent when the list of subjects changes.
	 * @param[in] subjects
	 *   The new list of subjects.
	 */
	void SubjectsChanged(const QStringList &subjects);

protected:

	/**
	 * Overrides QSortFilterProxyModel#filterAcceptsRow
	 */
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:

	/**
	 * Sets the channels for the property.
	 * @param[in] channels
	 *   The new channels to include in the model.
	 */
	void SetChannels(int channels);

	/**
	 * The list of channels to be allowed.
	 * If this is empty, they're all allowed.
	 */
	PLogMessage::Channels _Channels = 0xFF;

	/**
	 * The list of subjects that are allowed.
	 */
	QStringList _Subjects;
};
