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
#include "PKeyBindEdit.h"
#include <QKeyEvent>
#include <QLineEdit>
#include <QVBoxLayout>
#include "ukeysequence.h"

PKeyBindEdit::PKeyBindEdit(QWidget* parent /*= nullptr*/):
QWidget(parent)
{
	_LineEdit = new QLineEdit(this);
	_LineEdit->setObjectName(QStringLiteral("pkeybindedit_lineedit"));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(_LineEdit);

	_LineEdit->setFocusProxy(this);
	_LineEdit->installEventFilter(this);
	_LineEdit->setPlaceholderText(tr("Click to set"));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_MacShowFocusRect, true);
	setAttribute(Qt::WA_InputMethodEnabled, false);

	Reset();
}

QByteArray PKeyBindEdit::GetKeyBindId() const
{
	return _KeyBindId;
}

void PKeyBindEdit::SetKeyBindId(const QByteArray& keyBindId)
{
	_KeyBindId = keyBindId;
	Reset();
}

QString PKeyBindEdit::GetKeySequence() const
{
	return _LineEdit->text();
}

void PKeyBindEdit::SetKeySequence(const QString& keySeq)
{
	_LineEdit->setText(keySeq);
}

void PKeyBindEdit::Clear()
{
	Reset();
	_LineEdit->clear();
}

void PKeyBindEdit::keyPressEvent(QKeyEvent* evt)
{
	_KeySequence->AddKey(evt);
}

void PKeyBindEdit::keyReleaseEvent(QKeyEvent* evt)
{
	if(_KeySequence->Size() > 0) _LineEdit->setText(_KeySequence->ToString());
	Reset();
}

void PKeyBindEdit::Reset()
{
	_KeySequence.reset(new UKeySequence());
}

bool PKeyBindEdit::event(QEvent* evt)
{
	switch (evt->type())
	{
	case QEvent::Shortcut:
		return true;
	case QEvent::ShortcutOverride:
		evt->accept();
		return true;
	default: 
		break;
	}
	return QWidget::event(evt);
}
