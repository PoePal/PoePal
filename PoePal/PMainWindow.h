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

#include <QtWidgets/QMainWindow>
#include "ui_PMainWindow.h"
#include <QQmlListProperty>
#include "PMessage.h"

class PChatWidget;
class PJSConsoleWidget;
class PLogWidget;
class QActionGroup;
class QMenu;
class QMessageBox;
class QSettings;
class QToolButton;

/**
 * Provides the main application window of the PoePal application.
 */
class PMainWindow : public QMainWindow
{
	Q_OBJECT

	/**
	 * The custom chat widgets.
	 */
	Q_PROPERTY(QQmlListProperty<PChatWidget> customChatWidgets READ GetCustomChatWidgetsProperty)

public:

	/**
	 * Creates a new instance of this window.
	 * @param[in] parent
	 *   The parent of the window.
	 */
	PMainWindow(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PMainWindow();

	/**
	 * Retrieves the list of custom chat widgets.
	 * @return
	 *   The list of custom chat widgets.
	 */
	QList<PChatWidget *> GetCustomChatWidgets() const;

	/**
	 * Creates a new custom chat widget.
	 * @return
	 *   The new custom chat widget.
	 */
	Q_INVOKABLE PChatWidget * CreateCustomChatWidget();

	/**
	 * Retrieves the default chat widget for a channel.
	 * @param[in] channel
	 *   The channel for which to retrieve the default chat widget.
	 * @return
	 *   The default chat widget for the channel.
	 */
	Q_INVOKABLE PChatWidget * GetDefaultChatWidget(PMessage::Channel channel) const;

	/**
	 * Removes the given custom chat widget.
	 * @param[in] widget
	 *   The custom chat widget to remove.
	 */
	void RemoveCustomChatWidget(PChatWidget *widget);

public slots:

	/**
	 * Launches the custom chat configuration.
	 */
	void ConfigureCustomChatWidgets();

	/**
	 * Checks for updates.
	 */
	void CheckForUpdates();

	/**
	 * Whispers a given player.
	 * This ensures that the whisper dock widget is shown and sets the entry field to that player.
	 * @param[in] player
	 *   The player to whisper.
	 */
	void Whisper(const QString &player);

protected:

	/**
	 * Overrides QMainWindow#closeEvent.
	 */
	virtual void closeEvent(QCloseEvent *evt) override;

	/**
	 * Overrides QMainWindow#filterEvent.
	 */
	virtual bool eventFilter(QObject *watched, QEvent *evt) override;

private slots:

	/**
	 * Slot called when the update check request finishes.
	 */
	void OnUpdateRequestFinished();

	/**
	 * Slot called when one of the macro buttons is pressed.
	 */
	void OnMacroTriggered();

private:
	Ui::PMainWindow ui;

	/**
	 * Retrieves the property for custom chat widgets.
	 * @return
	 *   The custom chat widgets list property.
	 */
	QQmlListProperty<PChatWidget> GetCustomChatWidgetsProperty() const;

	/**
	 * Initializes a dock widget.
	 * @param[in] widget
	 *   The new dock widget.
	 * @param[in] dockLocation
	 *   The location to dock the widget.
	 * @param[in] action
	 *   The action associated to the widget.
	 */
	void InitializeDockWidget(QDockWidget *widget, Qt::DockWidgetArea dockLocation, QAction *action);

	/**
	 * Initializes a default chat widget.
	 * @param[in] channel
	 *   The chat channel for the widget.
	 * @param[in] action
	 *   The action to associate with the widget.
	 * @param[in] settings
	 *   The settings from which to load the widget state.
	 */
	void InitializeDefaultChatWidget(PMessage::Channel channel, QAction *action, QSettings &settings);

	/**
	 * Initializes a custom chat widget.
	 * @param[in] widget
	 *   The custom chat widget.
	 */
	void InitializeCustomChatWidget(PChatWidget *widget);

	/**
	 * Creates a chat widget.
	 * @param[in] objName
	 *   The new object name for the chat widget.
	 * @return
	 *   The new dock widget.
	 */
	PChatWidget * CreateChatWidget(const QString &objName);

	/**
	 * The log dock widget.
	 */
	PLogWidget *_LogWidget = nullptr;

	/**
	 * The default channels.
	 */
	QHash<PMessage::Channel, PChatWidget *> _DefaultChatWidgets;

	/**
	 * The list of custom chat widgets.
	 */
	QList<PChatWidget *> _CustomChatWidgets;

	/**
	 * The JS Console widget.
	 */
	PJSConsoleWidget *_JSConsole = nullptr;

	/**
	 * The drop-down button for custom chat windows.
	 */
	QToolButton *_ChatDropDown = nullptr;

	/**
	 * The menu for custom chat windows.
	 */
	QMenu *_CustomChatMenu = nullptr;

	/**
	 * The chat widgets action group.
	 */
	QActionGroup *_ChatWidgetsActionGroup = nullptr;

	/**
	 * The wait message box.
	 */
	QMessageBox *_WaitBox = nullptr;

	/**
	 * Indicates whether or not the first update check has finished.
	 */
	bool _FirstUpdateCheck = true;
};
