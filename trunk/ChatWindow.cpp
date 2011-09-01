#include "ChatWindow.h"
#include "Connection.h"
#include "RecipientsDlg.h"
#include "Setting.h"
#include <QDateTime>
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>
#include <QScrollBar>

ChatWindow::ChatWindow(const QString& name, QWidget *parent)
	: QDialog(parent), peerName(name), peerReplied(true), meReplied(true)
{
	ui.setupUi(this);
	ui.leInput->setFocus();
	ui.leInput->grabKeyboard();
	setWindowTitle("Chat with " + name);
	setWindowFlags(Qt::WindowStaysOnTopHint);   // use show() instead of exec()

	connect(ui.leInput,          SIGNAL(returnPressed()), this, SLOT(onInput()));
	connect(ui.btSend,           SIGNAL(clicked()),       this, SLOT(onInput()));
	connect(ui.btMoreRecipients, SIGNAL(clicked()),       this, SLOT(onRecipients()));

	chatWindows.insert(name, this);
	if(ui.teHistory->toPlainText().isEmpty())
		loadHistory();
}

// when the user hits enter or clicks the send button
void ChatWindow::onInput()
{
	if(peerReplied)    // my first message
		ui.teHistory->append("\r\nMe (" + 
			QDateTime::currentDateTime().toString(Setting::dateTimeFormat) + "):");
	ui.teHistory->append(ui.leInput->text());
	recipients << peerName;  // add the peer to the recipients
	Sender::getInstance()->sendChat(recipients, ui.leInput->text());  // send
	ui.leInput->clear();
	recipients.clear();
	peerReplied = false;
	meReplied   = true;
	accept();
}

// received a message
void ChatWindow::addPeerConversation(const QString& line)
{
	if(line.isEmpty())
		return;
	
	if(meReplied)   // peer's first message
		ui.teHistory->append("\r\n" + peerName + " (" + 
			QDateTime::currentDateTime().toString(Setting::dateTimeFormat) + "): ");
	ui.teHistory->append(line);
//	ui.teHistory->verticalScrollBar()->setValue(
//			ui.teHistory->verticalScrollBar()->maximum());   // scroll to the bottom
	peerReplied = true;
	meReplied   = false;
}

void ChatWindow::show() {
	if(!isVisible())   // reuse the window
		QDialog::show();
}

void ChatWindow::loadHistory()
{
	QString path = Setting::getInstance()->getChatHistoryPath();
	QFile file(path + "/" + peerName + ".txt");
	if(file.open(QFile::ReadOnly))
	{
		QTextStream is(&file);
		ui.teHistory->clear();
		ui.teHistory->append(is.readAll());
	}
}

// singletons
ChatWindow* ChatWindow::getChatWindow(const QString& name) {
	return chatWindows.contains(name) ? chatWindows[name] : new ChatWindow(name);
}

// save all windows' history
void ChatWindow::saveAllHistory() {
	foreach(ChatWindow* chat, chatWindows)
		chat->saveHistory();
}

void ChatWindow::saveHistory()
{
	QString path = Setting::getInstance()->getChatHistoryPath();
	QFile file(path + "/" + peerName + ".txt");
	if(file.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream os(&file);
		os << ui.teHistory->toPlainText();
	}
}

void ChatWindow::onRecipients()
{
	RecipientsDlg dlg(peerName, this);
	if(dlg.exec() == QDialog::Accepted)
		recipients = dlg.getRecipients();
}

void ChatWindow::closeAllWindows() {
	foreach(ChatWindow* window, chatWindows)
		window->accept();
}

QMap<QString, ChatWindow*> ChatWindow::chatWindows;