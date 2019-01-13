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
#include "PJSConsoleWidget.h"
#include "PApplication.h"
#include <QJSEngine>
#include <QTextStream>

PJSConsoleWidget::PJSConsoleWidget(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);
	connect(ui._EntryEdit->document(), &QTextDocument::contentsChange, this, 
		&PJSConsoleWidget::OnEntryChanged);
	ui._EntryEdit->installEventFilter(this);
}

PJSConsoleWidget::~PJSConsoleWidget()
{
}

void PJSConsoleWidget::Execute()
{
	auto script = ui._EntryEdit->toPlainText();
	if (script.isEmpty())
	{
		ui._DisplayEdit->appendHtml("<pre>%&gt;</pre>");
		return;
	}
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto value = app->GetJSEngine()->evaluate(script);
	QString color = value.isError() ? "red" : "black";
	QTextStream stream(&script);
	QString line;
	QString prefix('%');
	while (stream.readLineInto(&line))
	{
		ui._DisplayEdit->appendHtml("<pre>" + prefix + "&gt;" + line + "</pre>");
		prefix = "&hellip;";
	}
	ui._DisplayEdit->appendHtml("<pre style=\"color: " + color + "\">" +
		value.toString().toHtmlEscaped() + "</pre>");
	_PrevScripts.append(script);
	_CurrPrevScript = 0;
	ui._EntryEdit->clear();
}

bool PJSConsoleWidget::eventFilter(QObject *watched, QEvent *evt)
{
	if (evt->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evt);
		if (keyEvent->key() == Qt::Key_Return && !keyEvent->modifiers().testFlag(Qt::ShiftModifier))
		{
			Execute();
			return true;
		}
		else if (keyEvent->key() == Qt::Key_Up)
		{
			auto idx = _PrevScripts.length() - _CurrPrevScript - 1;
			if (idx >= 0) ui._EntryEdit->setText(_PrevScripts.at(idx));
			if (idx > 0) _CurrPrevScript++;
			auto cursor = ui._EntryEdit->textCursor();
			cursor.movePosition(QTextCursor::End);
			ui._EntryEdit->setTextCursor(cursor);
		}
		else if (keyEvent->key() == Qt::Key_Down)
		{
			auto idx = _PrevScripts.length() - _CurrPrevScript;
			if (_CurrPrevScript > 0) ui._EntryEdit->setText(_PrevScripts.at(idx));
			else ui._EntryEdit->clear();
			if (_CurrPrevScript > 0) _CurrPrevScript--;
			auto cursor = ui._EntryEdit->textCursor();
			cursor.movePosition(QTextCursor::End);
			ui._EntryEdit->setTextCursor(cursor);
		}
	}
	return QDockWidget::eventFilter(watched, evt);
}

void PJSConsoleWidget::OnEntryChanged()
{
	auto doc = ui._EntryEdit->document();
	doc->setTextWidth(ui._EntryEdit->width() - 2);
	auto height = doc->size().height();

	ui._EntryEdit->setMinimumHeight(height + 2);
}
