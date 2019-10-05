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
#include "PChatOptionsDlg.h"
#include "PChatDockWidget.h"

PChatOptionsDlg::PChatOptionsDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui._Buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui._Buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

PChatOptionsDlg::~PChatOptionsDlg()
{
}

PChatOptionsWidget * PChatOptionsDlg::GetOptionsWidget() const
{
	return ui._ChatOptions;
}

void PChatOptionsDlg::SetOptionsModal(PChatDockWidget *widget)
{
	PChatOptionsDlg dialog(widget);
	dialog.ui._ChatOptions->SetWidget(widget);
	if (dialog.exec() == QDialog::Accepted) dialog.ui._ChatOptions->SaveToWidget();
}
