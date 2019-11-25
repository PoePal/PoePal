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

#include <QDialog>
#include "ui_PMainOptionsDlg.h"

class PGlobalKeyBind;
class PKeyBindEdit;

/**
 * Dialog providing the UI to edit application-wide options.
 */
class PMainOptionsDlg : public QDialog, public Ui::PMainOptionsDlg
{
	Q_OBJECT;
public:

	/**
	 * Creates a new instance of this dialog.
	 * @param[in] parent
	 *   The parent of the dialog.
	 */
	PMainOptionsDlg(QWidget* parent);

	/**
	 * Overrides QDialog#accept()
	 */
	virtual void accept() override;

private:

	/**
	 * Adds a key bind to the options UI.
	 * @param[in] keyBind
	 *   The key bind to add to the options UI.
	 */
	void AddKeyBind(PGlobalKeyBind* keyBind);

	/**
	 * The list of key bind edits.
	 */
	QList<PKeyBindEdit*> _KeyBindEdits;

	/**
	 * The tree widget item for built-in key binds.
	 */
	QTreeWidgetItem* _BuiltInParentItem = nullptr;

	/**
	 * The tree widget item for custom key binds.
	 */
	QTreeWidgetItem* _CustomParentItem = nullptr;
};