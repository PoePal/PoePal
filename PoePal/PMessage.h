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

#include <QDateTime>
#include <QObject>

/**
 * Represents a message from the log.
 * Messages are interpreted to be of the form:
 *   <date> <time> <id> <code> <category/client> <contents>
 */
class PMessage : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QDateTime time READ GetTime)
	Q_PROPERTY(qint64 id READ GetId)
	Q_PROPERTY(qint16 code READ GetCode)
	Q_PROPERTY(Type type READ GetType)
	Q_PROPERTY(Subtype subtype READ GetSubtype)
	Q_PROPERTY(QString contents READ GetFullContents)
	Q_PROPERTY(Channel channel READ GetChannel)
	Q_PROPERTY(QString sender READ GetSubject)
	Q_PROPERTY(QString senderGuild READ GetSubjectGuild)
	Q_PROPERTY(QString fullSender READ GetFullSender)
	Q_PROPERTY(QString asString READ ToString)

public:

	/**
	 * The message type.
	 * @param Info
	 *   PoE reported the message as information
	 * @param Debug
	 *   PoE reported the message as debug
	 * @param WARN
	 *   PoE reported the message as a warning
	 * @param InvalidType
	 *   The type is invalid. This should never happen, but if it does, it means there is a new type of
	 *   message we don't know about.
	 */
	enum Type : qint8 {
		Info,
		Debug,
		Warn,
		Invalid
	};
	Q_ENUM(Type)

	/**
	 * Retrieves the string for a type.
	 * This fetches the string that is in the log file.
	 * @param[in] type
	 *   The type for which to retrieve the string.
	 * @return
	 *   The string for the type.
	 */
	Q_INVOKABLE static QString GetStringFromType(Type type);

	/**
	 * Retrieves the type from a log string.
	 * @param[in] string
	 *   The log string from which to retrieve the type.
	 * @return
	 *   The type matching the string or Invalid if the string is an invalid type.
	 */
	Q_INVOKABLE static Type GetTypeFromString(const QString &string);

	/**
	 * The subtype of the message.
	 * @param Chat
	 *   The message is a chat message.
	 * @param Event
	 *   The message represents an event
	 * @param Log
	 *   The message is some sort of log message from PoE.
	 */
	enum Subtype : qint8 {
		Chat,
		Event,
		Log
	};
	Q_ENUM(Subtype)

	/**
	 * Types of chat messages.
	 * @param Local
	 *   Message was not sent on a channel.
	 * @param Global
	 *   Message was sent on current global channel.
	 * @param Party
	 *   Message was sent on party channel.
	 * @param Trade
	 *   Message was sent on trade channel.
	 * @param Guild
	 *   Message was sent on guild channel.
	 * @param Whisper
	 *   Message was sent as a whisper.
	 * @param InvalidChannel
	 *   The channel is invalid.
	 */
	enum Channel
	{
		Local = (1 << 0),
		Global = (1 << 1),
		Party = (1 << 2),
		Trade = (1 << 3),
		Guild = (1 << 4),
		Whisper = (1 << 5),
		InvalidChannel = 0
	};
	Q_ENUM(Channel)
	Q_DECLARE_FLAGS(Channels, Channel)

	/**
	 * Retrieves the channel character from the channel.
	 * @param[in] channel
	 *   The channel for which to retrieve the character.
	 * @return
	 *   The prefix for the channel.
	 */
	static QChar GetPrefixFromChannel(Channel channel);

	/**
	 * Retrieves the channel from a prefix.
	 * @param[in] prefix
	 *   The prefix from which to retrieve the channel.
	 * @return
	 *   The corresponding channel or InvalidChannel if the prefix is invalid.
	 */
	static Channel GetChannelFromPrefix(const QChar &prefix);

	/**
	 * Retrieves the string from a channel.
	 * @param[in] channel
	 *   The channel for which to retrieve the string.
	 * @return
	 *   The prefix for the channel.
	 */
	static QString GetStringFromChannel(Channel channel);

	/**
	 * Retrieves the channel from a prefix.
	 * @param[in] string
	 *   The string from which to retrieve the channel.
	 * @return
	 *   The corresponding channel or InvalidChannel if the prefix is invalid.
	 */
	static Channel GetChannelFromString(const QString &string);

	/**
	 * Retrieves the label for a channel.
	 * @param[in] channel
	 *   The channel for which to retrieve the label.
	 * @return
	 *   The label for the channel.
	 */
	static QString GetChannelLabel(Channel channel);

	/**
	 * Retrieves the possible list of channels.
	 * @return
	 *   The list of channels.
	 */
	static QList<Channel> GetChannels();

	/**
	 * Creates a new log message from a string.
	 * @param[in] string
	 *   The string from which to create the log message.
	 * @param[in] parent
	 *   The parent of the new log message.
	 * @return
	 *   The log message or null if the string was an invalid format.
	 */
	static PMessage * FromString(const QString &string, QObject *parent);

	/**
	 * Creates a new log message.
	 * @param[in] parent
	 *   The parent of the new log message.
	 */
	PMessage(QObject *parent = nullptr);

	/**
	 * Destructor.
	 */
	virtual ~PMessage();

	/**
	 * Retrieves the time of the message.
	 * @return
	 *   The timestamp of the message.
	 */
	QDateTime GetTime() const;

	/**
	 * Retrieves the message ID.
	 * @return
	 *   The message ID.
	 */
	qint64 GetId() const;

	/**
	 * Retrieves the code for the message.
	 * @return
	 *   The message code.
	 */
	qint16 GetCode() const;

	/**
	 * Retrieves the type of the message.
	 * @return
	 *   The type of the message.
	 */
	Type GetType() const;

	/**
	 * Retrieves the subtype of the message.
	 * @return
	 *   The message subtype.
	 */
	Subtype GetSubtype() const;

	/**
	 * Retrieves the contents of the message.
	 * This constitutes the information of the message. If this is chat, this is the message itself sans the
	 * sender, etc.
	 * @return
	 *   The contents of the message.
	 */
	QString GetContents() const;

	/**
	 * Retrieves the entire contents of the message.
	 * This includes the sender and all of the original message.
	 * @return
	 *   The raw contents of the message.
	 */
	QString GetFullContents() const;

	/**
	 * Retrieves the channel if it is a chat message.
	 * @return
	 *   The channel of the chat message or InvalidChannel if this was not a chat message.
	 */
	Channel GetChannel() const;

	/**
	 * Retrieves the message subject, if it is a chat message.
	 * @return
	 *   The message subject. This will be an empty string if the message is not a chat message.
	 */
	QString GetSubject() const;

	/**
	 * Retrieves the guild of the message subject, if it is a chat message.
	 * @return
	 *   The message subject's guild. This will be an empty string if the message is not a chat message.
	 */
	QString GetSubjectGuild() const;

	/**
	 * Retrieves the full display name for the sender (guild + name).
	 * @return
	 *   The full display name for the sender.
	 */
	QString GetFullSender() const;

	/**
	 * Indicates if the chat message is an incoming chat message.
	 * @return
	 *   false if the chat message is an outgoing whisper, true otherwise.
	 */
	bool IsIncoming() const;

	/**
	 * Indicates whether or not the message is interpreted to be a trade request.
	 * @return
	 *   true if the message is a trade request, false otherwise.
	 */
	bool IsTradeRequest() const;

	/**
	 * Retrieves the name of the item being traded.
	 * @return
	 *   The name of the item being traded.
	 */
	QString GetTradeItem() const;

	/**
	 * Retrieves the amount of currency being traded.
	 * @return
	 *   The amount of currency to trade.
	 */
	float GetTradeAmount() const;

	/**
	 * Retrieves the currency being traded.
	 * @return
	 *   The trade abbreviation of the currency being traded.
	 */
	QString GetTradeCurrency() const;

	/**
	 * Retrieves the league in which the trade will occur.
	 * @return
	 *   The league in which the trade will occur.
	 */
	QString GetTradeLeague() const;

	/**
	 * Retrieves the tab containing the item being traded.
	 * @return
	 *   The name of the tab containing the item to trade.
	 */
	QString GetTradeTab() const;

	/**
	 * Retrieves the left position in the containing tab of the item being traded.
	 * @return
	 *   The left position of the item being trade in the tab containing it.
	 */
	int GetTradeLeftPosition() const;

	/**
	 * Retrieves the top position in the containing tab of the item being traded.
	 * @return
	 *   The top position of the item being trade in the tab containing it.
	 */
	int GetTradeTopPosition() const;

	/**
	 * Converts the message to a string.
	 * @return
	 *   The string representing the message.
	 */
	QString ToString() const;

private:

	/**
	 * The date of the message.
	 */
	QDateTime _Date;

	/**
	 * The id of the message.
	 */
	qint64 _Id = 0;

	/**
	 * The code of the message.
	 */
	qint16 _Code = 0;

	/**
	 * The type of the message.
	 */
	Type _Type = Invalid;

	/**
	 * The subtype of the message.
	 */
	Subtype _Subtype = Log;

	/**
	 * The client ID.
	 */
	qint16 _ClientId = 0;

	/**
	 * The contents of the message.
	 */
	QString _Contents;

	/**
	 * The channel if it is a chat message.
	 */
	Channel _Channel = InvalidChannel;

	/**
	 * The subject of the chat message.
	 */
	QString _ChatSubject;

	/**
	 * The guild of the subject of a chat message.
	 */
	QString _ChatSubjectGuild;

	/**
	 * Structure defining the extra trade request info.
	 */
	struct TradeReqInfo
	{
		/**
		 * The item being traded for.
		 */
		QString _Item;

		/**
		 * The amount of currency being traded.
		 */
		float _Amount = 0.0;

		/**
		 * The currency type being traded.
		 */
		QString _Currency;

		/**
		 * The league where the trade will occur.
		 */
		QString _League;

		/**
		 * The tab containing the item to be traded.
		 */
		QString _Tab;

		/**
		 * The left position of the item in the tab.
		 */
		int _Left = 0;

		/**
		 * The top position of the item in the tab.
		 */
		int _Top = 0;
	};

	/**
	 * The trade request details.
	 */
	QScopedPointer<TradeReqInfo> _TradeInfo;

	/**
	 * Indicates whether or not a chat message is incoming or outgoing.
	 * This will always be true unless the chat message is a whisper.
	 */
	bool _IsIncoming = true;
};
Q_DECLARE_METATYPE(PMessage::Channels)
Q_DECLARE_OPERATORS_FOR_FLAGS(PMessage::Channels)
