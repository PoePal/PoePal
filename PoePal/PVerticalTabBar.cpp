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
#include "PVerticalTabBar.h"
#include "PVerticalTabBarWidget.h"
#include <QListWidget>
#include <QScrollBar>

namespace {
	static QString _ListStylesheet(
		"QListWidget {"
		"	background: transparent;"
		"	border-right: 1px solid gray;"
		"}"

		".QListWidget::item{"
		"	background: transparent;"
		"	margin-left: 4px;"
		"	paddingt: 4px;"
		"	border-left: 1px solid #CCC;"
		"	border-top: 1px solid #CCC;"
		"	border-bottom: 1px solid #CCC;"
		"}"

		".QListWidget::item:selected{"
		"	margin-left: 1px;"
		"	margin-right: -1px;"
		"	background: white;"
		"	border-right: 1px solid white;"
		"}"

	);
}

PVerticalTabBar::PVerticalTabBar(QWidget* parent /*= nullptr*/):
QWidget(parent)
{
	_List = new QListWidget(this);
	setStyleSheet(_ListStylesheet);
	_List->setFrameShape(QFrame::NoFrame);
	_List->setFocusPolicy(Qt::NoFocus);
	_List->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_List->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	_List->setLayoutDirection(Qt::RightToLeft);
	_List->setResizeMode(QListView::Adjust);
	auto policy = _List->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::Minimum);
	_List->setSizePolicy(policy);
	auto layout = new QVBoxLayout(this);
	layout->addWidget(_List);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);
	connect(_List, &QListWidget::currentRowChanged, this, &PVerticalTabBar::currentChanged);
}

QListWidget* PVerticalTabBar::GetListWidget() const
{
	return _List;
}

int PVerticalTabBar::addTab(const QString& text)
{
	return insertTab(count(), text);
}

int PVerticalTabBar::count() const
{
	return _List->count();
}

int PVerticalTabBar::currentIndex() const
{
	return _List->currentRow();
}

int PVerticalTabBar::insertTab(int index, const QString& text)
{
	if (index >= 0 && index <= count())
	{
		auto itemWidget = new PVerticalTabBarWidget();
		itemWidget->SetText(text);
		itemWidget->SetClosable(_TabsClosable);
		auto item = new QListWidgetItem();
		_List->insertItem(index, item);
		_List->setItemWidget(item, itemWidget);
		connect(itemWidget, &PVerticalTabBarWidget::Closed, this, &PVerticalTabBar::OnTabClose);
		_List->setMinimumSize(_List->sizeHint());
		UpdateMinimumSize();
	}
	else return addTab(text);
	return index;
}

bool PVerticalTabBar::isTabEnabled(int index) const
{
	auto item = _List->item(index);
	if (item) return item->flags().testFlag(Qt::ItemIsEnabled);
	return false;
}

void PVerticalTabBar::moveTab(int from, int to)
{
	if (from == to) return;
	auto text = tabText(from);
	auto color = tabTextColor(from);
	auto item = _List->takeItem(from);
	if (item)
	{
		auto itemWidget = new PVerticalTabBarWidget();
		itemWidget->SetText(text);
		itemWidget->SetTextColor(color);
		itemWidget->SetClosable(_TabsClosable);
		_List->insertItem(to, item);
		_List->setItemWidget(item, itemWidget);
		connect(itemWidget, &PVerticalTabBarWidget::Closed, this, &PVerticalTabBar::OnTabClose);
	}
}

void PVerticalTabBar::removeTab(int index)
{
	auto item = _List->takeItem(index);
	if (item) delete item;
	UpdateMinimumSize();
}

void PVerticalTabBar::setTabData(int index, const QVariant& data)
{
	auto item = _List->item(index);
	if (item) item->setData(Qt::UserRole, data);
}

void PVerticalTabBar::setTabEnabled(int index, bool state)
{
	auto item = _List->item(index);
	if (item) item->setFlags(item->flags() | Qt::ItemIsEnabled);
}

void PVerticalTabBar::setTabText(int index, const QString& text)
{
	auto item = _List->item(index);
	if (item)
	{
		auto widget = qobject_cast<PVerticalTabBarWidget*>(_List->itemWidget(item));
		if (widget) widget->SetText(text);
		setMinimumWidth(_List->sizeHintForColumn(0) + 16);
	}
}

void PVerticalTabBar::setTabTextColor(int index, const QColor& color)
{
	auto item = _List->item(index);
	if (item)
	{
		auto widget = qobject_cast<PVerticalTabBarWidget*>(_List->itemWidget(item));
		if (widget) widget->SetTextColor(color);
	}
}

void PVerticalTabBar::setTabToolTip(int index, const QString& toolTip)
{
	auto item = _List->item(index);
	if (item) item->setToolTip(toolTip);
}

void PVerticalTabBar::setTabsClosable(bool state)
{
	if (_TabsClosable == state) return;
	_TabsClosable = state;
	int numTabs = count();
	for (int t = 0; t < numTabs; ++t)
	{
		auto item = _List->item(t);
		if (!item) continue;
		auto widget = qobject_cast<PVerticalTabBarWidget*>(_List->itemWidget(item));
		if (!widget) continue;
		widget->SetClosable(state);
	}
}

int PVerticalTabBar::tabAt(const QPoint& pos) const
{
	auto item = _List->itemAt(pos);
	if (item) return _List->row(item);
	return -1;
}

QVariant PVerticalTabBar::tabData(int index) const
{
	auto item = _List->item(index);
	if (item) return item->data(Qt::UserRole);
	return QVariant();
}

QRect PVerticalTabBar::tabRect(int index) const
{
	auto item = _List->item(index);
	if (item) return _List->visualItemRect(item);
	return QRect();
}

QString PVerticalTabBar::tabText(int index) const
{
	auto item = _List->item(index);
	if (item)
	{
		auto widget = qobject_cast<PVerticalTabBarWidget*>(_List->itemWidget(item));
		if (widget) return widget->GetText();
	}
	return QString();
}

QColor PVerticalTabBar::tabTextColor(int index) const
{
	auto item = _List->item(index);
	if (item)
	{
		auto widget = qobject_cast<PVerticalTabBarWidget*>(_List->itemWidget(item));
		if (widget) widget->GetTextColor();
	}
	return QColor();
}

QString PVerticalTabBar::tabToolTip(int index) const
{
	auto item = _List->item(index);
	if (item) return item->toolTip();
	return QString();
}

bool PVerticalTabBar::tabsClosable() const
{
	return _TabsClosable;
}

void PVerticalTabBar::setCurrentIndex(int index)
{
	_List->setCurrentRow(index);
}

void PVerticalTabBar::resizeEvent(QResizeEvent* event)
{
	UpdateMinimumSize();
	QWidget::resizeEvent(event);
}

void PVerticalTabBar::OnTabClose()
{
	auto tab = sender();
	int numTabs = count();
	for (int t = 0; t < numTabs; ++t)
	{
		auto item = _List->item(t);
		if (item && _List->itemWidget(item) == tab)
		{
			emit tabCloseRequested(t);
			removeTab(t);
			return;
		}
	}
}

void PVerticalTabBar::UpdateMinimumSize()
{
	auto width = _List->sizeHintForColumn(0);
	if (_List->verticalScrollBar()->isVisible()) width += _List->verticalScrollBar()->width();
	setMinimumWidth(width + 4);
}
