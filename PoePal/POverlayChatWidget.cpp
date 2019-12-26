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
#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QSettings>
#include <QScrollBar>
#include <QTimer>

POverlayChatWidget::POverlayChatWidget(QWidget* parent /*= nullptr*/):
PChatWidget(PMessage::Whisper, parent)
{
	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

	setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
	auto app = qobject_cast<PApplication*>(qApp);
	auto controller = app->GetOverlayController();

	// Fix the tab bar widget border on the right.
	auto tabBar = _WhisperTabs->tabBar();
	tabBar->setStyleSheet(controller->GetStyleSheet());
	tabBar->GetListWidget()->verticalScrollBar()->setStyleSheet(controller->GetStyleSheet());

	// Get the style sheet and apply it here.
	setStyleSheet(controller->GetStyleSheet());

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

bool POverlayChatWidget::ShouldRetainFocus() const
{
	return false;
}
