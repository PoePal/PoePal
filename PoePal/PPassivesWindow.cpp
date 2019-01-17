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
#include "PPassivesWindow.h"
#include "PApplication.h"
#include "PMessageHandler.h"
#include <QRegularExpression>

namespace {

	/**
	 * Format to use for act nodes.
	 */
	QString _ActFormat("%1 %2 (%3/%4)");
}

PPassivesWindow::PPassivesWindow(QWidget *parent)
	: QMdiSubWindow(parent)
{
	ui.setupUi(this);
	setWidget(ui._ContentWidget);
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto handler = app->GetMessageHandler();
	Q_ASSERT(handler);
	connect(handler, &PMessageHandler::NewMessage, this, &PPassivesWindow::OnNewMessage);
	connect(ui._RefreshBtn, &QPushButton::clicked, this, &PPassivesWindow::Refresh);
}

PPassivesWindow::~PPassivesWindow()
{
}

void PPassivesWindow::ClearStats()
{
	ui._TotalEdit->setText("0");
	ui._PassAllocEdit->setText("0");
	ui._PassFromLvlEdit->setText("0");
	ui._PassFromQuestsEdit->setText("0");
	ui._AscTotalEdit->setText("0");
	ui._AscAllocEdit->setText("0");
	// Loop through the tree and zero out everything.
	auto root = ui._QuestsTree->invisibleRootItem();
	int numActs = root->childCount();
	for (int a = 0; a < numActs; ++a)
	{
		auto actItem = root->child(a);
		int numQuest = actItem->childCount();
		int actTotal = 0;
		for (int q = 0; q < numQuest; ++q)
		{
			auto questItem = actItem->child(q);
			questItem->setCheckState(0, Qt::Unchecked);
			auto questItemText = questItem->text(0);
			auto count = questItemText.right(1) == ")" ? 2 : 1;
			actTotal += count;
		}
		actItem->setText(0, 
			_ActFormat.arg(tr("Act")).arg(QString::number(a + 1)).arg("0").arg(QString::number(actTotal)));
		actItem->setCheckState(0, Qt::Unchecked);
		actItem->setExpanded(true);
	}
}

void PPassivesWindow::Refresh()
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto handler = app->GetMessageHandler();
	Q_ASSERT(handler);
	handler->SendAction(PMessageHandler::Passives);
	// Zero out the stats;
	ClearStats();
}

void PPassivesWindow::OnNewMessage(PMessage *message)
{
	static QString totalPattern("^(\\d+)\\s+%1+\\s+([\\w]+)\\s+%2\\s+\\((\\d+)\\s%3\\)$");
	static QRegularExpression totRegex(totalPattern.arg(tr("total")).arg(tr("Skill Points"))
		.arg(tr("allocated")));
	static QString sumPattern("^(\\d+)\\s+%1\\s+([\\w\\s\\:]+)$");
	static QRegularExpression sumRegex(sumPattern.arg("Passive Skill Points from"));
	static QString questPattern("^\\((\\d)\\s+%1\\s+([\\w\\s\\']+)\\)$");
	static QRegularExpression questRegex(questPattern.arg("from"));
	auto str = questRegex.pattern();
	if (message->GetSubtype() == PMessage::Chat) return;
	auto content = message->GetContents();
	auto totMatch = totRegex.match(content);
	QRegularExpressionMatch sumMatch, questMatch;
	if (totMatch.hasMatch())
	{
		if (totMatch.captured(2) == tr("Passive"))
		{
			ClearStats();
			ui._TotalEdit->setText(totMatch.captured(1));
			ui._PassAllocEdit->setText(totMatch.captured(3));
		}
		else
		{
			ui._AscTotalEdit->setText(totMatch.captured(1));
			ui._AscAllocEdit->setText(totMatch.captured(3));
		}
	}
	else if((sumMatch = sumRegex.match(content)).hasMatch())
	{
		if (sumMatch.captured(2) == tr("character level"))
		{
			ui._PassFromLvlEdit->setText(sumMatch.captured(1));
		}
		else ui._PassFromQuestsEdit->setText(sumMatch.captured(1));
	}
	else if ((questMatch = questRegex.match(content)).hasMatch())
	{
		auto questName = questMatch.captured(2);
		auto root = ui._QuestsTree->invisibleRootItem();
		int numActs = root->childCount();
		for (int a = 0; a < numActs; ++a)
		{
			auto actItem = root->child(a);
			int numQuest = actItem->childCount();
			int actCount = 0, actTotal = 0;
			for (int q = 0; q < numQuest; ++q)
			{
				auto questItem = actItem->child(q);
				if (questItem->text(0).startsWith(questName))
				{
					questItem->setCheckState(0, Qt::Checked);
				}
				auto questItemText = questItem->text(0);
				auto count = questItemText.right(1) == ")" ? 2 : 1;
				if (questItem->checkState(0) == Qt::Checked) actCount += count;
				actTotal += count;
			}
			actItem->setText(0,
				_ActFormat.arg(tr("Act")).arg(QString::number(a + 1)).arg(QString::number(actCount)).arg(
					QString::number(actTotal)));
			actItem->setExpanded(actCount != actTotal);
		}
	}
}
