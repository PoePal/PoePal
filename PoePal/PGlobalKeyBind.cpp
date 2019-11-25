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
#include "PGlobalKeyBind.h"
#include "PApplication.h"
#include "PGlobalKeyBindManager.h"

PGlobalKeyBind::PGlobalKeyBind(QObject* parent):
QObject(parent),_Sequence(new UKeySequence())
{

}

bool PGlobalKeyBind::IsBuiltIn() const
{
	return _IsBuiltIn;
}

QByteArray PGlobalKeyBind::GetId() const
{
	return _ID;
}

QString PGlobalKeyBind::GetLabel() const
{
	return _Label;
}

void PGlobalKeyBind::SetLabel(const QString& label)
{
	if (!_IsBuiltIn) _Label = label;
}

QString PGlobalKeyBind::GetTooltip() const
{
	return _Tooltip;
}

void PGlobalKeyBind::SetTooltip(const QString& tooltip)
{
	if (!_IsBuiltIn) _Tooltip = tooltip;
}

QString PGlobalKeyBind::GetKeySequence() const
{
	return const_cast<PGlobalKeyBind*>(this)->_Sequence->ToString(); // fix const issue in UKeySequence
}

void PGlobalKeyBind::SetKeySequence(const QString& sequence)
{
	if (_Sequence->ToString().toLower() != sequence.toLower())
	{
		_Sequence.reset(new UKeySequence());
		if (!sequence.isEmpty()) _Sequence->FromString(sequence);
		emit KeySequenceChanged(*_Sequence);
	}
}

