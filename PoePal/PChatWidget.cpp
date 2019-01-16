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
#include "PChatWidget.h"
#include "PApplication.h"
#include "PChatOptionsDlg.h"
#include "PMessageHandler.h"
#include "PMainWindow.h"
#include <QKeyEvent>
#include <QMenu>
#include <QProxyStyle>
#include <QScrollBar>
#include <QSettings>
#include <QStringBuilder>
#include <QStyleOptionTab>
#include <QTabBar>
#include <QTextBlock>

/**
 * Class associates a text block with a message.
 */
class PTextBlockMessageData : public QTextBlockUserData
{
public:

	/**
	 * Creates a new data pointing to a given message.
	 * @param[in] message
	 *   The message to which the text block will point.
	 */
	PTextBlockMessageData(PMessage *message);

	/**
	 * Destructor.
	 */
	virtual ~PTextBlockMessageData();

	/**
	 * Retrieves the log message.
	 * @return
	 *   The log message.
	 */
	PMessage * GetLogMessage() const;

private:

	/**
	 * The log message being lined with a text block.
	 */
	QPointer<PMessage> _Message;
};

PTextBlockMessageData::PTextBlockMessageData(PMessage *message):
_Message(message)
{

}

PTextBlockMessageData::~PTextBlockMessageData()
{

}

PMessage * PTextBlockMessageData::GetLogMessage() const
{
	return _Message.data();
}

class PHorizontalTabStyle : public QProxyStyle
{

public:
	virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, 
		const QWidget *widget = nullptr) const override
	{
		if (element == CE_TabBarTabLabel)
		{
			if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(option))
			{
				QStyleOptionTab opt(*tab);
				opt.shape = QTabBar::RoundedNorth;
				QProxyStyle::drawControl(element, &opt, painter, widget);
				return;
			}
		}
		QProxyStyle::drawControl(element, option, painter, widget);
	}


	virtual QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, 
		const QWidget *widget) const override
	{
		QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
		if (type == QStyle::CT_TabBarTab) s.transpose();
		return s;
	}

};

PChatWidget::PChatWidget(QWidget *parent /*= Q_NULLPTR*/):
QDockWidget(parent)
{
	Initialize();
}

PChatWidget::PChatWidget(PMessage::Channel defaultChannel, QWidget *parent /*= Q_NULLPTR*/):
QDockWidget(parent)
{
	_DefaultChannel = defaultChannel;
	_Channels = defaultChannel;
	Initialize();
}

PChatWidget::~PChatWidget()
{
}

PMessage::Channel PChatWidget::GetDefaultChannel() const
{
	return _DefaultChannel;
}

PMessage::Channels PChatWidget::GetChannels() const
{
	return static_cast<PMessage::Channel>(_Channels.operator int());
}

void PChatWidget::SetChannels(const PMessage::Channels &channels)
{
	if (_DefaultChannel == PMessage::InvalidChannel)
	{
		_Channels = channels;
		UpdateForChannels();
	}
}

void PChatWidget::SetChannels(int channels)
{
	SetChannels(PMessage::Channels(static_cast<PMessage::Channel>(channels)));
}

QPlainTextEdit * PChatWidget::GetEntryWidget() const
{
	return ui._EntryEdit;
}

void PChatWidget::LoadState(const QSettings &settings)
{
	if (_DefaultChannel == PMessage::InvalidChannel)
	{
		_Channels = static_cast<PMessage::Channel>(settings.value(QStringLiteral("Channels")).toInt());
		setWindowTitle(settings.value(QStringLiteral("Title")).toString());
	}
	UpdateForChannels();
}

void PChatWidget::SaveState(QSettings &settings) const
{
	if (_DefaultChannel == PMessage::InvalidChannel)
	{
		settings.setValue(QStringLiteral("Title"), windowTitle());
		settings.setValue(QStringLiteral("Channels"), static_cast<int>(_Channels));
	}
}

void PChatWidget::Submit()
{
	auto text = ui._EntryEdit->toPlainText().trimmed();
	auto channel = GetCurrentChannel();
	ui._EntryEdit->clear();
	SetCurrentChannel(channel);
	// Strip off the channel prefix.
	if (GetCurrentChannel() != PMessage::Local)
	{
		text = text.mid(1).trimmed();
	}
	if (text.isEmpty()) return;
	// If the channel is whisper, get the sender from the first word.
	int chIdx = 0;
	QString target;
	if (GetCurrentChannel() == PMessage::Whisper)
	{
		while (chIdx < text.length() && !text[chIdx].isSpace()) ++chIdx;
		target = text.left(chIdx);
		text = text.mid(chIdx).trimmed();
		if (target.isEmpty()) return;
	}
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto handler = app->GetMessageHandler();
	Q_ASSERT(handler);
	handler->SendChatMessage(GetCurrentChannel(), text, target);
}

void PChatWidget::Configure()
{
	PChatOptionsDlg::SetOptionsModal(this);
}

void PChatWidget::Remove()
{
	if (_DefaultChannel != PMessage::InvalidChannel) return;
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	app->GetMainWindow()->RemoveCustomChatWidget(this);
}

void PChatWidget::SetWhisperTarget(const QString &target)
{
	ui._EntryEdit->setPlainText("@" + target + " ");
	auto cursor = ui._EntryEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	ui._EntryEdit->setTextCursor(cursor);
	ui._EntryEdit->setFocus(Qt::TabFocusReason);
}

bool PChatWidget::eventFilter(QObject *watched, QEvent *evt)
{
	if (watched == ui._EntryEdit)
	{
		if (evt->type() == QEvent::KeyPress)
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(evt);
			if (keyEvent->key() == Qt::Key_Return)
			{
				Submit();
				return true;
			}
		}
	}
	else if (watched == ui._DisplayEdit)
	{
		if (evt->type() == QEvent::ContextMenu)
		{
			QContextMenuEvent *contextEvent = static_cast<QContextMenuEvent *>(evt);
			_ContextMenu->popup(contextEvent->pos());
		}
	}
	return QDockWidget::eventFilter(watched, evt);
}

void PChatWidget::OnNewMessage(PMessage *message)
{
	if (!CheckMessage(message)) return;
	if (ui._DisplayEdit->isVisible())
	{
		auto wasAtEnd = ui._DisplayEdit->verticalScrollBar()->value() >= 
			ui._DisplayEdit->verticalScrollBar()->maximum()-2;
		auto doc = ui._DisplayEdit->document();
		QTextCursor cursor(doc);
		cursor.movePosition(QTextCursor::End);
		if(!doc->isEmpty()) cursor.insertBlock();
		cursor.block().setUserData(new PTextBlockMessageData(message));
		cursor.insertHtml(FormatMessage(message));
		if (ui._DisplayEdit->textCursor().selectedText().isEmpty() && wasAtEnd)
		{
			ui._DisplayEdit->setTextCursor(cursor);
			ui._DisplayEdit->ensureCursorVisible();
		}
	}
	else
	{
		int numTabs = ui._WhisperTabs->tabBar()->count();
		int idx = -1;
		for (int t = 0; t < numTabs; ++t)
		{
			if (ui._WhisperTabs->tabBar()->tabText(t) == message->GetSubject()) idx = t;
		}
		if (message->GetSubject().isEmpty())
		{
			if (numTabs > 0) idx = 0;
			else return;
		}
		if (idx >= 0)
		{
			auto isSel = ui._WhisperTabs->currentIndex() == idx;
			ui._WhisperTabs->tabBar()->moveTab(idx, 0);
			if(isSel) ui._WhisperTabs->setCurrentIndex(0);
			else if(message->IsIncoming()) ui._WhisperTabs->tabBar()->setTabTextColor(0, Qt::red);
		}
		else
		{
			auto textEdit = new QPlainTextEdit(ui._WhisperTabs);
			textEdit->setReadOnly(true);
			ui._WhisperTabs->insertTab(0, textEdit, message->GetSubject());
			if (numTabs > 0 && message->IsIncoming())
			{
				ui._WhisperTabs->tabBar()->setTabTextColor(0, Qt::red);
			}
			else ui._WhisperTabs->tabBar()->setCurrentIndex(0);
		}
		auto edit = qobject_cast<QPlainTextEdit *>(ui._WhisperTabs->widget(0));
		Q_ASSERT(edit);
		edit->appendHtml(FormatMessage(message));
	}
}

void PChatWidget::OnEntryChanged()
{
	auto doc = ui._EntryEdit->document();
	doc->setTextWidth(ui._EntryEdit->width()-2);
	auto height = doc->size().height();
	ui._EntryEdit->setMinimumHeight(height+2);
	UpdateChannelForPrefix();
}

void PChatWidget::OnChannelSelected()
{
	auto action = qobject_cast<QAction *>(sender());
	if (action) SetCurrentChannel(action->property("Channel").value<PMessage::Channel>());
}

void PChatWidget::OnTabSelected()
{
	int sel = ui._WhisperTabs->tabBar()->currentIndex();
	ui._WhisperTabs->tabBar()->setTabTextColor(sel, Qt::black);
	// Change the target of the whisper to the current tab.
	auto text = ui._EntryEdit->toPlainText().trimmed();
	// Get rid of the '@'
	text = text.mid(1).trimmed();
	// The target is the next characters up to the space, so strip it too.
	int ch = 0;
	while (ch < text.length() && !text.at(ch).isSpace()) ++ch;
	auto target = text.left(ch);
	text = text.mid(ch).trimmed();
	// If the target is not correct, set it.
	if (ui._WhisperTabs->tabText(sel) != target)
	{
		text.prepend(ui._WhisperTabs->tabText(sel) + ' ');
		text.prepend(PMessage::GetPrefixFromChannel(PMessage::Whisper));
		ui._EntryEdit->setPlainText(text);
		auto cursor = ui._EntryEdit->textCursor();
		cursor.movePosition(QTextCursor::End);
		ui._EntryEdit->setTextCursor(cursor);
	}
}

void PChatWidget::OnContextMenuRequested(const QPoint &pos)
{
	_ContextMenu->popup(ui._DisplayEdit->mapToGlobal(pos));
	auto cursor = ui._DisplayEdit->cursorForPosition(pos);
	auto userData = static_cast<PTextBlockMessageData *>(cursor.block().userData());
	if (userData) _ContextMenu->setProperty("msg", QVariant::fromValue(userData->GetLogMessage()));
	else
	{
		_ContextMenu->setDisabled(true);
		_ContextMenu->setProperty("msg", QVariant());
	}
}

void PChatWidget::OnContextMenuTriggered(QAction *action)
{
	auto message = _ContextMenu->property("msg").value<PMessage *>();
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto handler = app->GetMessageHandler();
	Q_ASSERT(handler);
	auto actionVar = action->property("action");
	if (!message) return;
	else if (!actionVar.isNull())
	{
		handler->SendAction(actionVar.value<PMessageHandler::Action>(), message->GetSubject());
	}
	else if (action == _WhisperAction)
	{
		auto mainWin = app->GetMainWindow();
		Q_ASSERT(mainWin);
		mainWin->Whisper(message->GetSubject());
	}
}

bool PChatWidget::CheckMessage(PMessage *message)
{
	return _Channels.testFlag(message->GetChannel()) || 
		(message->GetType() == PMessage::Info && message->GetSubtype() == PMessage::Event);
}

void PChatWidget::PrependMessages()
{
	auto app = qobject_cast<PApplication *>(qApp);
	Q_ASSERT(app);
	auto scanner = app->GetMessageHandler();
	QString contents;
	auto messages = scanner->GetLogMessages();
	int msgCount = messages.length();
	int curr = 0;
	for (int m = msgCount - 1; m >= 0 && curr < 100; --m, ++curr)
	{
		if (!CheckMessage(messages.at(m))) continue;
		contents.prepend(FormatMessage(messages.at(m)) + "<br />");
		_TopIdx = m;
	}
	contents.resize(contents.length() - 6);
	contents.append(ui._DisplayEdit->document()->toHtml());
	ui._DisplayEdit->clear();
	ui._DisplayEdit->appendHtml(contents);
}

QString PChatWidget::FormatMessage(PMessage *message) const
{
	QString color = "black";
	if (message->GetChannel() == PMessage::Global) color = "firebrick";
	else if (message->GetChannel() == PMessage::Trade) color = "goldenrod";
	else if (message->GetChannel() == PMessage::Guild) color = "gray";
	else if (message->GetChannel() == PMessage::Party) color = "cyan";
	else if (message->GetChannel() == PMessage::Local) color = "limegreen";
	else if (message->GetChannel() == PMessage::Whisper) color = "mediumpurple";
	QString text;
	if (message->GetSubtype() == PMessage::Chat)
	{
		text += "<span style=\"color: " + color + "; font-weight: bold;\">" + 
			message->GetTime().toString("[hh:mm] ");
		if (message->GetChannel() != PMessage::InvalidChannel)
		{
			text += PMessage::GetPrefixFromChannel(message->GetChannel());
		}
		if (message->GetChannel() == PMessage::Whisper)
		{
			text += message->IsIncoming() ? tr("From ") : tr("To ");
		}
		text += message->GetFullSender().toHtmlEscaped() + "</span> : ";
	}
	return text + message->GetContents().toHtmlEscaped();
}

void PChatWidget::Initialize()
{
	ui.setupUi(this);
	auto app = qobject_cast<PApplication *>(qApp);
	if (_DefaultChannel != PMessage::InvalidChannel)
	{
		setWindowTitle(PMessage::GetChannelLabel(_DefaultChannel));
	}
	Q_ASSERT(app);
	ui._WhisperTabs->tabBar()->setStyle(new PHorizontalTabStyle());
	connect(ui._EntryEdit, &QPlainTextEdit::textChanged, this, &PChatWidget::OnEntryChanged);
	connect(ui._DisplayEdit, &QWidget::customContextMenuRequested, this, &PChatWidget::OnContextMenuRequested);
	connect(ui._WhisperTabs->tabBar(), &QTabBar::currentChanged, this, &PChatWidget::OnTabSelected);
	ui._EntryEdit->installEventFilter(this);
	PrependMessages();
	auto scanner = app->GetMessageHandler();
	ui._DisplayEdit->verticalScrollBar()->setValue(ui._DisplayEdit->verticalScrollBar()->maximum());
	connect(scanner, &PMessageHandler::NewMessage, this, &PChatWidget::OnNewMessage);
	_ContextMenu = new QMenu(this);
	_FriendAction = _ContextMenu->addAction(tr("Add Friend"));
	_FriendAction->setProperty("action", QVariant::fromValue(PMessageHandler::Friend));
	_InviteAction = _ContextMenu->addAction(tr("Invite to Party"));
	_InviteAction->setProperty("action", QVariant::fromValue(PMessageHandler::Invite));
	_IgnoreAction = _ContextMenu->addAction(tr("Ignore"));
	_IgnoreAction->setProperty("action", QVariant::fromValue(PMessageHandler::Ignore));
	_WhoisAction = _ContextMenu->addAction(tr("Who Is This?"));
	_WhoisAction->setProperty("action", QVariant::fromValue(PMessageHandler::Whois));
	_WhisperAction = _ContextMenu->addAction(tr("Whisper"));
	_ContextMenu->addSeparator();
	_CopyAction = _ContextMenu->addAction(tr("Copy Text"));
	_CopyAction->setEnabled(!ui._DisplayEdit->textCursor().selectedText().isEmpty());
	connect(_ContextMenu, &QMenu::triggered, this, &PChatWidget::OnContextMenuTriggered);
	connect(ui._DisplayEdit, &QPlainTextEdit::copyAvailable, _CopyAction, &QAction::setEnabled);
	connect(_CopyAction, &QAction::triggered, ui._DisplayEdit, &QPlainTextEdit::copy);
	UpdateForChannels();
}

void PChatWidget::UpdateForChannels()
{
	if (!_ChannelMenu) 
	{
		_ChannelMenu = new QMenu(this);
		ui._ChannelBtn->setMenu(_ChannelMenu);
	}
	_ChannelMenu->clear();
	int channelCt = 0;
	for (const auto &channel : PMessage::GetChannels())
	{
		if (_Channels.testFlag(channel))
		{
			channelCt++;
			auto action = _ChannelMenu->addAction(PMessage::GetChannelLabel(channel));
			action->setProperty("Channel", channel);
			connect(action, &QAction::triggered, this, &PChatWidget::OnChannelSelected);
		}
	}
	bool oneChannel = channelCt == 1;
	SetChannelToFirst();
	if (_DefaultChannel != PMessage::InvalidChannel) ui._ChannelBtn->hide();
	else ui._ChannelBtn->show();
	bool tabbedWhisper = oneChannel && _Channels.testFlag(PMessage::Whisper);
	ui._WhisperTabs->setVisible(tabbedWhisper);
	ui._DisplayEdit->setVisible(!tabbedWhisper);
}

PMessage::Channel PChatWidget::GetCurrentChannel() const
{
	return ui._ChannelBtn->property("Channel").value<PMessage::Channel>();
}

void PChatWidget::SetCurrentChannel(PMessage::Channel channel)
{
	if (!_Channels.testFlag(channel))
	{
		SetChannelToFirst();
		return;
	}
	ui._ChannelBtn->setText(PMessage::GetChannelLabel(channel));
	ui._ChannelBtn->setProperty("Channel", channel);
	UpdatePrefixForChannel();
}

QChar PChatWidget::GetCurrentChannelPrefix() const
{
	return PMessage::GetPrefixFromChannel(GetCurrentChannel());
}

void PChatWidget::SetChannelToFirst()
{
	if (_ChannelMenu->actions().length() > 0)
	{
		SetCurrentChannel(_ChannelMenu->actions().first()->property("Channel").value<PMessage::Channel>());
	}
}

void PChatWidget::UpdatePrefixForChannel()
{
	auto channel = GetCurrentChannel();

	// Get the current text trimmed so that the first character indicates what channel it is currently 
	// destined for.
	auto text = ui._EntryEdit->toPlainText().trimmed();
	auto prefixChannel = text.isEmpty() ? PMessage::Local : PMessage::GetChannelFromPrefix(text[0]);
	if (prefixChannel == PMessage::InvalidChannel) prefixChannel = PMessage::Local;
	// If the prefix channel matches the current channel, don't do anything.
	if (prefixChannel == channel) return;

	// Otherwise, strip off the previous prefix and replace with the new appropriate prefix.
	if (prefixChannel != PMessage::Local) text = text.mid(1).trimmed();
	// There may be a second prefix.
	auto secondPrefix = text.isEmpty() ? 
		PMessage::InvalidChannel : PMessage::GetChannelFromPrefix(text[0]);
	if(secondPrefix != PMessage::InvalidChannel)text = text.mid(1).trimmed();
	if (channel != PMessage::Local) text.insert(0, PMessage::GetPrefixFromChannel(channel));
	if (channel == PMessage::Whisper && _DefaultChannel == PMessage::Whisper &&
		ui._WhisperTabs->count() > 0)
	{
		text += ui._WhisperTabs->tabBar()->tabText(ui._WhisperTabs->tabBar()->currentIndex()) + ' ';
	}
	ui._EntryEdit->setPlainText(text);
	auto cursor = ui._EntryEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	ui._EntryEdit->setTextCursor(cursor);
}

void PChatWidget::UpdateChannelForPrefix()
{
	auto text = ui._EntryEdit->document()->toPlainText().trimmed();
	if (text.isEmpty()) SetCurrentChannel(PMessage::Local);
	else
	{
		
		auto channel = PMessage::GetChannelFromPrefix(text[0]);
		text = text.mid(1).trimmed();
		auto secondChannel = text.isEmpty() ?
			PMessage::InvalidChannel : PMessage::GetChannelFromPrefix(text[0]);
		if (secondChannel != PMessage::InvalidChannel) SetCurrentChannel(secondChannel);
		else if (channel != PMessage::InvalidChannel) SetCurrentChannel(channel);
		else SetCurrentChannel(PMessage::Local);
	}
}
