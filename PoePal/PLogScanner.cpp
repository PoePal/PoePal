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
#include "PLogScanner.h"
#include <QBuffer>
#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QJSValue>
#include <QTimer>

PLogScanner::PLogScanner(QObject *parent)
	: QObject(parent)
{
	_Watcher = new QFileSystemWatcher();
	auto pfFolder = getenv("ProgramFiles(x86)");
	_LogDirPath = QStringList{ pfFolder,  "Grinding Gear Games", "Path of Exile", "logs" }.
		join(QDir::separator());
	_LogFilePath = _LogDirPath + QDir::separator() + "Client.txt";
	_Watcher->addPath(_LogDirPath);
	_Timer.setInterval(10);

	connect(&_Timer, &QTimer::timeout, this, &PLogScanner::OnFileChanged);
	connect(_Watcher, &QFileSystemWatcher::directoryChanged, this, &PLogScanner::OnDirectoryChanged);

	QTimer::singleShot(0, this, &PLogScanner::OnDirectoryChanged);
}

PLogScanner::~PLogScanner()
{
}

QList<PLogMessage *> PLogScanner::GetLogMessages() const
{
	return _Messages;
}

void PLogScanner::OnFileChanged()
{
	if (_File->atEnd()) return;
	QByteArray contents = _Remainder + _File->readAll();
	int lineBegin = 0;
	int lineEnd = 0;
	while ((lineEnd = contents.indexOf('\r', lineBegin)) >= 0)
	{
		auto line = QString::fromUtf8(contents.mid(lineBegin, lineEnd - lineBegin));
		auto newMsg = PLogMessage::FromString(line, this);
		if (newMsg)
		{
			_Messages.append(newMsg);
			if(_Initialized) emit NewMessage(newMsg);
		}
		else qDebug() << "Line not recognized as a message: " << line;
		lineBegin = lineEnd + 1;
	}
}

void PLogScanner::OnDirectoryChanged()
{
	// If the file is already open or doesn't exist, don't bother.
	if (_File || !QFile::exists(_LogFilePath)) return;
	_File = new QFile(_LogFilePath, this);
	if (!_File->open(QIODevice::ReadOnly))
	{
		qWarning("Could not open client log file for reading.");
		delete _File;
		_File = nullptr;
	}
	_File->seek(_File->size());
	_Watcher->deleteLater();
	_Timer.start();
	_Initialized = true;
	emit Initialized();
}

QQmlListProperty<PLogMessage> PLogScanner::GetLogMessagesProperty() const
{
	static QQmlListProperty<PLogMessage>::CountFunction countFunc = [](QQmlListProperty<PLogMessage> *prop)
	{
		auto scanner = qobject_cast<PLogScanner *>(prop->object);
		Q_ASSERT(scanner);
		return scanner->GetLogMessages().length();
	};
	static QQmlListProperty<PLogMessage>::AtFunction atFunc = [](QQmlListProperty<PLogMessage> *prop,
		int index)
	{
		auto scanner = qobject_cast<PLogScanner *>(prop->object);
		Q_ASSERT(scanner);
		return scanner->GetLogMessages().at(index);
	};
	return QQmlListProperty<PLogMessage>(const_cast<PLogScanner *>(this), nullptr, countFunc, atFunc);
}

