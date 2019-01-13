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

#include <QDockWidget>
#include "ui_PJSConsoleWidget.h"

/**
 * JavaScript console for running custom code inside the application as a dock widget.
 */
class PJSConsoleWidget : public QDockWidget
{
	Q_OBJECT

public:

	/**
	 * Creates a new instance of this widget.
	 */
	PJSConsoleWidget(QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PJSConsoleWidget();

public slots:

	/**
	 * Executes the entered text.
	 */
	void Execute();

protected:

	/**
	 * Overrides QDockWidget#eventFilter.
	 */
	virtual bool eventFilter(QObject *watched, QEvent *evt) override;

private slots:

	/**
	 * Slot called when the contents of the entry edit change.
	 */
	void OnEntryChanged();

private:

	/**
	 * The previously executed scripts.
	 */
	QStringList _PrevScripts;

	/**
	 * The current previous script.
	 */
	int _CurrPrevScript = 0;

	Ui::PJSConsoleWidget ui;
};
