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

#include <QWidget>
#include "ui_PChatWidgetsWidget.h"
#include "PMessage.h"

class PChatWidget;

/**
 * The user interface for editing all of the chat widgets.
 */
class PChatWidgetsWidget : public QWidget
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this widget.
	 */
	PChatWidgetsWidget(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PChatWidgetsWidget();

public slots:

	/**
	 * Saves the data to the widgets.
	 */
	void Save();

private slots:

	/**
	 * Slot called when the current chat widget changes.
	 */
	void OnChatWidgetChanged();

	/**
	 * Slot called when the add button is clicked.
	 */
	void OnAddClicked();

	/**
	 * Slot called when the remove button is clicked.
	 */
	void OnRemoveClicked();

	/**
	 * Updates text in the combo box.
	 */
	void UpdateWidgetsCombo();

private:

	/**
	 * Adds a default chat window.
	 * @param[in] channel
	 *   The channel for which to add the default chat window.
	 */
	void AddDefaultChatWidget(PMessage::Channel channel);

	/**
	 * Adds a chat window.
	 * @param[in] window
	 *   The chat window to add.
	 */
	void AddChatWidget(PChatWidget *window);

	Ui::PChatWidgetsWidget ui;
};
