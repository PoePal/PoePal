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
#include "PMessageHandler.h"
#include "PApplication.h"
#include <QBuffer>
#include <QClipboard>
#include <QDebug>
#include <QDir>
#include <QFileSystemWatcher>
#include <QJSValue>
#include <QTimer>
#include "windows.h"

namespace
{
	/**
	 * The complete list of commands.
	 */
	static QStringList _Commands
	{
		"bug",
		"ladder",
		"played",
		"age",
		"passives",
		"deaths",
		"remaining",
		"pvp",
		"fixmyhelmet",
		"oos",
		"dance",
		"status",
		"debug",
		"invite",
		"kick",
		"party_description",
		"tradewith",
		"friend",
		"unfriend",
		"accept",
		"ignore",
		"unignore",
		"clear_ignore_list",
		"whois",
		"afk",
		"afkoff",
		"dnd",
		"global",
		"trade",
		"cls",
		"hideout",
		"menagerie",
		"abandon_daily",
		"exit",
		"reset_xp",
		"recheck_achievements",
		"autoreply"
	};
}

PMessageHandler::PMessageHandler(QObject *parent)
	: QObject(parent)
{
	_Watcher = new QFileSystemWatcher();
	auto pfFolder = getenv("ProgramFiles(x86)");
	_LogDirPath = QStringList{ pfFolder,  "Grinding Gear Games", "Path of Exile", "logs" }.
		join(QDir::separator());
	_LogFilePath = _LogDirPath + QDir::separator() + "Client.txt";
	_Watcher->addPath(_LogDirPath);
	_Timer.setInterval(10);

	connect(&_Timer, &QTimer::timeout, this, &PMessageHandler::OnFileChanged);
	connect(_Watcher, &QFileSystemWatcher::directoryChanged, this, &PMessageHandler::OnDirectoryChanged);

	QTimer::singleShot(0, this, &PMessageHandler::OnDirectoryChanged);
}

PMessageHandler::~PMessageHandler()
{
}

QList<PMessage *> PMessageHandler::GetLogMessages() const
{
	return _Messages;
}

QList<PMessageHandler::Action> PMessageHandler::GetAllActions() const
{
	static QList<Action> actions{
		Bug,
		Ladder,
		Played,
		Age,
		Passives,
		Deaths,
		Remaining,
		Pvp,
		FixMyHelmet,
		Oos,
		Dance,
		Status,
		Debug,
		Invite,
		Kick,
		PartyDescription,
		TradeWith,
		Friend,
		Unfriend,
		Accept,
		Ignore,
		Unignore,
		Squelch,
		Unsquelch,
		ClearIgnoreList,
		Whois,
		Afk,
		Afkoff,
		Dnd,
		Global,
		Trade,
		Cls,
		Hideout,
		Hideout,
		Menagerie,
		AbandonDaily,
		Exit,
		ResetXp,
		RecheckAchievements,
		Autoreply
	};
	return actions;
}

QString PMessageHandler::GetCommandFromAction(Action action) const
{
	if (action == InvalidAction) return QString();
	return _Commands.at(action);
}

PMessageHandler::Action PMessageHandler::GetActionFromCommand(const QString &command) const
{
	return static_cast<PMessageHandler::Action>(_Commands.indexOf(command));
}

void PMessageHandler::SendChatMessage(PMessage::Channel channel, const QString &message, 
	const QString &target /*= QString()*/)
{
	auto thisWin = GetActiveWindow();
	auto hwnd = FindWindow(NULL, L"Path of Exile");
	if (!hwnd)
	{
		qWarning() << "Sending chat message, Path of Exile window not found";
		return;
	}
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	QString text = PMessage::GetPrefixFromChannel(channel);
	if (text[0] == '\0') text.clear();
	if (channel == PMessage::Whisper) text += target + ' ';
	text += message;
	app->clipboard()->setText(text);
	auto threadId = GetWindowThreadProcessId(hwnd, nullptr);
	auto currThread = GetCurrentThreadId();
	AttachThreadInput(currThread, threadId, TRUE);
	SetFocus(hwnd);
	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = 0;
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;

	// Press the "Enter" key
	input.ki.wVk = VK_RETURN;
	input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Release the "Enter" key
	input.ki.wVk = VK_RETURN;
	input.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Press the "Ctrl" key
	input.ki.wVk = VK_CONTROL;
	input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Press the "V" key
	input.ki.wVk = 'A';
	input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Release the "V" key
	input.ki.wVk = 'A';
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));

	// Release the "Ctrl" key
	input.ki.wVk = VK_CONTROL;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));

	// Press the "Backspace" key
	input.ki.wVk = VK_BACK;
	input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Release the "Backspace" key
	input.ki.wVk = VK_BACK;
	input.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Press the "Ctrl" key
	input.ki.wVk = VK_CONTROL;
	input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Press the "V" key
	input.ki.wVk = 'V';
	input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Release the "V" key
	input.ki.wVk = 'V';
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));

	// Release the "Ctrl" key
	input.ki.wVk = VK_CONTROL;
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));

	// Press the "Enter" key
	input.ki.wVk = VK_RETURN;
	input.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	// Release the "Enter" key
	input.ki.wVk = VK_RETURN;
	input.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
	SendInput(1, &input, sizeof(INPUT));

	AttachThreadInput(currThread, threadId, FALSE);
	QTimer::singleShot(10, [thisWin]() {
		SetFocus(thisWin);
		SetForegroundWindow(thisWin);
		SetActiveWindow(thisWin);
	});
}

void PMessageHandler::SendAction(Action action, const QString &args/*=QString()*/)
{
	if(args.isEmpty()) SendChatMessage(PMessage::Local, "/" + GetCommandFromAction(action));
	else  SendChatMessage(PMessage::Local, "/" + GetCommandFromAction(action) + " " + args);
}

void PMessageHandler::OnFileChanged()
{
	if (_File->atEnd()) return;
	QByteArray contents = _Remainder + _File->readAll();
	int lineBegin = 0;
	int lineEnd = 0;
	while ((lineEnd = contents.indexOf('\r', lineBegin)) >= 0)
	{
		auto line = QString::fromUtf8(contents.mid(lineBegin, lineEnd - lineBegin));
		auto newMsg = PMessage::FromString(line, this);
		if (newMsg)
		{
			_Messages.append(newMsg);
			if(_Initialized) emit NewMessage(newMsg);
		}
		else qDebug() << "Line not recognized as a message: " << line;
		lineBegin = lineEnd + 1;
	}
}

void PMessageHandler::OnDirectoryChanged()
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

QQmlListProperty<PMessage> PMessageHandler::GetLogMessagesProperty() const
{
	static QQmlListProperty<PMessage>::CountFunction countFunc = [](QQmlListProperty<PMessage> *prop)
	{
		auto scanner = qobject_cast<PMessageHandler *>(prop->object);
		Q_ASSERT(scanner);
		return scanner->GetLogMessages().length();
	};
	static QQmlListProperty<PMessage>::AtFunction atFunc = [](QQmlListProperty<PMessage> *prop,
		int index)
	{
		auto scanner = qobject_cast<PMessageHandler *>(prop->object);
		Q_ASSERT(scanner);
		return scanner->GetLogMessages().at(index);
	};
	return QQmlListProperty<PMessage>(const_cast<PMessageHandler *>(this), nullptr, countFunc, atFunc);
}

