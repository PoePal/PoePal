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
#include "PChatDockWidget.h"
#include "PApplication.h"
#include "PChatOptionsDlg.h"
#include "PChatWidget.h"
#include "PMainWindow.h"
#include <QSettings>

PChatDockWidget::PChatDockWidget(QWidget* parent /*= Q_NULLPTR*/) :
QDockWidget(parent), _ChatWidget(new PChatWidget(this))
{
	Initialize();
}

PChatDockWidget::PChatDockWidget(PMessage::Channel defaultChannel, QWidget* parent /*= Q_NULLPTR*/):
QDockWidget(parent), _ChatWidget(new PChatWidget(defaultChannel, this))
{
	Initialize();
}

PChatWidget* PChatDockWidget::GetChatWidget() const
{
	return _ChatWidget;
}

void PChatDockWidget::LoadState(const QSettings& settings)
{
	if (_ChatWidget->GetDefaultChannel() == PMessage::InvalidChannel)
	{
		PMessage::Channels channels = static_cast<PMessage::Channel>(
			settings.value(QStringLiteral("Channels")).toInt());
		_ChatWidget->SetChannels(channels);
		setWindowTitle(settings.value(QStringLiteral("Title")).toString());
	}
}

void PChatDockWidget::SaveState(QSettings& settings) const
{
	if (_ChatWidget->GetDefaultChannel() == PMessage::InvalidChannel)
	{
		settings.setValue(QStringLiteral("Title"), windowTitle());
		settings.setValue(QStringLiteral("Channels"), static_cast<int>(_ChatWidget->GetChannels()));
	}
}

void PChatDockWidget::Configure()
{
	PChatOptionsDlg::SetOptionsModal(this);
}

void PChatDockWidget::Remove()
{
	if (_ChatWidget->GetDefaultChannel() != PMessage::InvalidChannel) return;
	auto app = qobject_cast<PApplication*>(qApp);
	Q_ASSERT(app);
	app->GetMainWindow()->RemoveCustomChatWidget(this);
}

void PChatDockWidget::Initialize()
{
	setWidget(_ChatWidget);
}
