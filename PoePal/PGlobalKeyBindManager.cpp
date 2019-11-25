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
#include "PGlobalKeyBindManager.h"
#include "PGlobalKeyBind.h"
#include <QSettings>

PGlobalKeyBindManager::PGlobalKeyBindManager() :
UGlobalHotkeys()
{
	connect(this, &UGlobalHotkeys::activated, this, &PGlobalKeyBindManager::OnHotkeyTriggered);
}

QList<PGlobalKeyBind*> PGlobalKeyBindManager::GetKeyBinds() const
{
	return _KeyBindsByStringId.values();
}

PGlobalKeyBind* PGlobalKeyBindManager::GetKeyBind(const QByteArray& id) const
{
	return _KeyBindsByStringId.value(id);
}

PGlobalKeyBind* PGlobalKeyBindManager::RegisterKeyBind(const QByteArray& id, const QString& label,
	const QString& tooltip, const QString& keySequence)
{
	if (_KeyBindsByStringId.contains(id)) return _KeyBindsByStringId.value(id);
	auto keyBind = new PGlobalKeyBind(this);
	keyBind->_IsBuiltIn = false;
	keyBind->_ID = id;
	keyBind->_Label = label;
	keyBind->_Tooltip = tooltip;
	keyBind->_Sequence->FromString(keySequence);
	_KeyBindsByStringId.insert(id, keyBind);
	UpdateKeySequence(keyBind);
	connect(keyBind, &PGlobalKeyBind::KeySequenceChanged, this, &PGlobalKeyBindManager::OnKeySequenceModified);
	return keyBind;
}

PGlobalKeyBind* PGlobalKeyBindManager::RegisterBuiltInKeyBind(const QByteArray& id, const QString& label,
	const QString& tooltip, const QString& keySequence)
{
	auto keyBind = RegisterKeyBind(id, label, tooltip, keySequence);
	keyBind->_IsBuiltIn = true;
	return keyBind;
}

void PGlobalKeyBindManager::RemoveKeyBind(const QByteArray& id)
{
	if (_KeyBindsByStringId.contains(id))
	{
		auto keyBind = _KeyBindsByStringId.take(id);
		keyBind->deleteLater();
	}
}

void PGlobalKeyBindManager::SaveKeyBinds()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("GlobalKeyBinds"));
	auto iter = _KeyBindsByStringId.constBegin();
	auto end = _KeyBindsByStringId.constEnd();
	for (; iter != end; ++iter)
	{
		auto id = iter.key();
		auto keyBind = iter.value();
		if (keyBind->IsBuiltIn())
		{
			settings.beginGroup(QStringLiteral("BuiltIn"));
			settings.setValue(id, keyBind->GetKeySequence());
			settings.endGroup(); // BuiltIn
		}
		else
		{
			settings.beginGroup(QStringLiteral("Custom"));
			settings.beginGroup(id);
			settings.setValue(QStringLiteral("Label"), keyBind->GetLabel());
			settings.setValue(QStringLiteral("Tooltip"), keyBind->GetTooltip());
			settings.setValue(QStringLiteral("KeySeq"), keyBind->GetKeySequence());
			settings.endGroup(); // iter.value->GetId();
			settings.endGroup(); // custom
		}
	}
	settings.endGroup(); // GlobalKeyBinds
}

void PGlobalKeyBindManager::RestoreKeyBinds()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("GlobalKeyBinds"));
	// Load all of the global key binds.
	settings.beginGroup(QStringLiteral("BuiltIn"));
	auto iter = _KeyBindsByStringId.constBegin();
	auto end = _KeyBindsByStringId.constEnd();
	for (; iter != end; ++iter)
	{
		auto keyBind = iter.value();
		if (!keyBind->IsBuiltIn()) continue;
		keyBind->SetKeySequence(settings.value(iter.key()).toString());
		UpdateKeySequence(keyBind);
	}
	settings.endGroup(); // BuiltIn
	// Load all of the custom key binds.
	settings.beginGroup(QStringLiteral("Custom"));
	for (const auto& idStr : settings.childKeys())
	{
		auto id = idStr.toLatin1();
		settings.beginGroup(id);
		auto label = settings.value(QStringLiteral("Label")).toString();
		auto tooltip = settings.value(QStringLiteral("Tooltip")).toString();
		auto keySeq = settings.value(QStringLiteral("KeySeq")).toString();
		auto keyBind = _KeyBindsByStringId.value(id, nullptr);
		if (!keyBind)
		{
			keyBind = new PGlobalKeyBind(this);
			_KeyBindsByStringId.insert(id, keyBind);
		}
		keyBind->_IsBuiltIn = false;
		keyBind->_ID = id;
		keyBind->_Label = label;
		keyBind->_Tooltip = tooltip;
		keyBind->_Sequence->FromString(keySeq);
		UpdateKeySequence(keyBind);
		settings.endGroup(); // id
	}
	settings.endGroup(); // Custom
	settings.endGroup(); // GlobalKeyBinds

}

void PGlobalKeyBindManager::OnKeySequenceModified()
{
	auto keyBind = qobject_cast<PGlobalKeyBind*>(sender());
	Q_ASSERT(keyBind);
	if (!keyBind) return;
	UpdateKeySequence(keyBind);
}

void PGlobalKeyBindManager::OnHotkeyTriggered(size_t id)
{
	auto keyBind = _KeyBindsBySysId.value(id, nullptr);
	if (keyBind) emit keyBind->Triggered();
}

void PGlobalKeyBindManager::UpdateKeySequence(PGlobalKeyBind* keyBind)
{
	auto id = GetKeyBindSystemId(keyBind);
	// If there is no key sequence now, check if there was one. If there was one, remove it.
	if (keyBind->_Sequence->Size() == 0)
	{
		if (id != 0)
		{
			_KeyBindsBySysId.remove(id);
			unregisterHotkey(id);
			keyBind->setProperty("sysid", 0);
		}
	}
	else
	{
		if (id == 0)
		{
			id = ++_LastId; // it didn't have an ID yet, so use the next one.
			keyBind->setProperty("sysid", id);
			_KeyBindsBySysId.insert(id, keyBind);
		}
		registerHotkey(keyBind->GetKeySequence(), id);
	}

}

QQmlListProperty<PGlobalKeyBind> PGlobalKeyBindManager::GetKeyBindsProperty() const
{
	static QQmlListProperty<PGlobalKeyBind>::CountFunction countFunc = [](
		QQmlListProperty<PGlobalKeyBind>* prop)
	{
		auto manager = qobject_cast<PGlobalKeyBindManager*>(prop->object);
		Q_ASSERT(manager);
		if (!manager) return 0;
		return manager->GetKeyBinds().length();
	};
	static QQmlListProperty<PGlobalKeyBind>::AtFunction atFunc = [](QQmlListProperty<PGlobalKeyBind>* prop,
		int index)->PGlobalKeyBind*
	{
		auto manager = qobject_cast<PGlobalKeyBindManager*>(prop->object);
		Q_ASSERT(manager);
		if (!manager) return nullptr;
		return manager->GetKeyBinds().at(index);
	};
	return QQmlListProperty<PGlobalKeyBind>(const_cast<PGlobalKeyBindManager*>(this), nullptr, countFunc, 
		atFunc);
}

size_t PGlobalKeyBindManager::GetKeyBindSystemId(PGlobalKeyBind* keyBind) const
{
	return keyBind->property("sysid").toULongLong();
}
