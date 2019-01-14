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
#include "PLogMessage.h"
#include "PApplication.h"
#include <QClipboard>
#include <QDebug>
#include <QJSEngine>
#include <QJSValue>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QTimer>
#include "windows.h"

namespace {
	/**
	 * Retrieves the list of type strings we know about.
	 */
	QStringList * GetTypeStrings()
	{
		static QStringList strings{
			"INFO",
			"DEBUG",
			"WARN",
			"INVALID"
		};
		return &strings;
	}

	/**
	 * Retrieves the list of channel prefixes.
	 */
	QHash<PLogMessage::Channel, QChar> * GetChannelPrefixes()
	{
		static QHash<PLogMessage::Channel, QChar> prefixes{
			{PLogMessage::Local, '\0'},
			{PLogMessage::Global, '#'},
			{PLogMessage::Party, '%'},
			{PLogMessage::Trade, '$'},
			{PLogMessage::Guild, '&'},
			{PLogMessage::Whisper, '@'}
		};
		return &prefixes;
	}
	/**
	 * Retrieves the list of channel strings.
	 */
	QHash<PLogMessage::Channel, QString> * GetChannelStrings()
	{
		static QHash<PLogMessage::Channel, QString> strings{
			{PLogMessage::Local, "Local"},
			{PLogMessage::Global, "Global"},
			{PLogMessage::Party, "Party"},
			{PLogMessage::Trade, "Trade"},
			{PLogMessage::Guild, "Guild"},
			{PLogMessage::Whisper, "Whisper"}
		};
		return &strings;
	}
}

QString PLogMessage::GetStringFromType(Type type)
{
	return GetTypeStrings()->at(static_cast<int>(type));
}

PLogMessage::Type PLogMessage::GetTypeFromString(const QString &string)
{
	int typeIdx = GetTypeStrings()->indexOf(string);
	if (typeIdx <= 3) return static_cast<Type>(typeIdx);
	return Invalid;
}

QChar PLogMessage::GetPrefixFromChannel(Channel channel)
{
	return GetChannelPrefixes()->value(channel, '!');
}

PLogMessage::Channel PLogMessage::GetChannelFromPrefix(const QChar &prefix)
{
	return GetChannelPrefixes()->key(prefix);
}

QString PLogMessage::GetStringFromChannel(Channel channel)
{
	return GetChannelStrings()->value(channel, "Invalid");
}

PLogMessage::Channel PLogMessage::GetChannelFromString(const QString &string)
{
	return GetChannelStrings()->key(string);
}

QString PLogMessage::GetChannelLabel(Channel channel)
{
	static QHash<PLogMessage::Channel, QString> labels{
		{PLogMessage::Local, tr("Local")},
		{PLogMessage::Global, tr("Global")},
		{PLogMessage::Party, tr("Party")},
		{PLogMessage::Trade, tr("Trade")},
		{PLogMessage::Guild, tr("Guild")},
		{PLogMessage::Whisper, tr("Whisper")}
	};
	return labels.value(channel, "Invalid");
}

QList<PLogMessage::Channel> PLogMessage::GetChannels()
{
	return QList<Channel>{Global, Trade, Guild, Party, Local, Whisper};
}

PLogMessage * PLogMessage::FromString(const QString &string, QObject *parent)
{
	static QRegularExpression msgRegex(
		"(\\d{4}\\/\\d{2}\\/\\d{2}\\s\\d{2}:\\d{2}:\\d{2})\\s(\\d+)\\s([0-9a-f]+)\\s\\[(INFO|WARN|DEBUG)\\sClient\\s(\\d+)\\]\\s(.*)");
	static QRegularExpression chatRegex("^(\\$|#|@|&)?(From |To )?(?:\\<([^\\>]+)\\>\\s)?([^\\s:]*):\\s(.*)$");
	auto match = msgRegex.match(string);
	if (!match.hasMatch()) return nullptr;
	auto msg = new PLogMessage(parent);
	msg->_Date = QDateTime::fromString(match.captured(1), QStringLiteral("yyyy/MM/dd HH:mm:ss"));
	msg->_Id = match.captured(2).toLong();
	msg->_Code = match.captured(3).toInt(nullptr, 16);
	msg->_Type = GetTypeFromString(match.captured(4));
	msg->_ClientId = match.captured(5).toInt();
	auto contents = match.captured(6);
	auto chatMatch = chatRegex.match(contents);
	if (chatMatch.hasMatch())
	{
		msg->_ChatSubject = chatMatch.captured(4);
		msg->_ChatSubjectGuild = chatMatch.captured(3);
		msg->_Contents = chatMatch.captured(5);
		if (msg->_ChatSubject.isEmpty()) msg->_Subtype = Event;
		else
		{
			msg->_Channel = GetChannelFromPrefix(chatMatch.captured(1)[0]);
			if (msg->_Channel == Whisper) msg->_IsIncoming = chatMatch.captured(2) == tr("From ");
			msg->_Subtype = Chat;
		}
	}
	else msg->_Contents = contents;
	return msg;
}

void PLogMessage::SendChatMessage(Channel channel, const QString &message, 
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
	QString text = GetPrefixFromChannel(channel);
	if (text[0] == '\0') text.clear();
	if (channel == Whisper) text += target + ' ';
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

void PLogMessage::SendPlayerAction(const QString &target, PlayerAction action)
{
	static QByteArrayList actionList{
		"invite",
		"kick",
		"friend",
		"unfriend",
		"accept",
		"ignore",
		"unignore",
		"trade",
		"whois",
		"hideout"
	};
	if (target.isEmpty()) return;
	SendChatMessage(Local, "/" + actionList.at(action) + " " + target);
}

PLogMessage::PLogMessage(QObject *parent):
QObject(parent)
{
}

PLogMessage::~PLogMessage()
{
}

QDateTime PLogMessage::GetTime() const
{
	return _Date;
}

qint64 PLogMessage::GetId() const
{
	return _Id;
}

qint16 PLogMessage::GetCode() const
{
	return _Code;
}

PLogMessage::Type PLogMessage::GetType() const
{
	return _Type;
}

PLogMessage::Subtype PLogMessage::GetSubtype() const
{
	return _Subtype;
}

QString PLogMessage::GetContents() const
{
	return _Contents;
}

QString PLogMessage::GetFullContents() const
{
	if (_Subtype == Chat) return GetPrefixFromChannel(_Channel) % GetFullSender() % " : " % _Contents;
	else if (_Subtype == Event) return ": " %_Contents;
	return _Contents;
}

PLogMessage::Channel PLogMessage::GetChannel() const
{
	return _Channel;
}

QString PLogMessage::GetSubject() const
{
	return _ChatSubject;
}

QString PLogMessage::GetSubjectGuild() const
{
	return _ChatSubjectGuild;
}

QString PLogMessage::GetFullSender() const
{
	if (!_ChatSubjectGuild.isEmpty()) return "<" % _ChatSubjectGuild % "> " % _ChatSubject;
	return _ChatSubject;
}

bool PLogMessage::IsIncoming() const
{
	return _IsIncoming;
}

QString PLogMessage::ToString() const
{
	static QString format("%1 %2 %3 [%4 Client %5] %6");
	return format.arg(_Date.toString(QStringLiteral("yyyy/MM/dd HH:mm:ss")), QString::number(_Id),
		QString::number(_Code, 16), GetStringFromType(_Type), QString::number(_ClientId), 
		GetFullContents());
}
