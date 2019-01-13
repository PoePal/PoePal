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
#include "PApplicationUpdateWidget.h"
#include "PApplication.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QNetworkReply>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>

PApplicationUpdateWidget::PApplicationUpdateWidget(const PApplicationUpdate &update, QWidget *parent)
	: QWidget(parent),_Latest(update)
{
	ui.setupUi(this);
	ui._InstallBtn->setVisible(false);
	ui._DownloadLbl->setVisible(false);
	ui._Progress->setVisible(false);
	auto channel = new QWebChannel(this);
	ui._WebEngine->page()->setWebChannel(channel);
	channel->registerObject("content", this);
	_Text = QString("<center> %1 </center>").arg(tr("Loading..."));
	setWindowFlags(Qt::Window);
	// Request all of the releases so we can display the ones since this version.
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto mgr = app->GetNetworkManager();
	auto reply = mgr->get(QNetworkRequest(QUrl("https://api.github.com/repos/PoePal/PoePal/releases")));
	connect(reply, &QNetworkReply::finished, this, &PApplicationUpdateWidget::OnGetReleasesFinished);
	connect(ui._CancelBtn, &QPushButton::clicked, this, &QWidget::close);
	connect(ui._DownloadBtn, &QPushButton::clicked, this, &PApplicationUpdateWidget::StartDownload);
	connect(ui._InstallBtn, &QPushButton::clicked, this, &PApplicationUpdateWidget::Install);
}

PApplicationUpdateWidget::~PApplicationUpdateWidget()
{
}

QString PApplicationUpdateWidget::GetText() const
{
	return _Text;
}

void PApplicationUpdateWidget::StartDownload()
{
	ui._DownloadBtn->setText(tr("Downloading..."));
	ui._DownloadBtn->setDisabled(true);
	ui._DownloadLbl->show();
	ui._Progress->show();
	auto url = _Latest.GetDownloadUrl();
	ui._WebEngine->page()->download(url, _TempDir.path() + QDir::separator() + url.fileName());
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	connect(ui._WebEngine->page()->profile(), &QWebEngineProfile::downloadRequested, this, 
		&PApplicationUpdateWidget::OnDownloadStarted);
}

void PApplicationUpdateWidget::Install()
{
	auto batchPath = _TempDir.path() + QDir::separator() + "runinstall.bat";
	QFile batch(batchPath);
	if (!batch.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, tr("Error"),
			tr("Could not open bat file for writing: %1").arg(batch.errorString()));
		close();
		return;
	}
	auto path = _TempDir.path();
	QTextStream stream(&batch);
	// Wait until the poepal process is finished running.
	stream << "@ECHO OFF" << endl;
	stream << ":LOOP" << endl;
	stream << "tasklist | findstr /b \"PoePal.exe \" " << endl;
	stream << "IF ERRORLEVEL 1 (" << endl;
	stream << "  GOTO CONTINUE" << endl;
	stream << ") ELSE (" << endl;
	stream << "  Timeout /T 1 /nobreak" << endl;
	stream << "  GOTO :LOOP" << endl;
	stream << ")" << endl;
	stream << ":CONTINUE" << endl;
	// Execute temporary installation executable.
	stream << "start /WAIT \"\" \"" << ui._InstallBtn->property("file").toString() << "\"" << endl;
	// Make the batch delete the temp directory when finished.
	stream << "(goto) 2>nul & rmdir /s /q \"%~dp0\"" << endl;
	QProcess::startDetached(QString("cmd.exe"), { "/c", batchPath }, path);
	qApp->quit();
}

void PApplicationUpdateWidget::OnGetReleasesFinished()
{
	auto reply = qobject_cast<QNetworkReply *>(sender());
	auto data = reply->readAll();
	auto doc = QJsonDocument::fromJson(data);
	auto releases = doc.array();
	int count = releases.count();
	_Text = "";
	for (int r = 0; r < count; ++r)
	{
		auto release = PApplicationUpdate(releases.at(r).toObject());
		_Text += "\n# " + release.GetName() + "\n" + release.GetBody();
	}
	reply->deleteLater();

}

void PApplicationUpdateWidget::OnDownloadStarted(QWebEngineDownloadItem *item)
{
	item->accept();
	connect(item, &QWebEngineDownloadItem::downloadProgress, this, 
		&PApplicationUpdateWidget::OnDownloadProgress);
	connect(item, &QWebEngineDownloadItem::finished, this, &PApplicationUpdateWidget::OnDownloadFinished);
}

void PApplicationUpdateWidget::OnDownloadProgress(qint64 received, qint64 total)
{
	ui._Progress->setMaximum(total);
	ui._Progress->setMinimum(0);
	ui._Progress->setValue(received);
}

void PApplicationUpdateWidget::OnDownloadFinished()
{
	auto item = qobject_cast<QWebEngineDownloadItem *>(sender());
	ui._DownloadBtn->hide();
	ui._InstallBtn->show();
	ui._InstallBtn->setProperty("file", item->path());
}

