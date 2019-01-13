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
#include "PLogWidget.h"
#include "PApplication.h"
#include "PLogMessage.h"
#include "PLogScanner.h"
#include <QScrollBar>

PLogWidget::PLogWidget(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	connect(app->GetLogScanner(), &PLogScanner::Initialized, this, &PLogWidget::OnInitialized);
}

PLogWidget::~PLogWidget()
{
}

void PLogWidget::OnInitialized()
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto scanner = app->GetLogScanner();
	QString contents;
	for (const auto &msg : scanner->GetLogMessages())
	{
		contents += msg->ToString() + "\n";
	}
	contents.resize(contents.length() - 1);
	ui._Text->setPlainText(contents);
	ui._Text->verticalScrollBar()->setValue(ui._Text->verticalScrollBar()->maximum());
	connect(scanner, &PLogScanner::NewMessage, this, &PLogWidget::OnNewMessage);
}

void PLogWidget::OnNewMessage(PLogMessage *message)
{
	ui._Text->appendPlainText(message->ToString());
}
