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
#include "ui_PChatOptionsWidget.h"
#include "PMessage.h"

class PChatWidget;

/**
 * The user interface for defining options for a chat widget.
 * This is implemented as a widget so it can be embedded in multiple places.
 */
class PChatOptionsWidget : public QWidget
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this widget.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	PChatOptionsWidget(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PChatOptionsWidget();

	/**
	 * Retrieves the current label text.
	 * @return
	 *   The current label text.
	 */
	QString GetLabel() const;

	/**
	 * Sets the chat widget to configure.
	 * @param[in] widget
	 *   The widget being configured.
	 */
	void SetWidget(PChatWidget *widget);

	/**
	 * Saves the settings to the given chat widget.
	 */
	void SaveToWidget();

private:

	/**
	 * Updates the visibility for the controls of the given channel.
	 * @param[in] channel
	 *   The channel to update the controls for.
	 * @param[in] check
	 *   The check button for the control.
	 * @param[in] singleChannel
	 *   The single channel being displayed.
	 */
	void SetChannelViewMode(PMessage::Channel channel, QCheckBox *check, 
		PMessage::Channel singleChannel);

	/**
	 * The chat widget being configured.
	 */
	PChatWidget *_Widget = nullptr;

	Ui::PChatOptionsWidget ui;
};
