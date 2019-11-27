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
#include "PVerticalTabBarWidget.h"

PVerticalTabBarWidget::PVerticalTabBarWidget(QWidget* parent /*= nullptr*/):
QWidget(parent)
{
	setupUi(this);
	setLayoutDirection(Qt::LeftToRight);

	connect(_CloseBtn, &QPushButton::clicked, this, &PVerticalTabBarWidget::Closed);
}

QString PVerticalTabBarWidget::GetText() const
{
	return _Label->text();
}

void PVerticalTabBarWidget::SetText(const QString& text)
{
	_Label->setText(text);
}

QColor PVerticalTabBarWidget::GetTextColor() const
{
	return _Label->palette().text().color();
}

void PVerticalTabBarWidget::SetTextColor(const QColor& color)
{
	auto palette = _Label->palette();
	auto brush = palette.text();
	brush.setColor(color);
	palette.setColor(QPalette::Text, color);
	_Label->setPalette(palette);
}

bool PVerticalTabBarWidget::IsClosable() const
{
	return _CloseBtn->isVisible();
}

void PVerticalTabBarWidget::SetClosable(bool state)
{
	_CloseBtn->setVisible(state);
}
