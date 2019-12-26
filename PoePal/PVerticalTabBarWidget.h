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

#include <QFrame>
#include "ui_PChatTabWidget.h"

/**
 * Class provides the control for chat whisper tabs.
 */
class PVerticalTabBarWidget : public QFrame, public Ui::PChatTabWidget
{
	Q_OBJECT;

	/**
	 * The text of the tab.
	 */
	Q_PROPERTY(QString text READ GetText WRITE SetText);

	/**
	 * The color used for text.
	 */
	Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor);

	/**
	 * Indicates whether or not the tab is closable.
	 */
	Q_PROPERTY(bool isClosable READ IsClosable WRITE SetClosable);

public:

	/**
	 * Creates a new instance of this widget.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	PVerticalTabBarWidget(QWidget* parent = nullptr);

	virtual ~PVerticalTabBarWidget() = default;

	/**
	 * Retrieves the text of the tab.
	 * @return
	 *   The tab's text.
	 */
	QString GetText() const;

	/**
	 * Sets the text of the tab.
	 * @param[in] text
	 *   The new text to use as the label for the tab.
	 */
	void SetText(const QString& text);

	/**
	 * Retrieves the color used for text.
	 * @return
	 *   The color used for text.
	 */
	QColor GetTextColor() const;

	/**
	 * Sets the color used for text.
	 * @param[in] color
	 *   The new color to use for text.
	 */
	void SetTextColor(const QColor& color);

	/**
	 * Indicates whether or not the tab is closable.
	 * @return
	 *   true if the tab is closable, false otherwise.
	 */
	bool IsClosable() const;

	/**
	 * Sets whether or not the tab is closable.
	 * @param[in] state
	 *   true if the tab will be closable, false otherwise.
	 */
	void SetClosable(bool state);

signals:

	/**
	 * Signal sent when the tab is closed.
	 */
	void Closed();

};