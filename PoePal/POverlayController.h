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

#include <QObject>
#include <QPointer>
#include <QTimer>

class POverlayBarWidget;
class POverlayChatWidget;

/**
 * Class handles the behavior of the in-game overlay.
 */
class POverlayController : public QObject
{
	Q_OBJECT;

	/**
	 * Indicates whether the overlay is locked, unlocked, or transitioning to either.
	 */
	Q_PROPERTY(LockState lockState READ GetLockState NOTIFY LockStateChanged)

	/**
	 * Indicates whether the chat window is visible.
	 * This state transcends whether or not the overlay in general is shown due to the game being active. This
	 * makes it useful to see whether the chat widget will be shown if the game is active.
	 */
	Q_PROPERTY(bool chatVisible READ IsChatVisible WRITE SetChatVisibility NOTIFY ChatVisibilityChanged)

	/**
	 * Indicates whether or not the game is active.
	 */
	Q_PROPERTY(bool gameActive READ IsGameActive NOTIFY GameActiveChanged)

public:

	/**
	 * The lock state.
	 */
	enum LockState
	{
		OverlayLocked, ///< The overlay is locked.
		OverlayLockPending, ///< The overlay will be locked when the game takes focus again.
		OverlayUnlocked, ///< The overlay is unlocked.
		OverlayUnlockPending ///< The overlay will be unlocked when the game takes focus again.
	};
	Q_ENUM(LockState);

	/**
	 * Creates a new overlay controller.
	 * @param[in] parent
	 *   The parent of the overlay controller.
	 */
	POverlayController(QObject* parent);

	/**
	 * Destructor cleans up all of the child widgets.
	 */
	virtual ~POverlayController();

	/**
	 * Initializes the overlay controller.
	 */
	void Initialize();

	/**
	 * Retrieves the overlay bar widget.
	 * @return
	 *   The overlay bar widget.
	 */
	POverlayBarWidget* GetBarWidget() const;

	/**
	 * Retrieves the overlay chat widget.
	 * @return
	 *   The overlay chat widget.
	 */
	POverlayChatWidget* GetChatWidget() const;

	/**
	 * Retrieves the lock state of the overlay.
	 * @return
	 *   The current lock state of the overlay.
	 */
	LockState GetLockState() const;

	/**
	 * Indicates whether or not the chat widget is visible.
	 * @return
	 *   true if the chat widget is visible, false otherwise.
	 */
	bool IsChatVisible() const;

	/**
	 * Indicates whether or not the game is active.
	 * @return
	 *   true if the game is active, false otherwise.
	 */
	bool IsGameActive() const;

signals:

	/**
	 * Signal sent when the overlay is locked.
	 */
	void Locked();

	/**
	 * Signal sent when the overlay is unlocked.
	 */
	void Unlocked();

	/**
	 * Signal sent when the lock state changes.
	 * @param[in] state
	 *   The current lock state of the overlay.
	 */
	void LockStateChanged(LockState state);

	/**
	 * Signal sent when the chat visibility changes.
	 * @param[in] visibility
	 *   true if the chat window is now visible, false otherwise.
	 */
	void ChatVisibilityChanged(bool visibility);

	/**
	 * Signal sent when the game either becomes active or becomes inactive.
	 * @param[in] active
	 *   true if the game is now active, false otherwise.
	 */
	bool GameActiveChanged(bool active);

public slots:

	/**
	 * Locks the overlay widgets.
	 */
	void Lock();

	/**
	 * Unlocks the overlay widgets.
	 */
	void Unlock();

	/**
	 * Sets the lock state of the overlay widgets.
	 * @param[in] locked
	 *   true if the widgets are to be locked, false if they are to be unlocked.
	 */
	void SetLocked(bool locked);

	/**
	 * Shows the chat widget.
	 */
	void ShowChat();

	/**
	 * Hides the chat widget.
	 */
	void HideChat();

	/**
	 * Sets the visibility of the chat widget.
	 * @param[in] visible
	 *   true if the chat widget is to be visible, false otherwise.
	 */
	void SetChatVisibility(bool visible);

protected:

	/**
	 * Overrides QObject#eventFilter.
	 * This event filter serves the purpose of capturing the keyboard input of the widgets and all of their
	 * children to allow the arrow keys to move the widgets.
	 */
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

private slots:

	/**
	 * Slot called when the timer is triggered to check whether the game is the active window.
	 */
	void OnCheckGameActive();

	/**
	 * Slot called to apply the lock appearance to all widgets.
	 */
	void OnApplyLockAppaerance();

	/**
	 * Slot called to apply the unlock appearance to all widgets.
	 */
	void OnApplyUnlockAppearance();

private:

	/**
	 * Sets the lock state.
	 * @param[in] state
	 *   The new lock state.
	 */
	void SetLockState(LockState state);

	/**
	 * Checks whether the active window is either an overlay window or the game.
	 * @return
	 *   true if the active window is an overlay window or the game, false otherwise.
	 */
	bool IsGameOrOverlayActive() const;

	/**
	 * Change the appearance of a widget to show as locked.
	 * @param[in] widget
	 *   The widget to be locked.
	 */
	void ApplyLockAppearance(QWidget* widget);

	/**
	 * Change the appearance of a widget to show as unlocked.
	 * @param[in] widget
	 *   The widget to be unlocked.
	 */
	void ApplyUnlockAppearance(QWidget* widget);

	/**
	 * Installs the arrow event filter for the given widget and all of its children.
	 * @param[in] widget
	 *   The widget for which to install the event filter.
	 */
	void InstallArrowFilter(QWidget* widget);

	/**
	 * Uninstalls the arrow event filter for the given widget and all of its children.
	 * @param[in] widget
	 *   The widget for which to uninstall the event filter.
	 */
	void UninstallArrowFilter(QWidget* widget);

	/**
	 * Retrieves the root parent widget of the given widget.
	 * @param[in] child
	 *   The child widget to find the root parent of.
	 * @return 
	 *   The root parent of the widget.
	 */
	QWidget* FindRootWidget(QWidget* child) const;

	/**
	 * Sets the game as active or not active.
	 * @param[in] active
	 *   true if the game is active, false otherwise.
	 */
	void SetGameActive(bool active);

	/**
	 * Updates the visibility of windows to account for current states.
	 */
	void UpdateVisibility();

	/**
	 * The current lock state of the overlay.
	 */
	LockState _LockState = OverlayUnlocked;

	/**
	 * The overlay bar widget.
	 */
	QPointer<POverlayBarWidget> _BarWidget;

	/**
	 * The overlay chat widget.
	 */
	QPointer<POverlayChatWidget> _ChatWidget;

	/**
	 * Timer to check the whether the game is the foreground window.
	 */
	QTimer _GameCheckTimer;

	/**
	 * Indicates whether or not the game is active.
	 */
	bool _GameActive = false;

	/**
	 * Indicates whether or not the chat widget should be shown.
	 */
	bool _ChatVisible = false;

};