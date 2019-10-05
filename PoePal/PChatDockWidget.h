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

#include "PMessage.h"
#include <QDockWidget>

class PChatWidget;
class QSettings;

/**
 * Class provides a dock widget for chat channels and whispers.
 */
class PChatDockWidget : public QDockWidget
{
	Q_OBJECT;

	/**
	 * The chat widget providing the contents of the dock widget.
	 */
	Q_PROPERTY(PChatWidget* chatWidget READ GetChatWidget);

public:

	/**
	 * Creates a new instance of the widget.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	PChatDockWidget(QWidget* parent = Q_NULLPTR);

	/**
	 * Creates a new instance of the widget for a default channel.
	 * @param[in] defaultChannel
	 *   The default channel.
	 * @param[in] parent
	 *   The parent of the widget.
	 */
	PChatDockWidget(PMessage::Channel defaultChannel, QWidget* parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PChatDockWidget() = default;

	/**
	 * Retrieves the chat widget that provides the content of this dock widget.
	 * @return
	 *   The chat widget.
	 */
	PChatWidget* GetChatWidget() const;

	/**
	 * Loads the state of the widget from settings.
	 * @param[in] settings
	 *   The settings from which to load the state.
	 */
	void LoadState(const QSettings& settings);

	/**
	 * Saves the state of the widget to settings.
	 * @param[in] settings
	 *   The settings to which to save the state.
	 */
	void SaveState(QSettings& settings) const;

public slots:

	/**
	 * Configures the chat widget.
	 */
	void Configure();

	/**
	 * Removes the chat widget.
	 * This is ignored if the chat widget is a default chat widget.
	 */
	void Remove();

private:

	/**
	 * Initializes the dock widget.
	 */
	void Initialize();

	/**
	 * The chat widget providing the contents of this dock widget.
	 */
	PChatWidget* _ChatWidget = nullptr;

};