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
#include "PLogMessageModel.h"
#include "PApplication.h"
#include "PLogScanner.h"

PLogMessageModel::PLogMessageModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto scanner = app->GetLogScanner();
	Q_ASSERT(scanner);
	connect(scanner, &PLogScanner::NewMessage, this, &PLogMessageModel::OnNewMessage);
}

PLogMessageModel::~PLogMessageModel()
{
}

QModelIndex PLogMessageModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto scanner = app->GetLogScanner();
	Q_ASSERT(scanner);
	auto messages = scanner->GetLogMessages();
	if (row >= 0 && row < messages.length()) return createIndex(row, column, messages.at(row));
	return QModelIndex();
}

QModelIndex PLogMessageModel::parent(const QModelIndex &child) const
{
	// We will never have any parents.
	return QModelIndex();
}

int PLogMessageModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto scanner = app->GetLogScanner();
	Q_ASSERT(scanner);
	return scanner->GetLogMessages().length();
}

int PLogMessageModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 1;
}

QVariant PLogMessageModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid() || role != Qt::DisplayRole) return QVariant();
	auto msg = GetLogMessage(index);
	if (role == Qt::DisplayRole) return msg->ToString();
	return QVariant();
}

PLogMessage * PLogMessageModel::GetLogMessage(const QModelIndex &index) const
{
	if (!index.isValid()) return nullptr;
	return static_cast<PLogMessage *>(index.internalPointer());
}

void PLogMessageModel::OnNewMessage(PLogMessage *msg)
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto scanner = app->GetLogScanner();
	Q_ASSERT(scanner);
	auto messages = scanner->GetLogMessages();
	beginInsertRows(QModelIndex(), messages.length() - 1, messages.length() - 1);
	endInsertRows();
}
