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

#include <QDockWidget>
#include "ui_PLogWidget.h"

class PLogMessage;

/**
 * Displays log messages in a dock widget.
 */
class PLogWidget : public QDockWidget
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this dock widget.
	 * @param[in] parent
	 *   The parent of the dock widget.
	 */
	PLogWidget(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PLogWidget();

private slots:

	/**
	 * Slot called when the log scanner is initialized.
	 */
	void OnInitialized();

	/**
	 * Slot called when a new message is received from the log scanner.
	 * @param[in] message
	 *   The new message.
	 */
	void OnNewMessage(PLogMessage *message);

private:
	Ui::PLogWidget ui;
};
