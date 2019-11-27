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

#include <QWidget>
#include "ui_PVerticalTabWidget.h"

/**
 * Provides a tab widget with vertical tabs on the left side of the tab contents with horizontal text.
 */
class PVerticalTabWidget : public QWidget, public Ui::PVerticalTabWidget
{
	Q_OBJECT;

	/**
	 * The number of tabs in the widget.
	 */
	Q_PROPERTY(int count READ count);

	/**
	 * The index of the current tab.
	 */
	Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged);

	/**
	 * Indicates whether tabs are closable.
	 */
	Q_PROPERTY(bool tabsClosable READ tabsClosable WRITE setTabsClosable);

public:

	/**
	 * Creates a new tab widget.
	 * @param[in] parent
	 *   The parent of the tab widget.
	 */
	PVerticalTabWidget(QWidget* parent = nullptr);

	virtual ~PVerticalTabWidget() = default;

	/**
	 * Adds a new tab.
	 * @param[in] page
	 *   The page associated to the tab.
	 * @param[in] label
	 *   The label for the tab.
	 * @return
	 *   The index of the new tab.
	 */
	int addTab(QWidget* page, const QString& label);

	/**
	 * Retrieves the number of tabs.
	 * @return
	 *   The number of tabs in the widget.
	 */
	int count() const;

	/**
	 * Retrieves the index of the current tab.
	 * @return
	 *   The index of the current tab.
	 */
	int currentIndex() const;

	/**
	 * Retrieves index of a widget.
	 * @param[in] widget
	 *   The widget for which to retrieve the index.
	 * @return
	 *   The index of the widget.
	 */
	int indexOf(QWidget* widget) const;

	/**
	 * Inserts a new tab.
	 * @param[in] index
	 *   The index at which to insert the tab. If the index is out of range, the tab is appended.
	 * @param[in] page
	 *   The page associated to the tab.
	 * @param[in] label
	 *   The label for the tab.
	 * @return
	 *   The index of the new tab.
	 */
	int insertTab(int index, QWidget* page, const QString& label);

	/**
	 * Sets whether or not tabs are closable.
	 * @param[in] state
	 *   true if the tabs will now be closable, false otherwise.
	 */
	void setTabsClosable(bool state);

	/**
	 * Sets the text of a tab.
	 * @param[in] index
	 *   The index of the tab for which to set the text.
	 * @param[in] text
	 *   The new text of the tab.
	 */
	void setTabText(int index, const QString& text);

	/**
	 * Retrieves the tab bar for the widget.
	 * @return
	 *   The widget's tab bar.
	 */
	PVerticalTabBar* tabBar() const;

	/**
	 * Indicates whether or not tabs are closable.
	 * @return
	 *   true if the tabs are closable, false otherwise.
	 */
	bool tabsClosable() const;

	/**
	 * Retrieves the text of a tab.
	 * @param[in] index
	 *   The index of the tab for which to retrieve the text.
	 * @return
	 *   The text of the tab.
	 */
	QString tabText(int index) const;

	/**
	 * Retrieves the widget at the given index.
	 * @param[in] index
	 *   The index of the widget to retrieve.
	 * @return
	 *   The widget at the given index.
	 */
	QWidget* widget(int index) const;

public slots:

	/**
	 * Sets the current tab by index.
	 * @param[in] index
	 *   The index of the new current tab.
	 */
	void setCurrentIndex(int index);

	/**
	 * Sets the current tab by widget.
	 * @param[in] widget
	 *   The widget to become the current tab.
	 */
	void setCurrentWidget(QWidget* widget);

signals:

	/**
	 * Signal sent when the current index changes.
	 * @param[in] index
	 *   The index of the now current tab.
	 */
	void currentChanged(int index);

	/**
	 * Signal sent when a tab close is requested.
	 * @param[in] index
	 *   The index of the tab that has been requested to close.
	 */
	void tabCloseRequested(int index);

private slots:

	/**
	 * Slot called when a tab is closed.
	 * @param[in] index
	 *   The index of the tab being closed.
	 */
	void OnTabClosed(int index);

};