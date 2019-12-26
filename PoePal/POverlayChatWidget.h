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

#include "PChatWidget.h"

/**
 * Modified dock widget showing a chat channel suitable for the overlay.
 * This widget is intended to only show whispers.
 */
class POverlayChatWidget : public PChatWidget
{
	Q_OBJECT;

public:

	/**
	 * Creates a new instance of this widget.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	POverlayChatWidget(QWidget* parent = nullptr);

	/**
	 * Destructor saves the settings for this widget.
	 */
	virtual ~POverlayChatWidget();

protected:

	/**
	 * Overrides PChatWidget#ShouldRetainFocus.
	 */
	virtual bool ShouldRetainFocus() const override;
};