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
#include "PVerticalTabWidget.h"
#include "PVerticalTabBarWidget.h"

PVerticalTabWidget::PVerticalTabWidget(QWidget* parent /*= nullptr*/):
QWidget(parent)
{
	setupUi(this);
	connect(_TabBar, &PVerticalTabBar::currentChanged, _Pages, &QStackedWidget::setCurrentIndex);
	connect(_Pages, &QStackedWidget::currentChanged, this, &PVerticalTabWidget::currentChanged);
	connect(_TabBar, &PVerticalTabBar::tabCloseRequested, this, &PVerticalTabWidget::OnTabClosed);
}

int PVerticalTabWidget::addTab(QWidget* page, const QString& label)
{
	return insertTab(count(), page, label);
}

int PVerticalTabWidget::count() const
{
	return _Pages->count();
}

int PVerticalTabWidget::currentIndex() const
{
	return _Pages->currentIndex();
}

int PVerticalTabWidget::indexOf(QWidget* widget) const
{
	return _Pages->indexOf(widget);
}

int PVerticalTabWidget::insertTab(int index, QWidget* page, const QString& label)
{
	if (index >= 0 && index <= count())
	{
		_TabBar->insertTab(index, label);
		_Pages->insertWidget(index, page);
		return index;
	}
	else return addTab(page, label);
}

void PVerticalTabWidget::setTabsClosable(bool state)
{
	_TabBar->setTabsClosable(state);
}

void PVerticalTabWidget::setTabText(int index, const QString& text)
{
	_TabBar->setTabText(index, text);
}

PVerticalTabBar* PVerticalTabWidget::tabBar() const
{
	return _TabBar;
}

bool PVerticalTabWidget::tabsClosable() const
{
	return _TabBar->tabsClosable();
}

QString PVerticalTabWidget::tabText(int index) const
{
	return _TabBar->tabText(index);
}

QWidget* PVerticalTabWidget::widget(int index) const
{
	return _Pages->widget(index);
}

void PVerticalTabWidget::setCurrentIndex(int index)
{
	_TabBar->setCurrentIndex(index);
}

void PVerticalTabWidget::setCurrentWidget(QWidget* widget)
{
	int idx = _Pages->indexOf(widget);
	if (idx >= 0) setCurrentIndex(idx);
}

void PVerticalTabWidget::OnTabClosed(int index)
{
	emit tabCloseRequested(index);
	auto widget = _Pages->widget(index);
	if (widget) _Pages->removeWidget(widget);
}
