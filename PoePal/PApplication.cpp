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
#include "PMessage.h"
#include "PMessageModel.h"
#include "PMessageHandler.h"
#include "PMainWindow.h"
#include "windows.h"

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
	if (!_Manager) _Manager = new QNetworkAccessManager(this);
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

PChatSettings * PApplication::GetChatSettings() const
{
	return _ChatSettings;
}

QNetworkAccessManager * PApplication::GetNetworkManager() const
{
	return _Manager;
}
