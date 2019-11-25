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

#include <ukeysequence.h>
#include <QObject>

/**
 * A macro that can be triggered via a global keyboard shortcut.
 */
class PGlobalKeyBind : public QObject
{
	Q_OBJECT;

	/**
	 * Indicates whether or not the macro is built-in.
	 */
	Q_PROPERTY(bool isBuiltIn READ IsBuiltIn)

	/**
	 * The ID of the macro.
	 */
	Q_PROPERTY(QByteArray id READ GetId)

	/**
	 * The label for the macro.
	 * This is only writable for macros that are not built-in.
	 */
	Q_PROPERTY(QString label READ GetLabel WRITE SetLabel)

	/**
	 * The tooltip for the macro.
	 * This is only writable for macros that are not built-in.
	 */
	Q_PROPERTY(QString tooltip READ GetTooltip WRITE SetTooltip)

	/**
	 * The key sequence that triggers the macro as a string.
	 */
	Q_PROPERTY(QString keySequence READ GetKeySequence WRITE SetKeySequence)

public:

	/**
	 * Indicates whether or not the macro is a built-in macro.
	 * @return
	 *   true if the macro is a built-in macro, false otherwise.
	 */
	bool IsBuiltIn() const;

	/**
	 * Retrieves the ID of the macro.
	 * @return
	 *   The ID of the macro.
	 */
	QByteArray GetId() const;

	/**
	 * Retrieves the label of the macro.
	 * @return
	 *   The label of the macro.
	 */
	QString GetLabel() const;

	/**
	 * Sets the label of the macro.
	 * This method does nothing if this is a built-in macro.
	 * @param[in] label
	 *   The new label for the macro.
	 */
	void SetLabel(const QString& label);

	/**
	 * Retrieves the tooltip of the macro.
	 * @return
	 *   The tooltip of the macro.
	 */
	QString GetTooltip() const;

	/**
	 * Sets the tooltip for the macro.
	 * This method does nothing if this is a built-in macro.
	 * @param[in] tooltip
	 *   The new tooltip for the macro.
	 */
	void SetTooltip(const QString& tooltip);

	/**
	 * Retrieves the key sequence that triggers the macro.
	 * @return
	 *   The key sequence that triggers the macro.
	 */
	QString GetKeySequence() const;

	/**
	 * Sets the key sequence that triggers the macro from a string.
	 * @param[in] sequenceStr
	 *   The string representation of the new key sequence.
	 */
	void SetKeySequence(const QString& sequence);

signals:

	/**
	 * Sent when the macro is triggered.
	 */
	void Triggered();

	/**
	 * Signal sent when the key sequence changes.
	 * @param[in] sequence
	 *   The new key sequence.
	 */
	void KeySequenceChanged(const UKeySequence& sequence);

private:

	/**
	 * Creates a new macro.
	 * @param[in] parent
	 *   The parent of the macro.
	 */
	PGlobalKeyBind(QObject* parent);

	/**
	 * Indicates whether or not the macro is built-in.
	 */
	bool _IsBuiltIn = false;

	/**
	 * The ID of the macro.
	 */
	QByteArray _ID;

	/**
	 * The label of the macro.
	 */
	QString _Label;

	/**
	 * The tooltip of the macro.
	 */
	QString _Tooltip;

	/**
	 * The key sequence that triggers the macro.
	 */
	QScopedPointer<UKeySequence> _Sequence;

	friend class PGlobalKeyBindManager;
};