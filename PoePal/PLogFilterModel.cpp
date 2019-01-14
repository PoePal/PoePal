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
#include "PLogFilterModel.h"
#include "PLogMessageModel.h"

PLogFilterModel::PLogFilterModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{
}

PLogFilterModel::~PLogFilterModel()
{
}

PLogMessage::Channels PLogFilterModel::GetChannels() const
{
	return _Channels;
}

void PLogFilterModel::SetChannels(const PLogMessage::Channels &channels)
{
	_Channels = channels;
	invalidateFilter();
}

void PLogFilterModel::SetChannels(int channels)
{
	_Channels = static_cast<PLogMessage::Channel>(channels);
	invalidateFilter();
}

QStringList PLogFilterModel::GetSubjects() const
{
	return _Subjects;
}

void PLogFilterModel::SetSubjects(const QStringList &subjects)
{
	_Subjects = subjects;
	invalidateFilter();
}

PLogMessage * PLogFilterModel::GetMessage(const QModelIndex &index) const
{
	auto msgModel = qobject_cast<PLogMessageModel *>(sourceModel());
	if (!msgModel) return false;
	return msgModel->GetLogMessage(mapToSource(index));
}

bool PLogFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	auto msgModel = qobject_cast<PLogMessageModel *>(sourceModel());
	if (!msgModel) return false;
	auto message = msgModel->GetLogMessage(msgModel->index(source_row, 0));
	if (!message) return false;
	return _Channels.testFlag(message->GetChannel()) && 
		(_Subjects.isEmpty() || _Subjects.contains(message->GetSubject()));
}
