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
#include "PMessageModel.h"
#include <QScrollBar>

PLogWidget::PLogWidget(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);
	auto app = qobject_cast<PApplication *>(qApp);
	ui._ListView->setModel(app->GetMessageModel());
}

PLogWidget::~PLogWidget()
{
}
