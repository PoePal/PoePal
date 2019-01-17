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

#include <QMdiSubWindow>
#include "ui_PPassivesWindow.h"

class PMessage;

/**
 * Provides an MDI sub window to view passive and ascendancy skill points acquired.
 */
class PPassivesWindow : public QMdiSubWindow
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this window.
	 * @param[in] parent
	 *   The parent of this window.
	 */
	PPassivesWindow(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PPassivesWindow();

public slots:

	/**
	 * Clear out the statistics.
	 */
	void ClearStats();

	/**
	 * Refresh the statistics.
	 */
	void Refresh();

private:

	/**
	 * Slot called when a new message is received.
	 * @param[in] message
	 *   The new message.
	 */
	void OnNewMessage(PMessage *message);

private:
	Ui::PPassivesWindow ui;
};
