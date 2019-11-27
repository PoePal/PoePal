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

class QListWidget;

/**
 * The tab bar for vertical tab widgets.
 * This is intended to be implemented as a drop-in replacement for QTabBar for most uses, though it does not
 * directly derive from QTabBar for implementation reasons. It is easier to implement this way rather than
 * trying to redraw the tab bar with horizontal text when the tab bar is vertical.
 */
class PVerticalTabBar : public QWidget
{
	Q_OBJECT;

	/**
	 * The number of tabs in the tab bar.
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
	 * Creates a new tab bar.
	 * @param[in] parent
	 *   The parent of the tab bar.
	 */
	PVerticalTabBar(QWidget* parent = nullptr);

	/**
	 * Adds a tab to the tab bar.
	 * @param[in] text
	 *   The text to put on the tab.
	 * @return
	 *   The index where the tab was inserted.
	 */
	int addTab(const QString& text);

	/**
	 * Retrieves the number of tabs in the tab bar.
	 * @return
	 *   The number of tabs in the tab bar.
	 */
	int count() const;

	/**
	 * Retrieves the index of the current tab.
	 * @return
	 *   The current tab's index.
	 */
	int currentIndex() const;

	/**
	 * Inserts a tab into the tab bar.
	 * @param[in] index
	 *   The index where the new tab will be located.
	 * @param[in] text
	 *   The text for the new tab.
	 * @return
	 *   The index of the new tab.
	 */
	int insertTab(int index, const QString& text);

	/**
	 * Indicates whether or not a tab is enabled.
	 * @param[in] index
	 *   The index of the tab to test.
	 * @return
	 *   true if the tab is enabled, false otherwise.
	 */
	bool isTabEnabled(int index) const;

	/**
	 * Moves a tab from one index to another.
	 * @param[in] from
	 *   The index where the tab is currently located.
	 * @param[in] to
	 *   The new index for the tab.
	 */
	void moveTab(int from, int to);

	/**
	 * Removes a tab from the tab bar.
	 * @param[in] index
	 *   The index of the tab to remove.
	 */
	void removeTab(int index);

	/**
	 * Sets the data for a tab.
	 * @param[in] index
	 *   The index of the tab for which to set the data.
	 * @param[in] data
	 *   The data to associate to the tab.
	 */
	void setTabData(int index, const QVariant& data);

	/**
	 * Sets whether a tab is enabled.
	 * @param[in] index
	 *   The index of the tab.
	 * @param[in] state
	 *   true if the tab is enabled, false otherwise.
	 */
	void setTabEnabled(int index, bool state);

	/**
	 * Sets the text for a tab.
	 * @param[in] index
	 *   The index of the tab.
	 * @param[in] text
	 *   The new text for the tab.
	 */
	void setTabText(int index, const QString& text);

	/**
	 * Sets the color for text on the tab.
	 * @param[in] index
	 *   The index of the tab.
	 * @param[in] color
	 *   The new color to use for text on the tab.
	 */
	void setTabTextColor(int index, const QColor& color);

	/**
	 * Sets the tool tip for the tab.
	 * @param[in] index
	 *   The index of the tab.
	 * @param[in] toolTip
	 *   The new tool tip for the tab.
	 */
	void setTabToolTip(int index, const QString& toolTip);

	/**
	 * Sets whether or not tabs are closable.
	 * @param[in] state
	 *   true if tabs will now be closable, false otherwise.
	 */
	void setTabsClosable(bool state);

	/**
	 * Retrieves the index of the tab at the given position.
	 * @param[in] pos
	 *   The position to test.
	 * @return
	 *   The index of the tab at the given position.
	 */
	int tabAt(const QPoint& pos) const;

	/**
	 * Retrieves the data for a tab.
	 * @param[in] index
	 *   The index of the tab.
	 * @return
	 *   The data for the tab.
	 */
	QVariant tabData(int index) const;

	/**
	 * Retrieves the rect for a tab.
	 * @param[in] index
	 *   The index of the tab.
	 * @return
	 *   The rect for the tab.
	 */
	QRect tabRect(int index) const;

	/**
	 * Retrieves the text for a tab.
	 * @param[in] index
	 *   The index of the tab.
	 * @return
	 *   The text for the tab.
	 */
	QString tabText(int index) const;

	/**
	 * Retrieves the color of a tab's text.
	 * @param[in] index
	 *   The index of the tab.
	 * @return
	 *   The color of text on the tab.
	 */
	QColor tabTextColor(int index) const;

	/**
	 * Retrieves the tool tip for a tab.
	 * @param[in] index
	 *   The index of the tab.
	 * @return
	 *   The tool tip for the tab.
	 */
	QString tabToolTip(int index) const;

	/**
	 * Indicates whether or not tabs are closable.
	 * @return
	 *   true if tabs are closable, false otherwise.
	 */
	bool tabsClosable() const;

public slots:

	/**
	 * Sets the current tab by index.
	 * @param[in] index
	 *   The index of the new current tab.
	 */
	void setCurrentIndex(int index);

signals:

	/**
	 * Signal sent when the current tab changes.
	 * @param[in] index
	 *   The index of the new current tab.
	 */
	void currentChanged(int index);

	/**
	 * Signal sent when close is requested on a tab.
	 * @param[in] index
	 *   The index of the tab that has been requested to close.
	 */
	void tabCloseRequested(int index);

	/**
	 * Signal sent when a tab is moved.
	 * @param[in] from
	 *   The original index of the tab.
	 * @param[in] to
	 *   The new index of the tab.
	 */
	void tabMoved(int from, int to);

protected:

	/**
	 * Overrides QWidget#resizeEvent
	 */
	void resizeEvent(QResizeEvent* event) override;

private slots:

	/**
	 * Slot called when a close button is clicked.
	 */
	void OnTabClose();

private:

	/**
	 * Updates the minimum size to account for tab length.
	 */
	void UpdateMinimumSize();

	/**
	 * The list widget providing the contents of the tabs.
	 */
	QListWidget* _List = nullptr;

	/**
	 * Indicates whether tabs are closable.
	 */
	bool _TabsClosable = false;

};