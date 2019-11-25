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
#include "PMainWindow.h"
#include "PApplication.h"
#include "PApplicationUpdateWidget.h"
#include "PChatDockWidget.h"
#include "PChatWidget.h"
#include "PChatWidgetsDlg.h"
#include "PJSConsoleWidget.h"
#include "PLogWidget.h"
#include "PMainOptionsDlg.h"
#include "PMessageHandler.h"
#include "PPassivesWindow.h"
#include "PStatusWidget.h"
#include <QJsonDocument>
#include <QMenu>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QTimer>
#include <QToolButton>

PMainWindow::PMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setupUi(this);

	auto app = qobject_cast<PApplication*>(qApp);
	Q_ASSERT(app);

	// Create the hard-coded dock widgets.
	InitializeDockWidget(_LogWidget = new PLogWidget(this), Qt::BottomDockWidgetArea, _MessagesAction);
	InitializeDockWidget(_JSConsole = new PJSConsoleWidget(this), Qt::BottomDockWidgetArea, 
		_ConsoleAction);
	InitializeDockWidget(_StatusWidget = new PStatusWidget(this), Qt::LeftDockWidgetArea, _StatusAction);
	QSettings settings;
	settings.beginGroup(QStringLiteral("Chat"));
	settings.beginGroup(QStringLiteral("DefaultWidgets"));
	InitializeDefaultChatWidget(PMessage::Global, _GlobalAction, settings);
	InitializeDefaultChatWidget(PMessage::Trade, _TradeAction, settings);
	InitializeDefaultChatWidget(PMessage::Guild, _GuildAction, settings);
	InitializeDefaultChatWidget(PMessage::Party, _PartyAction, settings);
	InitializeDefaultChatWidget(PMessage::Local, _LocalAction, settings);
	InitializeDefaultChatWidget(PMessage::Whisper, _WhisperAction, settings);
	settings.endGroup(); // DefaultWidgets
	// Restore the custom chat widgets.
	_ChatDropDown = new QToolButton(_ChatToolbar);
	_ChatDropDown->setPopupMode(QToolButton::DelayedPopup);
	_ChatDropDown->setIcon(QIcon(":/PoePal/Resources/32x32/show_comment.png"));
	auto chatEditAction = _ChatToolbar->addWidget(_ChatDropDown);
	settings.beginGroup(QStringLiteral("CustomWidgets"));
	for (const auto &objName : settings.childGroups())
	{
		auto widget = CreateChatWidget(objName);
		settings.beginGroup(objName);
		InitializeCustomChatWidget(widget);
		widget->LoadState(settings);
		_CustomChatWidgets.append(widget);
		settings.endGroup();
	}
	settings.endGroup(); // CustomWidgets
	settings.endGroup(); // Chat

	_HideoutAction->setProperty("action", QVariant::fromValue(PMessageHandler::Hideout));
	_MenagerieAction->setProperty("action", QVariant::fromValue(PMessageHandler::Menagerie));
	_RemainingAction->setProperty("action", QVariant::fromValue(PMessageHandler::Remaining));
	_PassivesAction->setProperty("action", QVariant::fromValue(PMessageHandler::Passives));
	_DelveAction->setProperty("action", QVariant::fromValue(PMessageHandler::Delve));

	connect(_HideoutAction, &QAction::triggered, this, &PMainWindow::OnMacroTriggered);
	connect(_MenagerieAction, &QAction::triggered, this, &PMainWindow::OnMacroTriggered);
	connect(_RemainingAction, &QAction::triggered, this, &PMainWindow::OnMacroTriggered);
	connect(_PassivesAction, &QAction::triggered, this, &PMainWindow::OnMacroTriggered);
	connect(_DelveAction, &QAction::triggered, this, &PMainWindow::OnMacroTriggered);
	connect(_PassivesAction, &QAction::triggered, this, &PMainWindow::ShowPassivesWindow);

	connect(_OptionsAction, &QAction::triggered, app, &PApplication::ShowOptionsWindow);
	connect(app, &PApplication::OptionsChanged, this, &PMainWindow::OnOptionsChanged);

	connect(_ChatDropDown, &QToolButton::clicked, this, &PMainWindow::ConfigureCustomChatWidgets);
	connect(_UpdateAction, &QAction::triggered, this, &PMainWindow::CheckForUpdates);

	QTimer::singleShot(1, this, &PMainWindow::CheckForUpdates);

	restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
	restoreState(settings.value(QStringLiteral("state")).toByteArray());

	OnOptionsChanged();
}

PMainWindow::~PMainWindow()
{
}

QList<PChatDockWidget *> PMainWindow::GetCustomChatWidgets() const
{
	return _CustomChatWidgets;
}

PChatDockWidget * PMainWindow::CreateCustomChatWidget()
{
	QStringList existingNames;
	for (const auto &existing : _CustomChatWidgets) existingNames.append(existing->objectName());
	int i = 0;
	QString name("CustomChat%1");
	while (++i > 0)
	{
		if (!existingNames.contains(name.arg(i))) break;
	}
	auto widget = CreateChatWidget(name.arg(i));
	widget->setWindowTitle(tr("Custom Chat Widget %1").arg(i));
	_CustomChatWidgets.append(widget);
	InitializeCustomChatWidget(widget);
	return widget;
}

PChatDockWidget * PMainWindow::GetDefaultChatWidget(PMessage::Channel channel) const
{
	return _DefaultChatWidgets.value(channel, nullptr);
}

void PMainWindow::RemoveCustomChatWidget(PChatDockWidget *widget)
{
	_CustomChatWidgets.removeAll(widget);
	auto action = widget->property("associatedAction").value<QAction *>();
	if (action) action->deleteLater();
	if (_CustomChatWidgets.isEmpty())
	{
		_CustomChatMenu->deleteLater();
		_CustomChatMenu = nullptr;
		_ChatDropDown->setPopupMode(QToolButton::DelayedPopup);
		_ChatDropDown->update();
	}
	widget->deleteLater();
}

void PMainWindow::ConfigureCustomChatWidgets()
{
	PChatWidgetsDlg::LaunchModal(this);
}

void PMainWindow::CheckForUpdates()
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto reply = app->GetNetworkManager()->get(QNetworkRequest(
		QUrl("https://api.github.com/repos/PoePal/PoePal/releases/latest")));
	connect(reply, &QNetworkReply::finished, this, &PMainWindow::OnUpdateRequestFinished);
}

void PMainWindow::Whisper(const QString &player)
{
	auto widget = _DefaultChatWidgets.value(PMessage::Whisper, nullptr);
	if (widget)
	{
		_WhisperAction->setChecked(true);
		widget->raise();
		widget->GetChatWidget()->SetWhisperTarget(player);
	}
}

void PMainWindow::ShowPassivesWindow()
{
	bool found = false;
	for (const auto &window : _MDI->subWindowList())
	{
		auto passivesWindow = qobject_cast<PPassivesWindow *>(window);
		if (!passivesWindow) continue;
		_MDI->setActiveSubWindow(passivesWindow);
		passivesWindow->show();
		found = true;
		break;
	}
	if (!found)
	{
		auto newWin = new PPassivesWindow(_MDI);
		_MDI->addSubWindow(newWin);
		newWin->show();
	}
}

void PMainWindow::closeEvent(QCloseEvent *evt)
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("Chat"));
	settings.beginGroup(QStringLiteral("DefaultWidgets"));
	for (const auto &chatWidget : _DefaultChatWidgets.values())
	{
		settings.beginGroup(PMessage::GetStringFromChannel(chatWidget->GetChatWidget()->GetDefaultChannel()));
		chatWidget->SaveState(settings);
		settings.endGroup();
	}
	settings.endGroup(); // DefaultWidgets
	settings.remove(QStringLiteral("CustomWidgets"));
	settings.beginGroup(QStringLiteral("CustomWidgets"));
	for (const auto &customWidget : _CustomChatWidgets)
	{
		settings.beginGroup(customWidget->objectName());
		customWidget->SaveState(settings);
		settings.endGroup();
	}
	settings.endGroup(); // CustomWidgets
	settings.endGroup(); // Chat
	settings.setValue(QStringLiteral("geometry"), saveGeometry());
	settings.setValue(QStringLiteral("state"), saveState());
	QMainWindow::closeEvent(evt);
}

bool PMainWindow::eventFilter(QObject *watched, QEvent *evt)
{
	if (evt->type() == QEvent::Close)
	{
		auto action = watched->property("associatedAction").value<QAction *>();
		if (action) action->setChecked(false);
	}
	else if (evt->type() == QEvent::Show)
	{
		auto action = watched->property("associatedAction").value<QAction *>();
		if (action) action->setChecked(true);
	}
	return QMainWindow::eventFilter(watched, evt);
}

void PMainWindow::OnUpdateRequestFinished()
{
	auto reply = qobject_cast<QNetworkReply *>(sender());
	auto url = reply->url().toString();
	auto err = reply->error();
	auto data = reply->readAll();
	auto jsonData = QJsonDocument::fromJson(data);
	PApplicationUpdate update(jsonData.object());
	reply->deleteLater();
	if (!update.IsNewer())
	{
		if (!_FirstUpdateCheck)
		{
			QMessageBox::information(this, tr("Up-to-date"), tr("PoePal is already up-to-date."));
		}
		_FirstUpdateCheck = false;
		return;
	}
	else
	{
		auto dlg = new PApplicationUpdateWidget(update, this);
		dlg->setWindowModality(Qt::ApplicationModal);
		dlg->show();
	}
}

void PMainWindow::OnMacroTriggered()
{
	auto action = qobject_cast<QAction *>(sender());
	Q_ASSERT(action);
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto handler = app->GetMessageHandler();
	Q_ASSERT(handler);
	handler->SendAction(action->property("action").value<PMessageHandler::Action>());
}

void PMainWindow::OnOptionsChanged()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("MainWindow"));
	auto btnStyle = settings.value(QStringLiteral("ToolbarDisplayMode"), Qt::ToolButtonIconOnly)
		.value<Qt::ToolButtonStyle>();
	_ChatToolbar->setToolButtonStyle(btnStyle);
	_MacroToolbar->setToolButtonStyle(btnStyle);
	_SystemToolbar->setToolButtonStyle(btnStyle);
	settings.endGroup(); // MainWindow
}

QQmlListProperty<PChatDockWidget> PMainWindow::GetCustomChatWidgetsProperty() const
{
	static QQmlListProperty<PChatDockWidget>::CountFunction countFunc = 
		[](QQmlListProperty<PChatDockWidget> *prop)->int
	{
		auto mainWin = qobject_cast<PMainWindow *>(prop->object);
		Q_ASSERT(mainWin);
		return mainWin->GetCustomChatWidgets().length();
	};
	static QQmlListProperty<PChatDockWidget>::AtFunction atFunc =
		[](QQmlListProperty<PChatDockWidget> *prop, int index)->PChatDockWidget*
	{
		auto mainWin = qobject_cast<PMainWindow *>(prop->object);
		Q_ASSERT(mainWin);
		return mainWin->GetCustomChatWidgets().at(index);
	};
	return QQmlListProperty<PChatDockWidget>(const_cast<PMainWindow *>(this), nullptr, countFunc, atFunc);
}

void PMainWindow::InitializeDockWidget(QDockWidget *widget, Qt::DockWidgetArea dockLocation, QAction *action)
{
	connect(action, &QAction::toggled, widget, &QDockWidget::setVisible);
	widget->installEventFilter(this);
	addDockWidget(dockLocation, widget);
	widget->setProperty("associatedAction", QVariant::fromValue(action));
}

void PMainWindow::InitializeDefaultChatWidget(PMessage::Channel channel, QAction *action,
	QSettings &settings)
{
	auto widget = new PChatDockWidget(channel, this);
	widget->setObjectName(PMessage::GetStringFromChannel(channel));
	_DefaultChatWidgets.insert(channel, widget);
	InitializeDockWidget(widget, Qt::LeftDockWidgetArea, action);
	settings.beginGroup(PMessage::GetStringFromChannel(channel));
	widget->LoadState(settings);
	settings.endGroup();
}

void PMainWindow::InitializeCustomChatWidget(PChatDockWidget*widget)
{
	if (!_CustomChatMenu)
	{
		_ChatDropDown->setPopupMode(QToolButton::MenuButtonPopup);
		_CustomChatMenu = new QMenu(_ChatDropDown);
		_ChatDropDown->setMenu(_CustomChatMenu);
	}
	auto action = _CustomChatMenu->addAction(widget->windowTitle());
	action->setCheckable(true);
	connect(widget, &QWidget::windowTitleChanged, action, &QAction::setText);
	InitializeDockWidget(widget, Qt::LeftDockWidgetArea, action);
}

PChatDockWidget* PMainWindow::CreateChatWidget(const QString &objName)
{
	auto newWidget = new PChatDockWidget(this);
	newWidget->setObjectName(objName);
	addDockWidget(Qt::LeftDockWidgetArea, newWidget);
	return newWidget;
}
