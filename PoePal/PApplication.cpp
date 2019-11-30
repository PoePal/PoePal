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
#include "PApplication.h"
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QQmlEngine>
#include "PApplicationUpdate.h"
#include "PChatWidget.h"
#include "PGlobalKeyBind.h"
#include "PGlobalKeyBindManager.h"
#include "PMessage.h"
#include "PMessageModel.h"
#include "PMessageHandler.h"
#include "PMainOptionsDlg.h"
#include "PMainWindow.h"
#include "POverlayController.h"
#include "windows.h"

#define HIDEOUT_KEY_BIND "hideout"
#define MENAGERIE_KEY_BIND "menagerie"
#define DELVE_KEY_BIND "delve"
#define REMAINING_KEY_BIND "remaining"
#define PASSIVES_KEY_BIND "passives"

namespace {

	/**
	 * Handles a key bind that triggers a message.
	 * @param[in] action
	 *   The action to be sent.
	 */
	template<PMessageHandler::Action action>
	void HandleChatKeyBind()
	{
		auto app = qobject_cast<PApplication*>(qApp);
		Q_ASSERT(app);
		if (!app) return;
		auto handler = app->GetMessageHandler();
		Q_ASSERT(handler);
		if (!handler) return;
		handler->SendAction(action);
	}
}

QJSValue ChannelsToValue(PMessage::Channels channels)
{
	return QJSValue(static_cast<int>(channels));
}

#define P_REGISTER_JS_OBJECT(className) \
qmlRegisterType<className>("PoePal", 1, 0, #className); \
RegisterJSClass<className>();

namespace {
	/**
	 * Registers a class with javascript.
	 */
	template<typename T>
	void RegisterJSClass()
	{
		auto app = qobject_cast<PApplication *>(qApp);
		Q_ASSERT(app);
		auto engine = app->GetJSEngine();
		// Register each of the enumerations as <class>.<enumeration>.<value>
		auto classObj = engine->newObject();
		int numEnum = T::staticMetaObject.enumeratorCount();
		for (int e = 0; e < numEnum; ++e)
		{
			auto metaEnum = T::staticMetaObject.enumerator(e);
			int numKeys = metaEnum.keyCount();
			for (int k = 0; k < numKeys; ++k)
			{
				classObj.setProperty(metaEnum.key(k), metaEnum.keyToValue(metaEnum.key(k)));
			}
		}
		engine->globalObject().setProperty(T::staticMetaObject.className(), classObj);
	}

	/**
	 * Retrieves the version of the product.
	 */
	QString GetProductVersion()
	{
		// get the filename of the executable containing the version resource
		TCHAR szFilename[MAX_PATH + 1] = { 0 };
		if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
		{
			qDebug() << "GetModuleFileName failed with error " << GetLastError();
			return QString();
		}

		// allocate a block of memory for the version info
		DWORD dummy;
		DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
		if (dwSize == 0)
		{
			qDebug() << "GetFileVersionInfoSize failed with error " << GetLastError();
			return false;
		}
		std::vector<BYTE> data(dwSize);

		// load the version info
		if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0]))
		{
			qDebug() << "GetFileVersionInfo failed with error " << GetLastError();
			return false;
		}
		UINT                uiVerLen = 0;
		VS_FIXEDFILEINFO*   pFixedInfo = 0;     // pointer to fixed file info structure
		// get the fixed file info (language-independent) 
		if (VerQueryValue(&data[0], TEXT("\\"), (void**)&pFixedInfo, (UINT *)&uiVerLen) == 0)
		{
			return false;
		}

		QString version("%1.%2.%3");
		return version.arg(HIWORD(pFixedInfo->dwProductVersionMS))
			.arg(LOWORD(pFixedInfo->dwProductVersionMS))
			.arg(HIWORD(pFixedInfo->dwProductVersionLS));
	}
}

PApplication::PApplication(int &argc, char **argv) :
	QApplication(argc, argv)
{
	setApplicationDisplayName("PoePal");
	setApplicationName("PoePal");
	setOrganizationName("PoePal");
	setApplicationVersion(GetProductVersion());
}

PApplication::~PApplication()
{
}

void PApplication::Init()
{
	if (!_KeyBindMgr)
	{
		_KeyBindMgr.reset(new PGlobalKeyBindManager());
		RegisterKeyBind(HIDEOUT_KEY_BIND, tr("Hideout"), tr("Travel to hideout"), "CTRL+SHIFT+H",
			HandleChatKeyBind<PMessageHandler::Hideout>);
		RegisterKeyBind(MENAGERIE_KEY_BIND, tr("Menagerie"), tr("Travel to menagerie"), "CTRL+SHIFT+M",
			HandleChatKeyBind<PMessageHandler::Menagerie>);
		RegisterKeyBind(DELVE_KEY_BIND, tr("Mine Encampment"), tr("Travel to the mine encampment"),
			"CTRL+SHIFT+J", HandleChatKeyBind<PMessageHandler::Delve>);
		RegisterKeyBind(REMAINING_KEY_BIND, tr("Remaining Monsters"),
			tr("Show the monsters remaining in the area"), "CTRL+SHIFT+N",
			HandleChatKeyBind<PMessageHandler::Remaining>);
		RegisterKeyBind(PASSIVES_KEY_BIND, tr("View Passives"),
			tr("Show the passive skills that have been acquired"), "CTRL+SHIFT+P",
			HandleChatKeyBind<PMessageHandler::Passives>);
		_KeyBindMgr->RestoreKeyBinds();
	}
	if (!_NetManager) _NetManager = new QNetworkAccessManager(this);
	if (!_JSEngine)
	{
		_JSEngine = new QQmlEngine(this);
		_JSEngine->globalObject().setProperty("application", _JSEngine->newQObject(this));
	}
	if (!_MessageHandler)_MessageHandler = new PMessageHandler(this);
	if (!_MessageModel) _MessageModel = new PMessageModel(this); // This must happen after _LogScanner
	if (!_MainWindow)
	{
		_MainWindow = new PMainWindow();
		_MainWindow->show();
	}
	if (!_OverlayController)
	{
		_OverlayController = new POverlayController(this);
		_OverlayController->Initialize();
	}
	qRegisterMetaType<PMessage::Channel>("PLogMessage::Channel");
	qRegisterMetaType<PMessage::Channels>("PLogMessage::Channels");
	QMetaType::registerConverter<PMessage::Channel, QJSValue>(ChannelsToValue);
	P_REGISTER_JS_OBJECT(PChatWidget);
	P_REGISTER_JS_OBJECT(PMessageHandler);
	P_REGISTER_JS_OBJECT(PMessage);
	P_REGISTER_JS_OBJECT(PMainWindow);
}

PMessageHandler * PApplication::GetMessageHandler() const
{
	return _MessageHandler;
}

PMessageModel * PApplication::GetMessageModel() const
{
	return _MessageModel;
}

QJSEngine * PApplication::GetJSEngine() const
{
	return _JSEngine;
}

PMainWindow * PApplication::GetMainWindow() const
{
	return _MainWindow;
}

POverlayController* PApplication::GetOverlayController() const
{
	return _OverlayController.data();
}

QNetworkAccessManager * PApplication::GetNetworkManager() const
{
	return _NetManager;
}

PGlobalKeyBindManager* PApplication::GetKeyBindManager() const
{
	return _KeyBindMgr.data();
}

void PApplication::ShowOptionsWindow()
{
	PMainOptionsDlg dlg(_MainWindow);
	auto result = dlg.exec();
	if (result == QDialog::Accepted) emit OptionsChanged();
}

void PApplication::OnBuiltInKeyBindTriggered()
{
	auto keyBind = qobject_cast<PGlobalKeyBind*>(sender());
	Q_ASSERT(keyBind);
	if (!keyBind) return;
	auto func = _BuiltInKeyBindFuncs.value(keyBind->GetId(), nullptr);
	Q_ASSERT(func);
	if (!func) return;
	func();
}

void PApplication::RegisterKeyBind(const QByteArray& id, const QString& label, const QString& toolTip, 
	const QString& defaultKeySequence, const std::function<void()>& func)
{
	auto keyBind = _KeyBindMgr->RegisterBuiltInKeyBind(id, label, toolTip, defaultKeySequence);
	Q_ASSERT(keyBind);
	if (!keyBind) return;
	_BuiltInKeyBinds.insert(id, keyBind);
	_BuiltInKeyBindFuncs.insert(id, func);
	connect(keyBind, &PGlobalKeyBind::Triggered, this, &PApplication::OnBuiltInKeyBindTriggered);
}
