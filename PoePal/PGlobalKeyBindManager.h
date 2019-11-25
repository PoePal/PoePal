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

#include <QQmlListProperty>
#include <uglobalhotkeys.h>

class PGlobalKeyBind;

/**
 * Manages all of the key binds that have been defined, including built-in and user-defined key binds.
 */
class PGlobalKeyBindManager : public UGlobalHotkeys
{
	Q_OBJECT;

	/**
	 * The list of key binds that have been registered.
	 */
	Q_PROPERTY(QQmlListProperty<PGlobalKeyBind> keyBinds READ GetKeyBindsProperty);

public:

	/**
	 * Creates a new key bind manager.
	 */
	PGlobalKeyBindManager();

	/**
	 * Retrieves the list of key binds from the manager.
	 * @return
	 *   The list of key binds that have been registered.
	 */
	QList<PGlobalKeyBind*> GetKeyBinds() const;

	/**
	 * Retrieves a key bind by its ID.
	 * @param[in] id
	 *   The ID of the key bind to retrieve.
	 * @return
	 *   The key bind with the given ID.
	 */
	PGlobalKeyBind* GetKeyBind(const QByteArray& id) const;

	/**
	 * Registers a new custom key bind.
	 * @param[in] id
	 *   The ID of the key bind. If this ID has already been used, the key bind that this ID was used for is 
	 *   returned.
	 * @param[in] label
	 *   The label for the key bind.
	 * @param[in] tooltip
	 *   The tooltip for the key bind.
	 * @param[in] defaultKeySequence
	 *   The default key sequence for the key bind. If this is empty, the key bind will not have a keyboard 
	 *   shortcut by default, but it will still be configurable in the options dialog.
	 * @return
	 *   The new key bind.
	 */
	Q_INVOKABLE PGlobalKeyBind* RegisterKeyBind(const QByteArray& id, const QString& label, 
		const QString &tooltip, const QString& keySequence);

	/**
	 * Registers a new built-in key bind.
	 * @param[in] id
	 *   The ID of the key bind. If this ID has already been used, the key bind that this ID was used for is
	 *   returned.
	 * @param[in] label
	 *   The label for the key bind.
	 * @param[in] tooltip
	 *   The tooltip for the key bind.
	 * @param[in] defaultKeySequence
	 *   The default key sequence for the key bind. If this is empty, the key bind will not have a keyboard 
	 *   shortcut by default, but it will still be configurable in the options dialog.
	 * @return
	 *   The new key bind.
	 */
	PGlobalKeyBind* RegisterBuiltInKeyBind(const QByteArray& id, const QString& label, const QString& tooltip,
		const QString& keySequence);

	/**
	 * Removes the key bind with the given ID.
	 * @param[in] id
	 *   The ID of the key bind to remove.
	 */
	Q_INVOKABLE void RemoveKeyBind(const QByteArray& id);

	/**
	 * Saves the key binds to settings.
	 */
	Q_INVOKABLE void SaveKeyBinds();

	/**
	 * Loads the key binds from settings.
	 */
	Q_INVOKABLE void RestoreKeyBinds();

private slots:

	/**
	 * Slot called when a key bind's key sequence is modified.
	 */
	void OnKeySequenceModified();

	/**
	 * Slot called when a hotkey is triggered.
	 * @param[in] id
	 *   The ID of the hotkey that was triggered.
	 */
	void OnHotkeyTriggered(size_t id);

private:

	/**
	 * Updates the key sequence for a key bind.
	 * @param[in] keyBind
	 *   The key bind for which to update the key sequence.
	 */
	void UpdateKeySequence(PGlobalKeyBind* macro);

	/**
	 * Retrieves the list of key binds for the property.
	 * @return
	 *   The QML list property containing all of the key binds.
	 */
	QQmlListProperty<PGlobalKeyBind> GetKeyBindsProperty() const;

	/**
	 * Retrieves the system ID for a key bind.
	 * @param[in] keyBind
	 *   The key bind for which to retrieve the system ID.
	 * @return
	 *   The ID of the key bind. This will be zero if there was no ID set on the key bind.
	 */
	size_t GetKeyBindSystemId(PGlobalKeyBind* keyBind) const;

	/**
	 * The key binds indexed by their string id.
	 */
	QMap<QByteArray, PGlobalKeyBind*> _KeyBindsByStringId;

	/**
	 * The key binds indexed by system id.
	 */
	QHash<size_t, PGlobalKeyBind*> _KeyBindsBySysId;

	/**
	 * The last hotkey ID.
	 */
	size_t _LastId = 0;


	friend class PGlobalKeyBind;
};