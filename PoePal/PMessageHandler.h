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

#include "PMessage.h"

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
class PMessageHandler : public QObject
{
	Q_OBJECT

	/**
	 * The log messages discovered by the scanner.
	 */
	Q_PROPERTY(QQmlListProperty<PMessage> messages READ GetLogMessagesProperty)

public:

	/**
	 * Types of actions to perform.
	 * These actions ultimately get transformed into commands in the chat console in the game.
	 * @param InvalidAction
	 *   The action is invalid.
	 * @param Ladder
	 *   Displays the top ten characters on the current ladder.
	 * @param Played
	 *   Displays the length of time the current character has been played.
	 * @param Age
	 *   Displays how long ago the current character was created.
	 * @param Passives
	 *   Displays a summary of all passive skill points and the Deal with the Bandits reward the character has
	 *   gained.
	 * @param Deaths
	 *   Displays how many deaths the current character has incurred.
	 * @param Remaining	
	 *   Displays how many monsters remain alive in the current area.
	 * @param Pvp
	 *   Displays Win/Loss/Disconnect statistics for 1v1 and 3v3 PvP.
	 * @param Oos
	 *   Forces resync.
	 * @param Dance
	 *   Perform a dance.
	 * @param Status
	 *   Change your status. Status is displayed to all friends as an alert when changed, and is shown to 
	 *   friends in the social window.
	 * @param Debug
	 *   Displays debug information.
	 * @param Invite
	 *   Sends a party invite to <character>.
	 * @param Kick
	 *   Kicks <character> from the party.
	 * @param PartyDescription
	 *   Changes the description of the party to <description>.
	 * @param TradeWith
	 *   Initiates a trade with <character>. Characters must be in the same town hub instance to trade.
	 * @param Friend 
	 *   Adds <character> to the friends list.
	 * @param Unfriend
	 *   Removes <character> from the friends list.
	 * @param Accept
	 *   Accepts friend request.
	 * @param Ignore
	 *   Adds a player account specified by character name to the ignore list. No messages will be received 
	 *   from ignored players.
	 * @param Unignore
	 *   Removes a player account specified by character name from the ignore list.
	 * @param Squelch
	 *   Alternative to /ignore. Adds a player account specified by character name to the ignore list.
	 * @param Unsquelch
	 *   Alternative to /unignore. Removes a player account specified by character name from the ignore list.
	 * @param ClearIgnoreList
	 *   Removes all player accounts from the ignore list.
	 * @param Whois
	 *   Displays a character's level, class, league, and whether he is online.
	 * @param Afk
	 *   Turns AFK mode on, replying with <message> when someone whispers you. A default message will be used 
	 *   if one isn't specified.
	 * @param Afkoff
	 *   Turns off AFK mode
	 * @param Dnd
	 *   Toggles Do Not Disturb mode for chat. When on, no messages including whispers will be received.
	 * @param Global
	 *   Joins global chat channel <number>.
	 * @param Trade
	 *   Joins trade chat channel <number>.
	 * @param Cls
	 *   Clears the chat console of text.
	 * @param Hideout
	 *   Sends you to your hideout. Only useable while in town. Also works with <character> to send you to a
	 *   character's hideout.
	 * @param Menagerie	
	 *   Sends you to your Menagerie. Only useable while in town.
	 * @param AbandonDaily
	 *   Abandons your ongoing daily master missions.
	 * @param Exit
	 *   Exits the game to the Character selection screen.
	 * @param ResetXp
	 *   Resets the experience-per-hour estimation tool.
	 * @param RecheckAchievements
	 *   Forces a recheck of certain achievements.
	 * @param Autoreply
	 *   Replying with <message> when someone whispers you.
	 * @param Delve
	 *   Enters the Azurite Mine.
	 */
	enum Action
	{
		InvalidAction=-1,
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
		Squelch = Ignore,
		Unsquelch = Unignore,
		ClearIgnoreList,
		Whois,
		Afk,
		Afkoff,
		Dnd,
		Global,
		Trade,
		Cls,
		Hideout,
		Menagerie,
		AbandonDaily,
		Exit,
		ResetXp,
		RecheckAchievements,
		Autoreply,
		Delve
	};
	Q_ENUM(Action)

	/**
	 * Creates a new log scanner.
	 * @param[in] parent
	 *   The parent of the log scanner.
	 */
	PMessageHandler(QObject *parent=nullptr);

	/**
	 * Destructor.
	 */
	virtual ~PMessageHandler();

	/**
	 * Retrieves all log messages that have been loaded.
	 * @return
	 *   The list of all log messages.
	 */
	QList<PMessage *> GetLogMessages() const;

	/**
	 * Retrieves the list of all available actions.
	 * @return
	 *   The list of all available actions.
	 */
	Q_INVOKABLE QList<Action> GetAllActions() const;

	/**
	 * Retrieves the command for an action.
	 * @param[in] action
	 *   The action for which to retrieve the command.
	 * @return
	 *   The command for the action.
	 */
	Q_INVOKABLE QString GetCommandFromAction(Action action) const;

	/**
	 * Retrieves the action from a command.
	 * @param[in] command
	 *   The command for which to retrieve the action.
	 * @return
	 *   The corresponding action.
	 */
	Q_INVOKABLE Action GetActionFromCommand(const QString &command) const;

public slots:

	/**
	 * Sends a chat message to PoE if it is open.
	 * @param[in] channel
	 *   The channel to which to send the message.
	 * @param[in] message
	 *   The message to send.
	 * @param[in] target
	 *   The player to send the message to, if it is a whisper.
	 */
	void SendChatMessage(PMessage::Channel channel, const QString &message, const QString &target = QString());
	
	/**
	 * Executes an action.
	 * @param[in] action
	 *   The action to perform for the player.
	 * @param[in] args
	 *   The arguments to the action.
	 */
	void SendAction(Action action, const QString &args=QString());

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
	void NewMessage(PMessage *message);

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
	QQmlListProperty<PMessage> GetLogMessagesProperty() const;

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
	QList<PMessage *> _Messages;

	/**
	 * Indicates whether the log scanner has been initialized.
	 */
	bool _Initialized = false;

	/**
	 * The position of the first message loaded.
	 */
	qint64 _FirstMessagePos = -1;
};
