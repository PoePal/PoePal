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
#include "POverlayBarWidget.h"
#include "PApplication.h"
#include "PMessageHandler.h"
#include "POverlayController.h"
#include <QPropertyAnimation>
#include <QSettings>
#include "windows.h"

POverlayBarWidget::POverlayBarWidget(QWidget* parent /*= nullptr*/):
	QWidget(parent)
{
	setupUi(this);

	// Get the style sheet and apply it here.
	auto app = qobject_cast<PApplication*>(qApp);
	auto controller = app->GetOverlayController();
	setStyleSheet(controller->GetStyleSheet());

	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
	setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

	// Build the config menu.
	QIcon lockIcon;
	lockIcon.addFile(QString::fromUtf8(":/PoePal/Resources/32x32/lock.png"), QSize(), QIcon::Normal,
		QIcon::Off);
	_LockAction = _ConfigMenu.addAction(lockIcon, tr("Lock"));
	QIcon unlockIcon;
	unlockIcon.addFile(QString::fromUtf8(":/PoePal/Resources/32x32/lock_open.png"), QSize(),
		QIcon::Normal, QIcon::Off);
	_UnlockAction = _ConfigMenu.addAction(unlockIcon, tr("Unlock"));
	_UnlockAction->setVisible(false);
	QIcon exitIcon;
	exitIcon.addFile(QString::fromUtf8(":/PoePal/Resources/32x32/cross.png"), QSize(),
		QIcon::Normal, QIcon::Off);
	_ExitAction = _ConfigMenu.addAction(exitIcon, tr("Exit"));
	// Connect the lock/unlock to the controller.
	connect(_LockAction, &QAction::triggered, controller, &POverlayController::Lock);
	connect(_UnlockAction, &QAction::triggered, controller, &POverlayController::Unlock);
	connect(_ExitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
	connect(controller, &POverlayController::LockStateChanged, _LockAction, 
		[this](POverlayController::LockState state) {
			_LockAction->setVisible(state == POverlayController::OverlayUnlocked || 
				state == POverlayController::OverlayUnlockPending);
		});
	connect(controller, &POverlayController::LockStateChanged, _UnlockAction,
		[this](POverlayController::LockState state) {
			_UnlockAction->setVisible(state == POverlayController::OverlayLocked ||
				state == POverlayController::OverlayLockPending);
		});
	_OptionsBtn->setMenu(&_ConfigMenu);

	// Connect the chat button to the controller.
	connect(_ChatBtn, &QPushButton::clicked, controller, &POverlayController::SetChatVisibility);
	connect(controller, &POverlayController::ChatVisibilityChanged, _ChatBtn, &QPushButton::setChecked);

	// Connect the rest of the buttons to the generic method used to handle things.
	connect(_HideoutBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_MenagerieBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_DelveBtn, &QPushButton::clicked, this, &POverlayBarWidget::OnButtonClicked);
	connect(_OptionsBtn, &QPushButton::clicked, _OptionsBtn, &QToolButton::showMenu);

	_CollapseAnimation = new QPropertyAnimation(this, "size");
	_CollapseAnimation->setDuration(100);

	// Restore the bar widget settings.
	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	settings.beginGroup(QStringLiteral("BarWidget"));
	if (settings.contains(QStringLiteral("Geometry")))
	{
		restoreGeometry(settings.value(QStringLiteral("Geometry")).toByteArray());
	}
	settings.endGroup(); // BarWidget
	settings.endGroup(); // Overlay
}

POverlayBarWidget::~POverlayBarWidget()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("Overlay"));
	settings.beginGroup(QStringLiteral("BarWidget"));
	settings.setValue(QStringLiteral("Geometry"), saveGeometry());
	settings.endGroup(); // BarWidget
	settings.endGroup(); // Overlay
}

void POverlayBarWidget::enterEvent(QEvent* evt)
{
	auto collapseSize = _CollapseArea->sizeHint();
	auto fullSize = sizeHint();
	fullSize.setWidth(fullSize.width() + collapseSize.width());
	_CollapseAnimation->setStartValue(size());
	_CollapseAnimation->setEndValue(fullSize);
	_CollapseAnimation->start();
}

void POverlayBarWidget::leaveEvent(QEvent* evt)
{
	auto collapseSize = _CollapseArea->sizeHint();
	auto fullSize = sizeHint();
	_CollapseAnimation->setStartValue(size());
	_CollapseAnimation->setEndValue(fullSize);
	_CollapseAnimation->start();
}

void POverlayBarWidget::OnButtonClicked()
{
	auto button = qobject_cast<QAbstractButton*>(sender());
	Q_ASSERT(button);
	if (!button) return;
	auto app = qobject_cast<PApplication*>(qApp);
	Q_ASSERT(app);
	auto handler = app->GetMessageHandler();
	Q_ASSERT(handler);
	if (button == _HideoutBtn) handler->SendAction(PMessageHandler::Hideout, QString(), false);
	else if (button == _MenagerieBtn) handler->SendAction(PMessageHandler::Menagerie, QString(), false);
	else if (button == _DelveBtn) handler->SendAction(PMessageHandler::Delve, QString(), false);
	else if (button == _OptionsBtn) _ConfigMenu.popup(QCursor::pos());
}
