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
#include "PChatWidgetsWidget.h"
#include "PApplication.h"
#include "PChatOptionsWidget.h"
#include "PChatWidget.h"
#include "PMainWindow.h"
#include <QSettings>

PChatWidgetsWidget::PChatWidgetsWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui._WidgetCombo->clear();
	// Load the default widgets into the dropdown.
	AddDefaultChatWidget(PLogMessage::Global);
	AddDefaultChatWidget(PLogMessage::Trade);
	AddDefaultChatWidget(PLogMessage::Guild);
	AddDefaultChatWidget(PLogMessage::Party);
	AddDefaultChatWidget(PLogMessage::Local);
	AddDefaultChatWidget(PLogMessage::Whisper);
	// Load the custom widgets into the dropdown.
	AddChatWidget(nullptr);
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto mainWindow = app->GetMainWindow();
	for (const auto &widget : mainWindow->GetCustomChatWidgets()) AddChatWidget(widget);
	connect(ui._WidgetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), ui._WidgetStack, 
		&QStackedWidget::setCurrentIndex);
	connect(ui._WidgetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
		&PChatWidgetsWidget::OnChatWidgetChanged);
	connect(ui._AddWidgetBtn, &QPushButton::clicked, this, &PChatWidgetsWidget::OnAddClicked);
	connect(ui._DeleteWidgetBtn, &QPushButton::clicked, this, &PChatWidgetsWidget::OnRemoveClicked);
	connect(ui._WidgetCombo, QOverload<int>::of(&QComboBox::highlighted), this, 
		&PChatWidgetsWidget::UpdateWidgetsCombo);
	OnChatWidgetChanged();
}

PChatWidgetsWidget::~PChatWidgetsWidget()
{
}

void PChatWidgetsWidget::Save()
{
	for (const auto &widget : ui._WidgetStack->children())
	{
		auto chatWidget = qobject_cast<PChatOptionsWidget *>(widget);
		if (chatWidget) chatWidget->SaveToWidget();
	}
}

void PChatWidgetsWidget::OnChatWidgetChanged()
{
	int idx = ui._WidgetCombo->currentIndex();
	if (idx < 0) return;
	auto widget = ui._WidgetCombo->currentData().value<PChatWidget *>();
	ui._DeleteWidgetBtn->setEnabled(widget && widget->GetDefaultChannel() == PLogMessage::InvalidChannel);
	UpdateWidgetsCombo();
}

void PChatWidgetsWidget::OnAddClicked()
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto mainWindow = app->GetMainWindow();
	auto newWidget = mainWindow->CreateCustomChatWidget();
	newWidget->setFloating(false);
	AddChatWidget(newWidget);
	ui._WidgetCombo->setCurrentIndex(ui._WidgetCombo->count()-1);
	app->setActiveWindow(parentWidget());
	parentWidget()->setFocus(Qt::MouseFocusReason);
}

void PChatWidgetsWidget::OnRemoveClicked()
{
	int idx = ui._WidgetCombo->currentIndex();
	if (idx < 0) return;
	auto widget = ui._WidgetCombo->currentData().value<PChatWidget *>();
	auto stackWidget = ui._WidgetStack->widget(idx);
	ui._WidgetStack->removeWidget(stackWidget);
	stackWidget->deleteLater();
	widget->Remove();
	ui._WidgetCombo->removeItem(idx);
}

void PChatWidgetsWidget::UpdateWidgetsCombo()
{
	int count = ui._WidgetCombo->count();
	for (int w = 0; w < count; ++w)
	{
		auto stackedWidget = qobject_cast<PChatOptionsWidget *>(ui._WidgetStack->widget(w));
		if (stackedWidget) ui._WidgetCombo->setItemText(w, stackedWidget->GetLabel());
	}
}

void PChatWidgetsWidget::AddDefaultChatWidget(PLogMessage::Channel channel)
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto mainWindow = app->GetMainWindow();
	auto widget = mainWindow->GetDefaultChatWidget(channel);
	AddChatWidget(widget);
}

void PChatWidgetsWidget::AddChatWidget(PChatWidget *widget)
{
	if (!widget)
	{
		ui._WidgetCombo->addItem("---------");
		ui._WidgetStack->addWidget(new QWidget(ui._WidgetStack));
	}
	else
	{
		ui._WidgetCombo->addItem(widget->windowTitle(), QVariant::fromValue(widget));
		auto editWidget = new PChatOptionsWidget(ui._WidgetStack);
		editWidget->SetWidget(widget);
		ui._WidgetStack->addWidget(editWidget);
	}
}
