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

void PChatOptionsWidget::SetWidget(PChatWidget *widget)
{
	_Widget = widget;
	if (!_Widget) return;
	ui._LabelEdit->setText(widget->windowTitle());
	ui._LabelEdit->setEnabled(widget->GetDefaultChannel() == PLogMessage::InvalidChannel);
	auto action = widget->property("associatedAction").value<QAction *>();
	ui._ShowCheck->setChecked(action && action->isChecked());
	ui._DockCheck->setChecked(!widget->isFloating());
	auto defChannel = widget->GetDefaultChannel();
	SetChannelViewMode(PLogMessage::Global, ui._GlobalCheck, defChannel);
	SetChannelViewMode(PLogMessage::Trade, ui._TradeCheck, defChannel);
	SetChannelViewMode(PLogMessage::Guild, ui._GuildCheck, defChannel);
	SetChannelViewMode(PLogMessage::Party, ui._PartyCheck, defChannel);
	SetChannelViewMode(PLogMessage::Local, ui._LocalCheck, defChannel);
	SetChannelViewMode(PLogMessage::Whisper, ui._WhisperCheck, defChannel);
	auto channels = widget->GetChannels();
	ui._GlobalCheck->setChecked(channels.testFlag(PLogMessage::Global));
	ui._TradeCheck->setChecked(channels.testFlag(PLogMessage::Trade));
	ui._GuildCheck->setChecked(channels.testFlag(PLogMessage::Guild));
	ui._PartyCheck->setChecked(channels.testFlag(PLogMessage::Party));
	ui._LocalCheck->setChecked(channels.testFlag(PLogMessage::Local));
}

void PChatOptionsWidget::SaveToWidget()
{
	if (!_Widget) return;
	_Widget->setWindowTitle(ui._LabelEdit->text());
	auto action = _Widget->property("associatedAction").value<QAction *>();
	if(action) action->setChecked(ui._ShowCheck->isChecked());
	_Widget->setVisible(ui._ShowCheck->isChecked());
	_Widget->setFloating(!ui._DockCheck->isChecked());
	PLogMessage::Channels channels = 0;
	if (ui._GlobalCheck->isChecked()) channels.setFlag(PLogMessage::Global);
	if (ui._TradeCheck->isChecked()) channels.setFlag(PLogMessage::Trade);
	if (ui._GuildCheck->isChecked()) channels.setFlag(PLogMessage::Guild);
	if (ui._PartyCheck->isChecked()) channels.setFlag(PLogMessage::Party);
	if (ui._LocalCheck->isChecked()) channels.setFlag(PLogMessage::Local);
	if (ui._WhisperCheck->isChecked()) channels.setFlag(PLogMessage::Whisper);
	_Widget->SetChannels(channels);
}

void PChatOptionsWidget::SetChannelViewMode(PLogMessage::Channel channel, QCheckBox *check, 
	PLogMessage::Channel singleChannel)
{
	check->setEnabled(singleChannel == PLogMessage::InvalidChannel);
	if (singleChannel == channel) check->setChecked(true);
	else if (singleChannel != PLogMessage::InvalidChannel) check->setChecked(false);
}
