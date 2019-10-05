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
#pragma once

#include <QDialog>
#include "ui_PChatOptionsDlg.h"

/**
 * Standalone dialog to define options for a chat widget.
 */
class PChatOptionsDlg : public QDialog
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this dialog.
	 */
	PChatOptionsDlg(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PChatOptionsDlg();

	/**
	 * Retrieves the chat options widget.
	 * @return
	 *   The chat options widget.
	 */
	PChatOptionsWidget * GetOptionsWidget() const;

	/**
	 * Runs this dialog in modal form and retrieves options.
	 * @param[in] widget
	 *   The widget being configured.
	 */
	static void SetOptionsModal(PChatDockWidget *widget);

private:
	Ui::PChatOptionsDlg ui;
};
