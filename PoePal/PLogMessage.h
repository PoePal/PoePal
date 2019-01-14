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
class PLogMessage : public QObject
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
	 * Types of actions to perform on players.
	 * @param Invite
	 *   Invites the player to a party.
	 * @param Kick
	 *   Kicks the player from the current party.
	 * @param Friend
	 *   Adds the player as a friend.
	 * @param Unfriend
	 *   Removes the player as a friend.
	 * @param Accept
	 *   Accepts a friend request from the player.
	 * @param Ignore
	 *   Ignores the player.
	 * @param Unignore
	 *   Removes the player from the ignore list.
	 * @param OpenTrade
	 *   Requests to trade with the player.
	 * @param Whois
	 *   Performs a whois request for the player.
	 * @param Hideout
	 *   Goes to the hideout of the player.
	 */
	enum PlayerAction
	{
		Invite,
		Kick,
		Friend,
		Unfriend,
		Accept,
		Ignore,
		Unignore,
		OpenTrade,
		Whois,
		Hideout
	};
	Q_ENUM(PlayerAction)

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
	static PLogMessage * FromString(const QString &string, QObject *parent);

	/**
	 * Sends a chat message to PoE if it is open.
	 * @param[in] channel
	 *   The channel to which to send the message.
	 * @param[in] message
	 *   The message to send.
	 * @param[in] target
	 *   The player to send the message to, if it is a whisper.
	 */
	static void SendChatMessage(Channel channel, const QString &message, const QString &target = QString());

	/**
	 * Executes an action for the given target player.
	 * @param[in] target
	 *   The target player for which to execute the action.
	 * @param[in] action
	 *   The action to perform for the player.
	 */
	static void SendPlayerAction(const QString &target, PlayerAction action);

	/**
	 * Creates a new log message.
	 * @param[in] parent
	 *   The parent of the new log message.
	 */
	PLogMessage(QObject *parent = nullptr);

	/**
	 * Destructor.
	 */
	virtual ~PLogMessage();

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
	 * Indicates whether or not a chat message is incoming or outgoing.
	 * This will always be true unless the chat message is a whisper.
	 */
	bool _IsIncoming = true;
};
Q_DECLARE_METATYPE(PLogMessage::Channels)
Q_DECLARE_OPERATORS_FOR_FLAGS(PLogMessage::Channels)
