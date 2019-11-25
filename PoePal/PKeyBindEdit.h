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

class QLineEdit;
class UKeySequence;

/**
 * Editor widget for key binds.
 */
class PKeyBindEdit : public QWidget
{
	Q_OBJECT;

	/**
	 * The ID of the key bind being edited by this widget.
	 */
	Q_PROPERTY(QByteArray keyBindId READ GetKeyBindId WRITE SetKeyBindId)

public:

	/**
	 * Creates a new instance of this widget.
	 * @param[in] parent
	 *   The parent widget.
	 */
	PKeyBindEdit(QWidget* parent = nullptr);

	virtual ~PKeyBindEdit() = default;

	/**
	 * Retrieves the ID of the key bind being edited by this widget.
	 * @return
	 *   The ID of the key bind being edited by this widget.
	 */
	QByteArray GetKeyBindId() const;

	/**
	 * Sets the ID of the key bind being edited by this widget.
	 * @param[in] keyBindId
	 *   The ID of the new key bind to be edited by this widget.
	 */
	void SetKeyBindId(const QByteArray& keyBindId);

	/**
	 * Retrieves the current key sequence.
	 * @return
	 *   The current key sequence defined in the key bind.
	 */
	QString GetKeySequence() const;

	/**
	 * Sets the current key sequence.
	 * @param[in] keySeq
	 *   The current key sequence.
	 */
	void SetKeySequence(const QString& keySeq);

public slots:

	/**
	 * Clears the key bind.
	 */
	void Clear();

protected:
	
	/**
	 * Overrides QWidget#event.
	 */
	bool event(QEvent* evt) override;

	/**
	 * Overrides QWidget#keyPressEvent.
	 */
	void keyPressEvent(QKeyEvent* evt) override;

	/**
	 * Overrides QWidget#keyReleaseEvent.
	 */
	void keyReleaseEvent(QKeyEvent* evt) override;

private:

	/**
	 * Resets the editor for new key sequence recording.
	 * This gets called each time the editor takes focus or the ID of the key bind changes.
	 */
	void Reset();

	/**
	 * The ID of the key bind being edited by this widget.
	 */
	QByteArray _KeyBindId;

	/**
	 * The line editor that displays the user interacts with.
	 */
	QLineEdit* _LineEdit = nullptr;

	/**
	 * The current key sequence.
	 */
	QScopedPointer<UKeySequence> _KeySequence;
};