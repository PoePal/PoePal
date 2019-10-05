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
#include <QLabel>
#include <QTimer>

POverlayChatWidget::POverlayChatWidget(QWidget* parent /*= nullptr*/):
PChatWidget(PMessage::Whisper, parent)
{
	setWindowFlags(windowFlags() | Qt::Window | Qt::Tool | Qt::WindowStaysOnTopHint);

	_WhisperTabs->setTabPosition(QTabWidget::North);
	_WhisperTabs->setTabsClosable(true);

	setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
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
		newWindowPos.y() + origCornerPos.y() - newCornerPos.y() + newWindowPos.y() - origWindowPos.y());
	move(newWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		newWindowPos.y() + origCornerPos.y() - newCornerPos.y());
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
	auto newWindowPos = pos();
	auto newCornerPos = geometry().topLeft();
	move(origWindowPos.x() + origCornerPos.x() - newCornerPos.x(),
		origWindowPos.y() + origCornerPos.y() - newCornerPos.y());
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

void POverlayChatWidget::focusInEvent(QFocusEvent* event)
{
	PChatWidget::focusInEvent(event);
	auto app = qobject_cast<PApplication*>(qApp);
	Q_ASSERT(app);
	app->GetOverlayBarWidget()->raise();
}
