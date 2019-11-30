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
#include "POverlayController.h"
#include "POverlayBarWidget.h"
#include "POverlayChatWidget.h"
#include <QScreen>
#include <QSettings>
#include "windows.h"

POverlayController::POverlayController(QObject* parent):
QObject(parent)
{

}

POverlayController::~POverlayController()
{
	if (_BarWidget) _BarWidget->deleteLater();
	if (_ChatWidget) _ChatWidget->deleteLater();
	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	settings.setValue(QStringLiteral("Locked"), 
		_LockState == OverlayLocked || _LockState == OverlayLockPending);
	settings.setValue(QStringLiteral("ChatVisible"), _ChatVisible);
	settings.endGroup(); // Overlay
}

void POverlayController::Initialize()
{
	// Initialize the chat widgets.
	_BarWidget = new POverlayBarWidget();
	_BarWidget->hide();
	InstallArrowFilter(_BarWidget);
	_ChatWidget = new POverlayChatWidget();
	_ChatWidget->hide();
	_ChatWidget->setStyleSheet(_BarWidget->styleSheet());
	InstallArrowFilter(_ChatWidget);

	// Set up the timer that checks for the game being active.
	_GameCheckTimer.setInterval(100);
	_GameCheckTimer.start();
	connect(&_GameCheckTimer, &QTimer::timeout, this, &POverlayController::OnCheckGameActive);

	// Load the settings.
	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	if (settings.value(QStringLiteral("Locked")).toBool()) Lock();
	SetChatVisibility(settings.value(QStringLiteral("ChatVisible"), false).toBool());
	settings.endGroup(); // Overlay
}

POverlayBarWidget* POverlayController::GetBarWidget() const
{
	return _BarWidget;
}

POverlayChatWidget* POverlayController::GetChatWidget() const
{
	return _ChatWidget;
}

POverlayController::LockState POverlayController::GetLockState() const
{
	return _LockState;
}

bool POverlayController::IsChatVisible() const
{
	return _ChatVisible;
}

bool POverlayController::IsGameActive() const
{
	return _GameActive;
}

void POverlayController::Lock()
{
	// If the lock state was already set to OverlayLocked or OverlayLockPending, we don't need to do anything.
	if (_LockState == OverlayLocked || _LockState == OverlayLockPending) return;
	// Otherwise, setting the state will cause it to be locked the next time the overlay is shown.
	else SetLockState(OverlayLockPending);
}

void POverlayController::Unlock()
{
	// If the lock state was already set to OverlayUnlocked or OverlayUnlockPending, we don't need to do 
	// anything.
	if (_LockState == OverlayUnlocked || _LockState == OverlayUnlockPending) return;
	// Otherwise, setting the state will cause it to be locked the next time the overlay is shown.
	else SetLockState(OverlayUnlockPending);
}

void POverlayController::SetLocked(bool locked)
{
	if (locked) Lock();
	else Unlock();
}

void POverlayController::ShowChat()
{
	SetChatVisibility(true);
}

void POverlayController::HideChat()
{
	SetChatVisibility(false);
}

void POverlayController::SetChatVisibility(bool visible)
{
	if (visible != _ChatVisible)
	{
		_ChatVisible = visible;
		UpdateVisibility();
		emit ChatVisibilityChanged(visible);
	}
}

bool POverlayController::eventFilter(QObject* watched, QEvent* event)
{
	// Determine which overlay widget is being acted upon.
	auto rootWidget = FindRootWidget(qobject_cast<QWidget*>(watched));
	if (!rootWidget || event->type() != QEvent::KeyPress) return false;
	auto keyPressEvent = static_cast<QKeyEvent*>(event);
	auto newPos = rootWidget->pos();
	auto screen = qApp->screenAt(rootWidget->geometry().topLeft());
	auto screenRect = screen->geometry();
	auto increment = keyPressEvent->modifiers().testFlag(Qt::ControlModifier) ? 1 : 5;
	bool arrowKey = true;
	switch (keyPressEvent->key())
	{
	case Qt::Key_Up:
		newPos.setY(qMax(newPos.y() - increment, screenRect.top() - rootWidget->geometry().top() +
			rootWidget->frameGeometry().top()));
		break;
	case Qt::Key_Right:
		newPos.setX(qMin(newPos.x() + increment, screenRect.right() - rootWidget->geometry().right() + 
			rootWidget->frameGeometry().left()));
		break;
	case Qt::Key_Down:
		newPos.setY(qMin(newPos.y() + increment, screenRect.bottom() - rootWidget->geometry().bottom() +
			rootWidget->frameGeometry().top()));
		break;
	case Qt::Key_Left:
		newPos.setX(qMax(newPos.x() - increment, screenRect.left() - rootWidget->geometry().left() +
			rootWidget->frameGeometry().left()));
		break;
	default:
		arrowKey = false;
		break;
	}
	if (arrowKey)
	{
		rootWidget->move(newPos);
		return true;
	}
	return false;

}

void POverlayController::OnCheckGameActive()
{
	// Update the state of whether or not the game is active.
	SetGameActive(IsGameOrOverlayActive());
	// Trigger the timer to apply lock or unlock appearance if they're pending. This can only be done
	// after windows are shown and the event loop has run to successfully handle position changes due to
	// hide/show of the title bar of the windows.
	if (_GameActive)
	{
		if (_LockState == OverlayLockPending)
		{
			QTimer::singleShot(1, this, &POverlayController::OnApplyLockAppaerance);
		}
		else if (_LockState == OverlayUnlockPending)
		{
			QTimer::singleShot(1, this, &POverlayController::OnApplyUnlockAppearance);
		}
	}
}

void POverlayController::OnApplyLockAppaerance()
{
	if (_LockState != OverlayLockPending) return;
	ApplyLockAppearance(_ChatWidget);
	ApplyLockAppearance(_BarWidget);
	SetLockState(OverlayLocked);
}

void POverlayController::OnApplyUnlockAppearance()
{
	if (_LockState != OverlayUnlockPending) return;
	ApplyUnlockAppearance(_ChatWidget);
	ApplyUnlockAppearance(_BarWidget);
	SetLockState(OverlayUnlocked);
}

void POverlayController::SetLockState(LockState state)
{
	if (state != _LockState)
	{
		_LockState = state;
		if (state == OverlayLocked) emit Locked();
		if (state == OverlayUnlocked) emit Unlocked();
		emit LockStateChanged(state);
	}
}

bool POverlayController::IsGameOrOverlayActive() const
{
	// We're looking for the title of the foreground window to see if it's Path of Exile or one of the overlay
	// windows.
	auto windowHwnd = GetForegroundWindow();
	if (!windowHwnd) return false;
	WCHAR title[33];
	title[32] = '\0]';
	auto len = GetWindowTextW(windowHwnd, title, 32);
	bool show = false;
	auto titleStr = QString::fromStdWString(title);
	titleStr.remove(" - PoePal");
	return titleStr == L"Path of Exile" || titleStr == _BarWidget->windowTitle() ||
		(_ChatWidget && titleStr == _ChatWidget->windowTitle());
}

void POverlayController::ApplyLockAppearance(QWidget* widget)
{
	// Record the original corner location. We'll use this to move the window once the frame and title are
	// hidden.
	auto origCornerPos = widget->geometry().topLeft();
	bool visible = widget->isVisible();
	if (visible) widget->hide();
	widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
	if (visible) widget->show();
	// Move the window back so that the corner is in the original location.
	auto newCornerPos = widget->geometry().topLeft();
	auto newWindowPos = widget->pos();
	widget->move(newWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		newWindowPos.y() + origCornerPos.y() - newCornerPos.y());
	// Remove the event filter so arrows are no longer captured.
	UninstallArrowFilter(widget);
}

void POverlayController::ApplyUnlockAppearance(QWidget* widget)
{
	// Record the original corner location. We'll use this to move the window once the frame and title are
	// shown.
	auto origCornerPos = widget->geometry().topLeft();
	auto origWindowPos = widget->pos();
	bool visible = widget->isVisible();
	if (visible) widget->hide();
	widget->setWindowFlags(widget->windowFlags() & ~Qt::FramelessWindowHint);
	if (visible) widget->show();
	// Move the window back so that the corner is in the original location.
	auto newCornerPos = widget->geometry().topLeft();
	widget->move(origWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		origWindowPos.y() + origCornerPos.y() - newCornerPos.y());
	// Install an event filter on all of the child widgets. We'll use this to be able to move the widget 
	// around with keys while the UI is unlocked.
	InstallArrowFilter(widget);
}

void POverlayController::InstallArrowFilter(QWidget* widget)
{
	for (const auto& child : widget->findChildren<QWidget*>()) child->installEventFilter(this);
}

void POverlayController::UninstallArrowFilter(QWidget* widget)
{
	for (const auto& child : widget->findChildren<QWidget*>()) child->removeEventFilter(this);
}

QWidget* POverlayController::FindRootWidget(QWidget* child) const
{
	// Check whether the child itself is a root widget.
	if (!child || child == _ChatWidget || child == _BarWidget) return child;
	// Otherwise, check the parent, if there is one.
	return FindRootWidget(child->parentWidget());
}

void POverlayController::SetGameActive(bool active)
{
	if (active == _GameActive) return;
	_GameActive = active;
	UpdateVisibility();
	emit GameActiveChanged(_GameActive);
}

void POverlayController::UpdateVisibility()
{
	// Update the visibility of the windows.
	// The chat widget should be shown if it's boolean is set or if we're unlocked or about to unlock.
	bool unlocking = _LockState == OverlayUnlocked || _LockState == OverlayUnlockPending;
	_ChatWidget->setVisible(_GameActive && (_ChatVisible || unlocking));
	_BarWidget->setVisible(_GameActive); // bar widget is always visible when the game is active.

}
