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
#include "PMessageFilterModel.h"
#include "PMessageModel.h"

PMessageFilterModel::PMessageFilterModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{
}

PMessageFilterModel::~PMessageFilterModel()
{
}

PMessage::Channels PMessageFilterModel::GetChannels() const
{
	return _Channels;
}

void PMessageFilterModel::SetChannels(const PMessage::Channels &channels)
{
	_Channels = channels;
	invalidateFilter();
}

void PMessageFilterModel::SetChannels(int channels)
{
	_Channels = static_cast<PMessage::Channel>(channels);
	invalidateFilter();
}

QStringList PMessageFilterModel::GetSubjects() const
{
	return _Subjects;
}

void PMessageFilterModel::SetSubjects(const QStringList &subjects)
{
	_Subjects = subjects;
	invalidateFilter();
}

PMessage * PMessageFilterModel::GetMessage(const QModelIndex &index) const
{
	auto msgModel = qobject_cast<PMessageModel *>(sourceModel());
	if (!msgModel) return false;
	return msgModel->GetLogMessage(mapToSource(index));
}

bool PMessageFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	auto msgModel = qobject_cast<PMessageModel *>(sourceModel());
	if (!msgModel) return false;
	auto message = msgModel->GetLogMessage(msgModel->index(source_row, 0));
	if (!message) return false;
	return _Channels.testFlag(message->GetChannel()) && 
		(_Subjects.isEmpty() || _Subjects.contains(message->GetSubject()));
}
