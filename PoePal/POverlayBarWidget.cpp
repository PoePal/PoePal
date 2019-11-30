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
#include <QPropertyAnimation>
#include <QSettings>
#include "windows.h"

POverlayBarWidget::POverlayBarWidget(QWidget* parent /*= nullptr*/):
	QWidget(parent)
{
	setupUi(this);
	
	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
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

	_ChatWidget = new POverlayChatWidget();
	_ChatWidget->setStyleSheet(styleSheet());
	_ChatWidget->lower();

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

	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	settings.beginGroup(QStringLiteral("BarWidget"));
	if (settings.contains(QStringLiteral("Geometry")))
	{
		restoreGeometry(settings.value(QStringLiteral("Geometry")).toByteArray());
	}
	_ChatBtn->setChecked(settings.value(QStringLiteral("ChatVisible"), false).toBool());
	_LockedAtStart = settings.value(QStringLiteral("Locked")).toBool();
// 	if (_LockedAtStart) Lock();
	for (const auto& child : findChildren<QWidget*>()) child->installEventFilter(this);
	settings.endGroup(); // BarWidget
	settings.beginGroup(QStringLiteral("ChatWidget"));
	_ChatWidget->restoreGeometry(settings.value(QStringLiteral("Geometry")).toByteArray());
	settings.endGroup(); // ChatWidget
	settings.endGroup(); // Overlay
}

POverlayBarWidget::~POverlayBarWidget()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	settings.beginGroup(QStringLiteral("BarWidget"));
	settings.setValue(QStringLiteral("Geometry"), saveGeometry());
	settings.setValue(QStringLiteral("ChatVisible"), _ChatBtn->isChecked());
	settings.setValue(QStringLiteral("Locked"), IsLocked());
	settings.endGroup(); // BarWidget
	settings.beginGroup(QStringLiteral("ChatWidget"));
	settings.setValue(QStringLiteral("Geometry"), _ChatWidget->saveGeometry());
	settings.endGroup(); // ChatWidget
	settings.endGroup(); // Overlay
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
		newWindowPos.y() + origCornerPos.y() - newCornerPos.y());
	_LockAction->setVisible(false);
	_UnlockAction->setVisible(true);
	// Remove the event filter.
	for (const auto& child : findChildren<QWidget*>()) child->removeEventFilter(this);
	UpdateChatWindowVisibility();
}

void POverlayBarWidget::Unlock()
{
	if (!_Locked) return;
	_Locked = false;
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
	// Install an event filter on all of the child widgets. We'll use this to be able to move the widget 
	// around with keys.
	for (const auto& child : findChildren<QWidget*>()) child->installEventFilter(this);
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

void POverlayBarWidget::keyPressEvent(QKeyEvent* event)
{
	auto newPos = pos();
	auto key = event->key();
	switch (event->key())
	{
	case Qt::Key_Up:
		newPos.setY(newPos.y() - 1);
		break;
	case Qt::Key_Right:
		newPos.setX(newPos.x() + 1);
		break;
	case Qt::Key_Down:
		newPos.setY(newPos.y() + 1);
		break;
	case Qt::Key_Left:
		newPos.setX(newPos.x() - 1);
		break;
	}
	move(newPos);
	QWidget::keyPressEvent(event);
}

bool POverlayBarWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() != QEvent::KeyPress) return false;
	auto keyPressEvent = static_cast<QKeyEvent*>(event);
	auto newPos = pos();
	bool arrowKey = true;
	switch (keyPressEvent->key())
	{
	case Qt::Key_Up:
		newPos.setY(newPos.y() - 1);
		break;
	case Qt::Key_Right:
		newPos.setX(newPos.x() + 1);
		break;
	case Qt::Key_Down:
		newPos.setY(newPos.y() + 1);
		break;
	case Qt::Key_Left:
		newPos.setX(newPos.x() - 1);
		break;
	default:
		arrowKey = false;
		break;
	}
	if (arrowKey)
	{
		move(newPos);
		return true;
	}
	return false;
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
		if (_GameActive && _LockedAtStart)
		{
			QTimer::singleShot(10, this, &POverlayBarWidget::Lock);
			_LockedAtStart = false;
		}
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
		_ChatWidget->setVisible(true);
		if (_ChatWidget->IsLocked() != _Locked)
		{
			if (_Locked) QTimer::singleShot(10, _ChatWidget, &POverlayChatWidget::Lock);
			else QTimer::singleShot(10, _ChatWidget, &POverlayChatWidget::Unlock);
		}
		setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		raise();
	}
	else if(_ChatWidget) _ChatWidget->setVisible(false);
}
