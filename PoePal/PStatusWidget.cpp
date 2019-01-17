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
#include "PStatusWidget.h"
#include "PApplication.h"
#include "PMessageHandler.h"
#include <QKeyEvent>

PStatusWidget::PStatusWidget(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);
	ui._StatusBtn->addAction(ui._AvailableAction);
	ui._StatusBtn->addAction(ui._AutoReplyAction);
	ui._StatusBtn->addAction(ui._AFKAction);
	ui._StatusBtn->addAction(ui._DNDAction);
	connect(ui._StatusBtn, &QToolButton::triggered, this, &PStatusWidget::OnActionTriggered);
	ui._StatusBtn->setDefaultAction(ui._AvailableAction);
	ui._AutoReplyEdit->installEventFilter(this);
	ui._AFKMsgEdit->installEventFilter(this);
	ui._DNDMsgEdit->installEventFilter(this);
	ui._PartyDescEdit->installEventFilter(this);
	ui._StatusMsgEdit->installEventFilter(this);
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto msgHandler = app->GetMessageHandler();
	Q_ASSERT(msgHandler);
	connect(msgHandler, &PMessageHandler::NewMessage, this, &PStatusWidget::OnNewMessage);
}

PStatusWidget::~PStatusWidget()
{
}

bool PStatusWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == ui._StatusMsgEdit || watched == ui._PartyDescEdit || watched == ui._AutoReplyEdit ||
		watched == ui._AFKMsgEdit || watched == ui._DNDMsgEdit)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->key() == Qt::Key_Return)
			{
				auto app = qobject_cast<PApplication *>(qApp);
				Q_ASSERT(app);
				auto msgHandler = app->GetMessageHandler();
				Q_ASSERT(msgHandler);
				if (watched == ui._StatusMsgEdit)
				{
					msgHandler->SendAction(PMessageHandler::Status, ui._StatusMsgEdit->text());
				}
				else if (watched == ui._AutoReplyEdit)
				{
					msgHandler->SendAction(PMessageHandler::Autoreply, ui._AutoReplyEdit->text());
				}
				else if (watched == ui._AFKMsgEdit)
				{
					msgHandler->SendAction(PMessageHandler::Afk, ui._AFKMsgEdit->text());
				}
				else if (watched == ui._DNDMsgEdit)
				{
					msgHandler->SendAction(PMessageHandler::Dnd, ui._DNDMsgEdit->text());
				}
				else msgHandler->SendAction(PMessageHandler::PartyDescription, ui._PartyDescEdit->text());
				return true;
			}
		}
	}
	return QDockWidget::eventFilter(watched, event);
}

void PStatusWidget::OnNewMessage(PMessage *msg)
{
	if (msg->GetSubtype() == PMessage::Chat) return;
	auto contents = msg->GetContents();
	if (contents.startsWith(tr("AFK mode is now ON.")))
	{
		auto quoteLoc = contents.indexOf("\"");
		ui._AFKMsgEdit->setText(contents.mid(quoteLoc + 1, contents.length() - quoteLoc - 2));
		if (ui._StatusBtn->defaultAction() != ui._DNDAction) ui._StatusBtn->setDefaultAction(ui._AFKAction);
	}
	else if (contents == tr("AFK mode is now OFF."))
	{
		if (ui._StatusBtn->defaultAction() == ui._AFKAction)
		{
			ui._StatusBtn->setDefaultAction(ui._AvailableAction);
		}
	}
	else if (contents.startsWith(tr("Autoreply set")))
	{
		auto quoteLoc = contents.indexOf("\"");
		ui._AutoReplyEdit->setText(contents.mid(quoteLoc + 1, contents.length() - quoteLoc - 2));
		if (ui._StatusBtn->defaultAction() != ui._DNDAction) ui._StatusBtn->setDefaultAction(ui._AutoReplyAction);
	}
	else if (contents.startsWith(tr("You have entered")))
	{
		if (ui._StatusBtn->defaultAction() == ui._AutoReplyAction)
		{
			ui._StatusBtn->setDefaultAction(ui._AvailableAction);
		}
	}
	else if (contents.startsWith(tr("DND mode is now ON.")))
	{
		auto quoteLoc = contents.indexOf("\"");
		ui._DNDMsgEdit->setText(contents.mid(quoteLoc + 1, contents.length() - quoteLoc - 2));
		ui._StatusBtn->setDefaultAction(ui._DNDAction);
	}
	else if (contents.startsWith(tr("DND mode is now OFF.")))
	{
		if (ui._StatusBtn->defaultAction() == ui._DNDAction)
		{
			ui._StatusBtn->setDefaultAction(ui._AvailableAction);
		}
	}
}

void PStatusWidget::OnActionTriggered(QAction *action)
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto msgHandler = app->GetMessageHandler();
	Q_ASSERT(msgHandler);
	if (action == ui._AvailableAction)
	{
		if (ui._StatusBtn->defaultAction() == ui._DNDAction) msgHandler->SendAction(PMessageHandler::Dnd);
		else if (ui._StatusBtn->defaultAction() == ui._AFKAction)
		{
			msgHandler->SendAction(PMessageHandler::Afkoff);
		}
	}
	else if (action == ui._AFKAction)
	{
		if (ui._StatusBtn->defaultAction() == ui._DNDAction) msgHandler->SendAction(PMessageHandler::Dnd);
		msgHandler->SendAction(PMessageHandler::Afk, ui._AFKMsgEdit->text());
	}
	else if (action == ui._DNDAction)
	{
		if (ui._StatusBtn->defaultAction() != ui._DNDAction) msgHandler->SendAction(PMessageHandler::Dnd);
	}
	else if (action == ui._AutoReplyAction)
	{
		msgHandler->SendAction(PMessageHandler::Autoreply, ui._AutoReplyEdit->text());
	}
}
