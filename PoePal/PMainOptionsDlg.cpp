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
#include "PMainOptionsDlg.h"
#include "PApplication.h"
#include "PGlobalKeyBind.h"
#include "PGlobalKeyBindManager.h"
#include "PKeyBindEdit.h"
#include <QSettings>

PMainOptionsDlg::PMainOptionsDlg(QWidget* parent):
QDialog(parent)
{
	setupUi(this);

	auto app = qobject_cast<PApplication*>(qApp);
	Q_ASSERT(app);

	connect(this, &QDialog::accepted, app, &PApplication::OptionsChanged);
	
	// Load current settings into the dialog.
	QSettings settings;
	settings.beginGroup(QStringLiteral("MainWindow"));
	switch (settings.value(QStringLiteral("ToolbarDisplayMode"), Qt::ToolButtonIconOnly)
		.value<Qt::ToolButtonStyle>())
	{
	case Qt::ToolButtonIconOnly: _ToolbarDisplayCombo->setCurrentIndex(0); break;
	case Qt::ToolButtonTextBesideIcon: _ToolbarDisplayCombo->setCurrentIndex(1); break;
	case Qt::ToolButtonTextUnderIcon: _ToolbarDisplayCombo->setCurrentIndex(2); break;
	case Qt::ToolButtonTextOnly: _ToolbarDisplayCombo->setCurrentIndex(3); break;
	}
	settings.endGroup(); // MainWindow

	// Initialize the shortcut tree root nodes.
	_BuiltInParentItem = new QTreeWidgetItem(_ShortcutsTree);
	_ShortcutsTree->addTopLevelItem(_BuiltInParentItem);
	_BuiltInParentItem->setText(0, tr("Built-In Shortcuts"));
	_ShortcutsTree->expandItem(_BuiltInParentItem);
	_CustomParentItem = new QTreeWidgetItem(_ShortcutsTree);
	_ShortcutsTree->addTopLevelItem(_CustomParentItem);
	_CustomParentItem->setText(0, tr("Custom"));
	_ShortcutsTree->expandItem(_CustomParentItem);

	// The key binds sorted by their labels.
	auto keyBindMgr = app->GetKeyBindManager();
	Q_ASSERT(keyBindMgr);
	QMap<QString, PGlobalKeyBind*> keyBinds;
	for (const auto& keyBind : keyBindMgr->GetKeyBinds()) keyBinds.insert(keyBind->GetLabel(), keyBind);
	auto iter = keyBinds.constBegin();
	auto end = keyBinds.constEnd();
	for (; iter != end; ++iter) AddKeyBind(iter.value());
	if (_CustomParentItem->childCount() == 0) _CustomParentItem->setHidden(true);
}

void PMainOptionsDlg::accept()
{
	// Save settings from the dialog.
	QSettings settings;
	settings.beginGroup(QStringLiteral("MainWindow"));
	switch (_ToolbarDisplayCombo->currentIndex())
	{
	case 0: settings.setValue(QStringLiteral("ToolbarDisplayMode"), Qt::ToolButtonIconOnly); break;
	case 1: settings.setValue(QStringLiteral("ToolbarDisplayMode"), Qt::ToolButtonTextBesideIcon); break;
	case 2: settings.setValue(QStringLiteral("ToolbarDisplayMode"), Qt::ToolButtonTextUnderIcon); break;
	case 3: settings.setValue(QStringLiteral("ToolbarDisplayMode"), Qt::ToolButtonTextOnly); break;
	}
	settings.endGroup(); // MainWindow

	// Save the key binds.
	auto app = qobject_cast<PApplication*>(qApp);
	auto keyBindMgr = app->GetKeyBindManager();
	Q_ASSERT(keyBindMgr);
	for (const auto& edit : _KeyBindEdits)
	{
		auto keyBind = keyBindMgr->GetKeyBind(edit->GetKeyBindId());
		if (keyBind) keyBind->SetKeySequence(edit->GetKeySequence());
	}
	keyBindMgr->SaveKeyBinds();

	// Save the chat settings.
	_ChatWidgets->Save();

	// Call the base class to tell everybody.
	QDialog::accept();
}

void PMainOptionsDlg::AddKeyBind(PGlobalKeyBind* keyBind)
{
	static QIcon btnIcon;
	if (btnIcon.isNull()) btnIcon.addFile(":/PoePal/Resources/32x32/delete.png");
	auto keyBindLabel = new QLabel();
	keyBindLabel->setText(keyBind->GetLabel());
	auto keyBindEdit = new PKeyBindEdit();
	keyBindEdit->SetKeyBindId(keyBind->GetId());
	keyBindEdit->SetKeySequence(keyBind->GetKeySequence());
	auto keyBindBtn = new QPushButton();
	keyBindBtn->setIcon(btnIcon);
	keyBindBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	connect(keyBindBtn, &QPushButton::clicked, keyBindEdit, &PKeyBindEdit::Clear);
	int row = 0, t = 0;
	QTreeWidgetItem* parentItem = nullptr;
	if (keyBind->IsBuiltIn()) parentItem = _BuiltInParentItem;
	else parentItem = _CustomParentItem;
	auto newItem = new QTreeWidgetItem(parentItem);
	parentItem->addChild(newItem);
	newItem->setText(0, keyBind->GetLabel());
	_ShortcutsTree->setItemWidget(newItem, 1, keyBindEdit);
	_ShortcutsTree->setItemWidget(newItem, 2, keyBindBtn);
	_KeyBindEdits.append(keyBindEdit);
	_ShortcutsTree->resizeColumnToContents(2);
	_ShortcutsTree->resizeColumnToContents(0);
}
