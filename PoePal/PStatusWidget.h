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
#include "ui_PStatusWidget.h"

class PMessage;

/**
 * Provides UI for setting status messages and status (AFK, Auto Reply, DND, etc.)
 */
class PStatusWidget : public QDockWidget
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this widget.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	PStatusWidget(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PStatusWidget();


protected:

	/**
	 * Overrides QObject#eventFilter
	 */
	virtual bool eventFilter(QObject *watched, QEvent *event) override;
	
private slots:

	/**
	 * Slot called when a new message is found.
	 * @param[in] msg
	 *   The new message.
	 */
	void OnNewMessage(PMessage *msg);

	/**
	 * Slot called when an action is clicked.
	 * @param[in] action
	 *   The action that was triggered.
	 */
	void OnActionTriggered(QAction *Action);

private:
	Ui::PStatusWidget ui;
};
