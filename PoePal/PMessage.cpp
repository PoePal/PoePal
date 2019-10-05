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
#include "PMessage.h"
#include "PApplication.h"
#include <QDebug>
#include <QJSEngine>
#include <QJSValue>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QTimer>

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
	QHash<PMessage::Channel, QChar> * GetChannelPrefixes()
	{
		static QHash<PMessage::Channel, QChar> prefixes{
			{PMessage::Local, '\0'},
			{PMessage::Global, '#'},
			{PMessage::Party, '%'},
			{PMessage::Trade, '$'},
			{PMessage::Guild, '&'},
			{PMessage::Whisper, '@'}
		};
		return &prefixes;
	}
	/**
	 * Retrieves the list of channel strings.
	 */
	QHash<PMessage::Channel, QString> * GetChannelStrings()
	{
		static QHash<PMessage::Channel, QString> strings{
			{PMessage::Local, "Local"},
			{PMessage::Global, "Global"},
			{PMessage::Party, "Party"},
			{PMessage::Trade, "Trade"},
			{PMessage::Guild, "Guild"},
			{PMessage::Whisper, "Whisper"}
		};
		return &strings;
	}
}

QString PMessage::GetStringFromType(Type type)
{
	return GetTypeStrings()->at(static_cast<int>(type));
}

PMessage::Type PMessage::GetTypeFromString(const QString &string)
{
	int typeIdx = GetTypeStrings()->indexOf(string);
	if (typeIdx <= 3) return static_cast<Type>(typeIdx);
	return Invalid;
}

QChar PMessage::GetPrefixFromChannel(Channel channel)
{
	return GetChannelPrefixes()->value(channel, '!');
}

PMessage::Channel PMessage::GetChannelFromPrefix(const QChar &prefix)
{
	return GetChannelPrefixes()->key(prefix);
}

QString PMessage::GetStringFromChannel(Channel channel)
{
	return GetChannelStrings()->value(channel, "Invalid");
}

PMessage::Channel PMessage::GetChannelFromString(const QString &string)
{
	return GetChannelStrings()->key(string);
}

QString PMessage::GetChannelLabel(Channel channel)
{
	static QHash<PMessage::Channel, QString> labels{
		{PMessage::Local, tr("Local")},
		{PMessage::Global, tr("Global")},
		{PMessage::Party, tr("Party")},
		{PMessage::Trade, tr("Trade")},
		{PMessage::Guild, tr("Guild")},
		{PMessage::Whisper, tr("Whisper")}
	};
	return labels.value(channel, "Invalid");
}

QList<PMessage::Channel> PMessage::GetChannels()
{
	return QList<Channel>{Global, Trade, Guild, Party, Local, Whisper};
}

PMessage * PMessage::FromString(const QString &string, QObject *parent)
{
	static QRegularExpression msgRegex(
		"(\\d{4}\\/\\d{2}\\/\\d{2}\\s\\d{2}:\\d{2}:\\d{2})\\s(\\d+)\\s([0-9a-f]+)\\s\\[(INFO|WARN|DEBUG)\\sClient\\s(\\d+)\\]\\s(.*)");
	static QRegularExpression chatRegex("^(\\$|#|@|&|%)?(From |To )?(?:\\<([^\\>]+)\\>\\s)?([^\\s:]*):\\s(.*)$");
	static QRegularExpression tradeRegex(
		"Hi, I would like to buy your ([\\w\\s+()]+) listed for (\\d+) (\\w+) in (\\w+) \\(stash tab \"([^ \\\"]+)\"; position: left(\\d + ), top(\\d + )\\)");
	auto match = msgRegex.match(string);
	if (!match.hasMatch()) return nullptr;
	auto msg = new PMessage(parent);
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
			auto tradeMatch = tradeRegex.match(msg->_Contents);
			if (tradeMatch.hasMatch())
			{
				msg->_TradeInfo.reset(new TradeReqInfo);
				msg->_TradeInfo->_Item = tradeMatch.captured(1);
				msg->_TradeInfo->_Amount = tradeMatch.captured(2).toFloat();
				msg->_TradeInfo->_Currency = tradeMatch.captured(3);
				msg->_TradeInfo->_League = tradeMatch.captured(4);
				msg->_TradeInfo->_Tab = tradeMatch.captured(5);
				msg->_TradeInfo->_Left = tradeMatch.captured(6).toInt();
				msg->_TradeInfo->_Top = tradeMatch.captured(7).toInt();
			}
		}
	}
	else msg->_Contents = contents;
	return msg;
}

PMessage::PMessage(QObject *parent):
QObject(parent)
{
}

PMessage::~PMessage()
{
}

QDateTime PMessage::GetTime() const
{
	return _Date;
}

qint64 PMessage::GetId() const
{
	return _Id;
}

qint16 PMessage::GetCode() const
{
	return _Code;
}

PMessage::Type PMessage::GetType() const
{
	return _Type;
}

PMessage::Subtype PMessage::GetSubtype() const
{
	return _Subtype;
}

QString PMessage::GetContents() const
{
	return _Contents;
}

QString PMessage::GetFullContents() const
{
	if (_Subtype == Chat) return GetPrefixFromChannel(_Channel) % GetFullSender() % " : " % _Contents;
	else if (_Subtype == Event) return ": " %_Contents;
	return _Contents;
}

PMessage::Channel PMessage::GetChannel() const
{
	return _Channel;
}

QString PMessage::GetSubject() const
{
	return _ChatSubject;
}

QString PMessage::GetSubjectGuild() const
{
	return _ChatSubjectGuild;
}

QString PMessage::GetFullSender() const
{
	if (!_ChatSubjectGuild.isEmpty()) return "<" % _ChatSubjectGuild % "> " % _ChatSubject;
	return _ChatSubject;
}

bool PMessage::IsIncoming() const
{
	return _IsIncoming;
}

bool PMessage::IsTradeRequest() const
{
	return _TradeInfo;
}

QString PMessage::GetTradeItem() const
{
	if (_TradeInfo) return _TradeInfo->_Item;
	return QString();
}

float PMessage::GetTradeAmount() const
{
	if (_TradeInfo) return _TradeInfo->_Amount;
	return 0.0;
}

QString PMessage::GetTradeCurrency() const
{
	if (_TradeInfo) return _TradeInfo->_Currency;
	return QString();
}

QString PMessage::GetTradeLeague() const
{
	if (_TradeInfo) return _TradeInfo->_League;
	return QString();
}

QString PMessage::GetTradeTab() const
{
	if (_TradeInfo) return _TradeInfo->_Tab;
	return QString();
}

int PMessage::GetTradeLeftPosition() const
{
	if (_TradeInfo) return _TradeInfo->_Left;
	return 0;
}

int PMessage::GetTradeTopPosition() const
{
	if (_TradeInfo) return _TradeInfo->_Top;
	return 0;
}

QString PMessage::ToString() const
{
	static QString format("%1 %2 %3 [%4 Client %5] %6");
	return format.arg(_Date.toString(QStringLiteral("yyyy/MM/dd HH:mm:ss")), QString::number(_Id),
		QString::number(_Code, 16), GetStringFromType(_Type), QString::number(_ClientId), 
		GetFullContents());
}
