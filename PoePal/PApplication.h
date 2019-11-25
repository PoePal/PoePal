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

class PGlobalKeyBind;
class PGlobalKeyBindManager;
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
	Q_PROPERTY(PMainWindow* mainWindow READ GetMainWindow);

	/**
	 * The macro manager for the application.
	 */
	Q_PROPERTY(PGlobalKeyBindManager* macroManager READ GetKeyBindManager);

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
	 * Retrieves the network access manager.
	 * @return
	 *   The network access manager.
	 */
	QNetworkAccessManager * GetNetworkManager() const;

	/**
	 * Retrieves the key bind manager for the application.
	 * @return
	 *   The key bind manager.
	 */
	PGlobalKeyBindManager* GetKeyBindManager() const;

public slots:

	/**
	 * Shows the main options dialog.
	 */
	void ShowOptionsWindow();

signals:

	/**
	 * Signal sent when the update check is finished.
	 * @param[in] isUpdate
	 *   true if an update is available, false otherwise.
	 */
	void UpdateCheckFinished(bool isUpdate);

	/**
	 * Signal sent when options have been changed.
	 */
	void OptionsChanged();

private slots:

	/**
	 * Slot called when the timer is triggered to check the foreground window.
	 */
	void OnCheckForegroundWindow();

	/**
	 * Slot called when one of the built-in global key binds is triggered.
	 */
	void OnBuiltInKeyBindTriggered();

private:

	/**
	 * Registers a new global key bind.
	 * @param[in] id
	 *   The ID of the global key bind to register.
	 * @param[in] label
	 *   The label for the key bind.
	 * @param[in] toolTip
	 *   The tool tip that describes the key bind.
	 * @param[in] defaultKeySequence
	 *   The default key sequence for the global key bind.
	 * @param[in] func
	 *   The function to be called when the key bind is triggered.
	 */
	void RegisterKeyBind(const QByteArray &id, const QString &label, const QString &toolTip, 
		const QString &defaultKeySequence, const std::function<void()>& func);

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
	 * The network access manager.
	 */
	QNetworkAccessManager *_NetManager = nullptr;

	/**
	 * The macro manager.
	 */
	QScopedPointer<PGlobalKeyBindManager> _KeyBindMgr;

	/**
	 * Timer to check the foreground window.
	 */
	QTimer _ForegroundWindowTimer;

	/**
	 * The overlay bar widget.
	 */
	QPointer<POverlayBarWidget> _OverlayBarWidget;

	/**
	 * The hash of all built-in key binds that have been registered.
	 */
	QHash<QByteArray, PGlobalKeyBind*> _BuiltInKeyBinds;

	/**
	 * The functions associated to each built-in key bind.
	 */
	QHash<QByteArray, std::function<void()>> _BuiltInKeyBindFuncs;
};