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
#include "PChatOptionsWidget.h"
#include "PChatDockWidget.h"
#include "PChatWidget.h"
#include <QAction>

PChatOptionsWidget::PChatOptionsWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

PChatOptionsWidget::~PChatOptionsWidget()
{
}

QString PChatOptionsWidget::GetLabel() const
{
	return ui._LabelEdit->text();
}

void PChatOptionsWidget::SetWidget(PChatDockWidget *widget)
{
	_Widget = widget;
	if (!_Widget) return;
	ui._LabelEdit->setText(widget->windowTitle());
	auto chatWidget = widget->GetChatWidget();
	ui._LabelEdit->setEnabled(chatWidget->GetDefaultChannel() == PMessage::InvalidChannel);
	auto action = widget->property("associatedAction").value<QAction *>();
	ui._ShowCheck->setChecked(action && action->isChecked());
	ui._DockCheck->setChecked(!widget->isFloating());
	auto defChannel = chatWidget->GetDefaultChannel();
	SetChannelViewMode(PMessage::Global, ui._GlobalCheck, defChannel);
	SetChannelViewMode(PMessage::Trade, ui._TradeCheck, defChannel);
	SetChannelViewMode(PMessage::Guild, ui._GuildCheck, defChannel);
	SetChannelViewMode(PMessage::Party, ui._PartyCheck, defChannel);
	SetChannelViewMode(PMessage::Local, ui._LocalCheck, defChannel);
	SetChannelViewMode(PMessage::Whisper, ui._WhisperCheck, defChannel);
	auto channels = chatWidget->GetChannels();
	ui._GlobalCheck->setChecked(channels.testFlag(PMessage::Global));
	ui._TradeCheck->setChecked(channels.testFlag(PMessage::Trade));
	ui._GuildCheck->setChecked(channels.testFlag(PMessage::Guild));
	ui._PartyCheck->setChecked(channels.testFlag(PMessage::Party));
	ui._LocalCheck->setChecked(channels.testFlag(PMessage::Local));
}

void PChatOptionsWidget::SaveToWidget()
{
	if (!_Widget) return;
	_Widget->setWindowTitle(ui._LabelEdit->text());
	auto chatWidget = _Widget->GetChatWidget();
	auto action = _Widget->property("associatedAction").value<QAction *>();
	if(action) action->setChecked(ui._ShowCheck->isChecked());
	_Widget->setVisible(ui._ShowCheck->isChecked());
	_Widget->setFloating(!ui._DockCheck->isChecked());
	PMessage::Channels channels = 0;
	if (ui._GlobalCheck->isChecked()) channels.setFlag(PMessage::Global);
	if (ui._TradeCheck->isChecked()) channels.setFlag(PMessage::Trade);
	if (ui._GuildCheck->isChecked()) channels.setFlag(PMessage::Guild);
	if (ui._PartyCheck->isChecked()) channels.setFlag(PMessage::Party);
	if (ui._LocalCheck->isChecked()) channels.setFlag(PMessage::Local);
	if (ui._WhisperCheck->isChecked()) channels.setFlag(PMessage::Whisper);
	chatWidget->SetChannels(channels);
}

void PChatOptionsWidget::SetChannelViewMode(PMessage::Channel channel, QCheckBox *check, 
	PMessage::Channel singleChannel)
{
	check->setEnabled(singleChannel == PMessage::InvalidChannel);
	if (singleChannel == channel) check->setChecked(true);
	else if (singleChannel != PMessage::InvalidChannel) check->setChecked(false);
}
