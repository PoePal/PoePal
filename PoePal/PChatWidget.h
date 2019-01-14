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
#include <QDockWidget>
#include "ui_PChatWidget.h"

class QSettings;

/**
 * Dock widget that shows a chat channel.
 */
class PChatWidget : public QDockWidget
{
	Q_OBJECT

	/**
	 * The default channel displayed in the widget.
	 * If this is not InvalidChannel, the channels cannot be set on the widget.
	 */
	Q_PROPERTY(PLogMessage::Channel defaultchannel READ GetDefaultChannel)

	/**
	 * The channels shown in the widget.
	 */
	Q_PROPERTY(int channels READ GetChannels WRITE SetChannels)

	/**
	 * The entry widget.
	 */
	Q_PROPERTY(QPlainTextEdit *entryWidget READ GetEntryWidget)

public:
	
	/**
	 * Creates a new instance of the widget.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	PChatWidget(QWidget *parent = Q_NULLPTR);

	/**
	 * Creates a new instance of the widget for a default channel.
	 * @param[in] defaultChannel
	 *   The default channel.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	PChatWidget(PLogMessage::Channel defaultChannel, QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PChatWidget();

	/**
	 * Retrieves the default channel shown in the widget.
	 * @return
	 *   The default channel to show in the widget.
	 */
	PLogMessage::Channel GetDefaultChannel() const;

	/**
	 * Retrieves the list of channels shown in the widget.
	 * @return
	 *   The list of channels shown in the widget.
	 */
	PLogMessage::Channels GetChannels() const;

	/**
	 * Sets the list of channels shown in the widget.
	 * @param[in] channels
	 *   The new list of channels.
	 */
	void SetChannels(const PLogMessage::Channels &channels);

	/**
	 * Retrieves the entry widget.
	 * @return
	 *   The text editor used for entering text.
	 */
	QPlainTextEdit * GetEntryWidget() const;

	/**
	 * Loads the state of the widget from settings.
	 * @param[in] settings
	 *   The settings from which to load the state.
	 */
	void LoadState(const QSettings &settings);

	/**
	 * Saves the state of the widget to settings.
	 * @param[in] settings
	 *   The settings to which to save the state.
	 */
	void SaveState(QSettings &settings) const;

public slots:

	/**
	 * Submits the entered text.
	 */
	void Submit();

	/**
	 * Configures the chat widget.
	 */
	void Configure();

	/**
	 * Removes the chat widget.
	 * This is ignored if the chat widget is a default chat widget.
	 */
	void Remove();

	/**
	 * Sets the whisper target for the widget.
	 * @param[in] target
	 *   The whisper target for the widget.
	 */
	void SetWhisperTarget(const QString &target);

protected:

	/**
	 * Overrides QDockWidget#eventFilter.
	 */
	virtual bool eventFilter(QObject *watched, QEvent *evt) override;

private slots:

	/**
	 * Slot called when a new messages is received.
	 * @param[in] message
	 *   The new message.
	 */
	void OnNewMessage(PLogMessage *message);

	/**
	 * Slot called when the contents of the entry edit change.
	 */
	void OnEntryChanged();

	/**
	 * Slot called when a channel is selected.
	 */
	void OnChannelSelected();

	/**
	 * Slot called when the tab selection changes.
	 */
	void OnTabSelected();

	/**
	 * Slot called when a context menu is requested in the display edit.
	 * @param[in] pos
	 *   The position where the context menu was requested.
	 */
	void OnContextMenuRequested(const QPoint &pos);

	/**
	 * Slot called when a contextual menu action is triggered.
	 * @param[in] action
	 *   The action that was triggered.
	 */
	void OnContextMenuTriggered(QAction *action);

private:

	/**
	 * Sets channels.
	 * @param[in] channels
	 *   The new channels.
	 */
	void SetChannels(int channels);

	/**
	 * Checks whether the given message matches the filter.
	 * @param[in] message
	 *   The message to check.
	 * @return
	 *   true if the message matches the filter, false otherwise.
	 */
	virtual bool CheckMessage(PLogMessage *message);
	
	/**
	 * Prepends 100 messages to the display.
	 */
	void PrependMessages();

	/**
	 * Formats the given message.
	 * @param[in] message
	 *   The message to format.
	 * @return
	 *   The formatted message.
	 */
	QString FormatMessage(PLogMessage *message) const;

	/**
	 * Initializes the widget.
	 */
	void Initialize();

	/**
	 * Updates the display to account for new channels.
	 */
	void UpdateForChannels();

	/**
	 * Retrieves the current channel.
	 * @return
	 *   The current selected channel.
	 */
	PLogMessage::Channel GetCurrentChannel() const;

	/**
	 * Sets the current channel.
	 * @param[in] channel
	 *   The current channel.
	 */
	void SetCurrentChannel(PLogMessage::Channel channel);

	/**
	 * Retrieves the prefix for the current channel.
	 */
	QChar GetCurrentChannelPrefix() const;

	/**
	 * Sets the current channel to the first channel.
	 */
	void SetChannelToFirst();

	/**
	 * Updates the prefix in the entry field to accommodate the current channel.
	 */
	void UpdatePrefixForChannel();

	/**
	 * Updates the current channel to accommodate the prefix.
	 */
	void UpdateChannelForPrefix();

	/**
	 * The default channel displayed in the widget.
	 */
	PLogMessage::Channel _DefaultChannel = PLogMessage::InvalidChannel;

	/**
	 * The chat channel displayed in the widget.
	 */
	PLogMessage::Channels _Channels;

	/**
	 * The index of the top message displayed in the widget.
	 */
	int _TopIdx = 0;

	/**
	 * The channel selection menu.
	 */
	QMenu *_ChannelMenu = nullptr;

	/**
	 * The context menu for text display.
	 */
	QMenu *_ContextMenu = nullptr;

	/**
	 * The action to invite a player into your party.
	 */
	QAction *_InviteAction = nullptr;

	/**
	 * The action to send a friend invitation to a player.
	 */
	QAction *_FriendAction = nullptr;

	/**
	 * The action to ignore a player.
	 */
	QAction *_IgnoreAction = nullptr;

	/**
	 * The action to get the player information for a player.
	 */
	QAction *_WhoisAction = nullptr;

	/**
	 * The action to copy the selected text.
	 */
	QAction *_CopyAction = nullptr;

	/**
	 * The action to whisper a player.
	 */
	QAction *_WhisperAction = nullptr;

	Ui::PChatWidget ui;
};
