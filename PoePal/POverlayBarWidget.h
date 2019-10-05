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

#include <QMenu>
#include <QPointer>
#include <QWidget>
#include "ui_POverlayBarWidget.h"

class POverlayChatWidget;
class POverlayTradeWidget;
class QPropertyAnimation;

/**
 * Class provides a bar UI that overlays the game and serves as the main UI for the user in-game.
 */
class POverlayBarWidget : public QWidget, public Ui::POverlayBarWidget
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
	 *   The parent of this widget.
	 */
	POverlayBarWidget(QWidget* parent = nullptr);

	/**
	 * Destructor.
	 */
	virtual ~POverlayBarWidget() = default;

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
	 * Overrides QWidget#enterEvent.
	 */
	virtual void enterEvent(QEvent* evt) override;

	/**
	 * Overrides QWidget#leaveEvent.
	 */
	virtual void leaveEvent(QEvent* evt) override;

private slots:

	/**
	 * Slot called when a button is clicked.
	 */
	void OnButtonClicked();

private:

	/**
	 * Indicates whether or not the widget is locked.
	 * true if the widget is locked, false otherwise.
	 */
	bool _Locked = false;

	/**
	 * The animation for the collapsible frame width.
	 */
	QPointer<QPropertyAnimation> _CollapseAnimation;

	/**
	 * The configuration menu.
	 */
	QMenu _ConfigMenu;

	/**
	 * The action that locks the positions of all of the widgets.
	 */
	QPointer<QAction> _LockAction;

	/**
	 * The action that unlocks the positions of all of the widgets.
	 */
	QPointer<QAction> _UnlockAction;

	/**
	 * The overlay chat widget.
	 */
	QPointer<POverlayChatWidget> _ChatWidget;

	/**
	 * The overlay trade widget.
	 */
	QPointer<POverlayTradeWidget> _TradeWidget;
};