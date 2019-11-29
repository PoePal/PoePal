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
#include "POverlayBarWidget.h"
#include "PApplication.h"
#include "PMessageHandler.h"
#include "POverlayChatWidget.h"
#include <QDebug>
#include <QPropertyAnimation>
#include "windows.h"

POverlayBarWidget::POverlayBarWidget(QWidget* parent /*= nullptr*/):
	QWidget(parent)
{
	setupUi(this);
	
	setWindowFlags(windowFlags() | Qt::Window | Qt::Tool | Qt::WindowStaysOnTopHint);
	setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

	QIcon lockIcon;
	lockIcon.addFile(QString::fromUtf8(":/PoePal/Resources/32x32/lock.png"), QSize(), QIcon::Normal,
		QIcon::Off);
	_LockAction = _ConfigMenu.addAction(lockIcon, tr("Lock"));
	QIcon unlockIcon;
	unlockIcon.addFile(QString::fromUtf8(":/PoePal/Resources/32x32/lock_open.png"), QSize(),
		QIcon::Normal, QIcon::Off);
	_UnlockAction = _ConfigMenu.addAction(unlockIcon, tr("Unlock"));
	_UnlockAction->setVisible(false);

	connect(_HideoutBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_MenagerieBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_DelveBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_ChatBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_TradeBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_OptionsBtn, &QPushButton::clicked, _OptionsBtn, &QToolButton::showMenu);
	connect(_LockAction, &QAction::triggered, this, &POverlayBarWidget::Lock);
	connect(_UnlockAction, &QAction::triggered, this, &POverlayBarWidget::Unlock);
	_OptionsBtn->setMenu(&_ConfigMenu);

	_CollapseAnimation = new QPropertyAnimation(this, "size");
	_CollapseAnimation->setDuration(100);

	connect(&_ForegroundWindowTimer, &QTimer::timeout, this, &POverlayBarWidget::OnCheckForegroundWindow);
	_ForegroundWindowTimer.setInterval(100);
	_ForegroundWindowTimer.start();
}

bool POverlayBarWidget::IsLocked() const
{
	return _Locked;
}

void POverlayBarWidget::Lock()
{
	if (_Locked) return;
	_Locked = true;
	if (_ChatWidget) _ChatWidget->Lock();
	auto origCornerPos = geometry().topLeft();
	auto origWindowPos = pos();
	hide();
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	show();
	auto newCornerPos = geometry().topLeft();
	auto newWindowPos = pos();
	move(newWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		newWindowPos.y() + origCornerPos.y() - newCornerPos.y() + newWindowPos.y() - origWindowPos.y());
	move(newWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		newWindowPos.y() + origCornerPos.y() - newCornerPos.y());
	_LockAction->setVisible(false);
	_UnlockAction->setVisible(true);
	UpdateChatWindowVisibility();
}

void POverlayBarWidget::Unlock()
{
	if (!_Locked) return;
	_Locked = false;
	if (_ChatWidget) _ChatWidget->Unlock();
	auto origCornerPos = geometry().topLeft();
	auto origWindowPos = pos();
	hide();
	setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
	show();
	auto newCornerPos = geometry().topLeft();
	move(origWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		origWindowPos.y() + origCornerPos.y() - newCornerPos.y());
	_LockAction->setVisible(true);
	_UnlockAction->setVisible(false);
	UpdateChatWindowVisibility();
}

void POverlayBarWidget::SetLocked(bool locked)
{
	if (locked) Lock();
	else Unlock();
}

void POverlayBarWidget::ToggleLock()
{
	SetLocked(!_Locked);
}

void POverlayBarWidget::enterEvent(QEvent* evt)
{
	auto collapseSize = _CollapseArea->sizeHint();
	auto fullSize = sizeHint();
	fullSize.setWidth(fullSize.width() + collapseSize.width());
	_CollapseAnimation->setStartValue(size());
	_CollapseAnimation->setEndValue(fullSize);
	_CollapseAnimation->start();
}

void POverlayBarWidget::leaveEvent(QEvent* evt)
{
	auto collapseSize = _CollapseArea->sizeHint();
	auto fullSize = sizeHint();
	_CollapseAnimation->setStartValue(size());
	_CollapseAnimation->setEndValue(fullSize);
	_CollapseAnimation->start();
}

void POverlayBarWidget::OnButtonClicked()
{
	auto button = qobject_cast<QAbstractButton*>(sender());
	Q_ASSERT(button);
	if (!button) return;
	auto app = qobject_cast<PApplication*>(qApp);
	Q_ASSERT(app);
	auto handler = app->GetMessageHandler();
	Q_ASSERT(handler);
	if (button == _HideoutBtn) handler->SendAction(PMessageHandler::Hideout, QString(), false);
	else if (button == _MenagerieBtn) handler->SendAction(PMessageHandler::Menagerie, QString(), false);
	else if (button == _DelveBtn) handler->SendAction(PMessageHandler::Delve, QString(), false);
	else if (button == _OptionsBtn) _ConfigMenu.popup(QCursor::pos());
	else if (button == _ChatBtn) UpdateChatWindowVisibility();
}

void POverlayBarWidget::OnCheckForegroundWindow()
{
	auto windowHwnd = GetForegroundWindow();
	if (!windowHwnd) return;
	WCHAR title[33];
	title[32] = '\0]';
	auto len = GetWindowTextW(windowHwnd, title, 32);
	bool show = false;
	auto titleStr = QString::fromStdWString(title);
	titleStr.remove(" - PoePal");
	bool oldActive = _GameActive;
	_GameActive = titleStr == L"Path of Exile" || titleStr == windowTitle() ||
		(_ChatWidget && titleStr == _ChatWidget->windowTitle());
	if (oldActive != _GameActive)
	{
		setVisible(_GameActive);
		UpdateChatWindowVisibility();
	}
}

void POverlayBarWidget::UpdateChatWindowVisibility()
{
	// Determine whether we should show it. We want to show it if the overlay is unlocked or the chat button
	// is toggled on.
	bool show = (!IsLocked() || _ChatBtn->isChecked()) && _GameActive;
	if (show)
	{
		if (!_ChatWidget)
		{
			_ChatWidget = new POverlayChatWidget();
			_ChatWidget->setStyleSheet(styleSheet());
			_ChatWidget->lower();
		}
		_ChatWidget->setVisible(true);
		_ChatWidget->SetLocked(_Locked);
		setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		raise();
	}
	else if(_ChatWidget) _ChatWidget->setVisible(false);
}
