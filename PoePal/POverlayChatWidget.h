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

	/**
	 * Indicates whether the widget is locked.
	 * This is true if the widget is locked and false otherwise.
	 */
	Q_PROPERTY(bool locked READ IsLocked WRITE SetLocked)

public:

	/**
	 * Creates a new instance of this widget.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	POverlayChatWidget(QWidget* parent = nullptr);

	/**
	 * Indicates whether or not the widget is locked.
	 * @return
	 *   true if the widget is locked, false otherwise.
	 */
	bool IsLocked() const;

public slots:

	/**
	 * Locks the overlay widget.
	 */
	void Lock();

	/**
	 * Unlocks the overlay widget.
	 */
	void Unlock();

	/**
	 * Sets the lock state of the overlay widget.
	 * @param[in] locked
	 *   true if the widget is to be locked, false if it is to be unlocked.
	 */
	void SetLocked(bool locked);

	/**
	 * Toggles the lock state of the overlay widget.
	 */
	void ToggleLock();

protected:

	/**
	 * Overrides PChatWidget#ShouldRetainFocus.
	 */
	virtual bool ShouldRetainFocus() const override;

	/**
	 * Overrides PChatWidget#focusInEvent.
	 */
	virtual void focusInEvent(QFocusEvent* event) override;

private:

	/**
	 * Indicates whether or not the widget is locked.
	 * true if the widget is locked, false otherwise.
	 */
	bool _Locked = false;
};