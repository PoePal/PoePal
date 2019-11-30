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
#include "POverlayChatWidget.h"
#include "PApplication.h"
#include "POverlayBarWidget.h"
#include "POverlayController.h"
#include <QLabel>
#include <QSettings>
#include <QTimer>

POverlayChatWidget::POverlayChatWidget(QWidget* parent /*= nullptr*/):
PChatWidget(PMessage::Whisper, parent)
{
	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

	setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

	// Restore the chat widget settings.
	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	settings.beginGroup(QStringLiteral("ChatWidget"));
	if (settings.contains(QStringLiteral("Geometry")))
	{
		restoreGeometry(settings.value(QStringLiteral("Geometry")).toByteArray());
	}
	settings.endGroup(); // ChatWidget
	settings.endGroup(); // Overlay
}

POverlayChatWidget::~POverlayChatWidget()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	settings.beginGroup(QStringLiteral("ChatWidget"));
	settings.setValue(QStringLiteral("Geometry"), saveGeometry());
	settings.endGroup(); // ChatWidget
	settings.endGroup(); // Overlay
}

bool POverlayChatWidget::IsLocked() const
{
	return _Locked;
}

void POverlayChatWidget::Lock()
{
	if (_Locked) return;
	_Locked = true;
	auto origCornerPos = geometry().topLeft();
	auto origWindowPos = pos();
	bool visible = isVisible();
	if (visible) hide();
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	if (visible) show();
	auto newCornerPos = geometry().topLeft();
	auto newWindowPos = pos();
	move(newWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		newWindowPos.y() + origCornerPos.y() - newCornerPos.y());
	// Remove the event filter so arrows are no longer captured.
	for (const auto& child : findChildren<QWidget*>()) child->removeEventFilter(this);
}

void POverlayChatWidget::Unlock()
{
	if (!_Locked) return;
	_Locked = false;
	auto origCornerPos = geometry().topLeft();
	auto origWindowPos = pos();
	bool visible = isVisible();
	if(visible) hide();
	setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
	if(visible) show();
	auto newCornerPos = geometry().topLeft();
	move(origWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		origWindowPos.y() + origCornerPos.y() - newCornerPos.y());
	// Install an event filter on all of the child widgets. We'll use this to be able to move the widget 
	// around with keys.
	for (const auto& child : findChildren<QWidget*>()) child->installEventFilter(this);
}

void POverlayChatWidget::SetLocked(bool locked)
{
	if (locked) Lock();
	else Unlock();
}

void POverlayChatWidget::ToggleLock()
{
	SetLocked(!_Locked);
}

bool POverlayChatWidget::ShouldRetainFocus() const
{
	return false;
}

bool POverlayChatWidget::eventFilter(QObject* watched, QEvent* event)
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
