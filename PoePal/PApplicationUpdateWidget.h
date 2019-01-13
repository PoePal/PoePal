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

#include <QTemporaryDir>
#include <QWidget>
#include "ui_PApplicationUpdateWidget.h"
#include "PApplicationUpdate.h"

/**
 * Provides the user interface for performing an update.
 */
class PApplicationUpdateWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QString text READ GetText NOTIFY TextChanged)

public:
	/**
	 * Creates a new instance of this widget.
	 * @param[in] update
	 *   The newest update.
	 */
	PApplicationUpdateWidget(const PApplicationUpdate &update, QWidget *parent = Q_NULLPTR);

	/**
	 * Destructor.
	 */
	virtual ~PApplicationUpdateWidget();

	/**
	 * Retrieves the current markdown text displayed in the widget.
	 * @return
	 *   The current markdown text.
	 */
	QString GetText() const;

public slots:

	/**
	 * Starts the download.
	 */
	void StartDownload();

	/**
	 * Installs the update.
	 */
	void Install();

signals:

	/**
	 * Signal sent when the markdown text changes.
	 * @param[in] text
	 *   The new markdown text.
	 */
	void TextChanged(const QString &text);

private slots:

	/**
	 * Slot called when the request to fetch all releases finishes.
	 */
	void OnGetReleasesFinished();

	/**
	 * Slot called when the download has started.
	 * @param[in] item
	 *   The downloaded item.
	 */
	void OnDownloadStarted(QWebEngineDownloadItem *item);

	/**
	 * Slot called when download progress changes.
	 * @param[in] received
	 *   The amount of bytes received.
	 * @param[in] total
	 *   The total amount of bytes.
	 */
	void OnDownloadProgress(qint64 received, qint64 total);

	/**
	 * Slot called when the download is finished.
	 */
	void OnDownloadFinished();

private:
	Ui::PApplicationUpdateWidget ui;

	/**
	 * The newest update.
	 */
	PApplicationUpdate _Latest;

	/**
	 * The current markdown text.
	 */
	QString _Text;

	/**
	 * The temporary directory we're saving the executable to.
	 */
	QTemporaryDir _TempDir;
};
