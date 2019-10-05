/**
 * PoePal - A companion application to Path of Exile.
 * Copyright (c) 2019 Phillip Doup (https://github.com/douppc)
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

#include <QApplication>
#include <PApplicationUpdate.h>
#include <QPointer>
#include <QTimer>

class PChatSettings;
class PMessageModel;
class PMessageHandler;
class PMainWindow;
class POverlayBarWidget;
class QJSEngine;
class QNetworkAccessManager;

/**
 * The application for PoePal.
 */
class PApplication : public QApplication
{
	Q_OBJECT

	/**
	 * The message handler belonging to the application.
	 */
	Q_PROPERTY(PMessageHandler *messageHandler READ GetMessageHandler)

	/**
	 * The message model belonging to the application.
	 */
	Q_PROPERTY(PMessageModel *messageModel READ GetMessageModel);

	/**
	 * The main window of the application.
	 */
	Q_PROPERTY(PMainWindow *mainWindow READ GetMainWindow)

	/**
	 * The chat settings.
	 */
	Q_PROPERTY(PChatSettings *chatSettings READ GetChatSettings)
public:

	/**
	 * Creates a new instance of the application.
	 */
	PApplication(int &argc, char **argv);

	/**
	 * Destructor.
	 */
	~PApplication();

	/**
	 * Initializes the application.
	 */
	void Init();

	/**
	 * Retrieves the message handler.
	 * @return
	 *   The message handler.
	 */
	PMessageHandler * GetMessageHandler() const;

	/**
	 * Retrieves the message model.
	 * @return
	 *   The message model.
	 */
	PMessageModel * GetMessageModel() const;

	/**
	 * Retrieves the JS engine.
	 * @return
	 *   The JS engine for the application.
	 */
	QJSEngine * GetJSEngine() const;

	/**
	 * Retrieves the application's main window.
	 * @return 
	 *   The application's main window.
	 */
	PMainWindow * GetMainWindow() const;

	/**
	 * Retrieves the overlay bar widget.
	 * @return
	 *   The overlay bar widget.
	 */
	POverlayBarWidget * GetOverlayBarWidget() const;

	/**
	 * Retrieves the chat settings.
	 * @return
	 *   The chat settings.
	 */
	PChatSettings * GetChatSettings() const;

	/**
	 * Retrieves the network access manager.
	 * @return
	 *   The network access manager.
	 */
	QNetworkAccessManager * GetNetworkManager() const;

signals:

	/**
	 * Signal sent when the update check is finished.
	 * @param[in] isUpdate
	 *   true if an update is available, false otherwise.
	 */
	void UpdateCheckFinished(bool isUpdate);

private slots:

	/**
	 * Slot called when the timer is triggered to check the foreground window.
	 */
	void OnCheckForegroundWindow();

private:

	/**
	 * The message handler.
	 */
	PMessageHandler *_MessageHandler = nullptr;

	/**
	 * The message model.
	 */
	PMessageModel *_MessageModel = nullptr;

	/**
	 * The JS engine for the application.
	 */
	QJSEngine *_JSEngine = nullptr;

	/**
	 * The main window of the application.
	 */
	QPointer<PMainWindow> _MainWindow;

	/**
	 * The chat settings.
	 */
	PChatSettings *_ChatSettings = nullptr;

	/**
	 * The network access manager.
	 */
	QNetworkAccessManager *_Manager = nullptr;

	/**
	 * Timer to check the foreground window.
	 */
	QTimer _ForegroundWindowTimer;

	/**
	 * The overlay bar widget.
	 */
	QPointer<POverlayBarWidget> _OverlayBarWidget;
};