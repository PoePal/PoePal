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

#include "PLogMessage.h"

#include <QObject>
#include <QQmlListProperty>
#include <QTextStream>
#include <QTimer>

class QFile;
class QFileSystemWatcher;
class QJSValue;

/**
 * Scans the PoE log file for messages.
 */
class PLogScanner : public QObject
{
	Q_OBJECT

	/**
	 * The log messages discovered by the scanner.
	 * @accessors GetLogMessages()
	 */
	Q_PROPERTY(QQmlListProperty<PLogMessage> messages READ GetLogMessagesProperty)

public:

	/**
	 * Creates a new log scanner.
	 * @param[in] parent
	 *   The parent of the log scanner.
	 */
	PLogScanner(QObject *parent=nullptr);

	/**
	 * Destructor.
	 */
	virtual ~PLogScanner();

	/**
	 * Retrieves all log messages that have been loaded.
	 * @return
	 *   The list of all log messages.
	 */
	QList<PLogMessage *> GetLogMessages() const;

signals:

	/**
	 * Signal sent when the initial load of messages is finished.
	 */
	void Initialized();

	/**
	 * Signal sent when a new message is received.
	 * @param[in] message
	 *   The new message.
	 */
	void NewMessage(PLogMessage *message);

private slots:

	/**
	 * Slot called when the Client.txt file changes.
	 */
	void OnFileChanged();

	/**
	 * Slot called when the log directory changes.
	 */
	void OnDirectoryChanged();

private:

	/**
	 * 
	 */
	QQmlListProperty<PLogMessage> GetLogMessagesProperty() const;

	/**
	 * The file system watcher that looks at the Client.txt
	 */
	QFileSystemWatcher *_Watcher = nullptr;

	/**
	 * The path to the log directory.
	 */
	QString _LogDirPath;
	
	/**
	 * The path to the log file.
	 */
	QString _LogFilePath;

	/**
	 * The file open on the Client.txt file.
	 */
	QFile *_File = nullptr;

	/**
	 * The timer looking at the file.
	 */
	QTimer _Timer;

	/**
	 * The remainder of the file.
	 */
	QByteArray _Remainder;

	/**
	 * The list of log messages.
	 */
	QList<PLogMessage *> _Messages;

	/**
	 * Indicates whether the log scanner has been initialized.
	 */
	bool _Initialized = false;

	/**
	 * The position of the first message loaded.
	 */
	qint64 _FirstMessagePos = -1;
};
