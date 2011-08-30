#include "ChatWindow.h"
#include "Connection.h"
#include "RecipientsDlg.h"
#include <QDateTime>
#include <QGraphicsScene>
#include <QFile>
#include <QTextStream>

ChatWindow::ChatWindow(const QString& name, QWidget *parent)
	: QDialog(parent), peerName(name), peerReplied(true), meReplied(true)
{
	ui.setupUi(this);
	ui.leInput->setFocus();
	ui.leInput->grabKeyboard();
	setWindowTitle("Chat with " + name);
	setWindowFlags(Qt::WindowStaysOnTopHint);

	connect(ui.leInput,          SIGNAL(returnPressed()), this, SLOT(onInput()));
	connect(ui.btMoreRecipients, SIGNAL(clicked()),       this, SLOT(onRecipients()));

	chatWindows.insert(name, this);
	if(ui.teHistory->toPlainText().isEmpty())
		loadHistory(historyPath);
}

void ChatWindow::onInput()
{
	if(peerReplied)
		ui.teHistory->append("\r\nMe (" + QDateTime::currentDateTime().toString() + "):");
	ui.teHistory->append(ui.leInput->text());
//	emit messageSent(ui.leInput->text());
	recipients << peerName;
	Sender::getInstance()->sendChat(recipients, ui.leInput->text());
	ui.leInput->clear();
	recipients.clear();
	peerReplied = false;
	meReplied   = true;
	accept();
}

void ChatWindow::addPeerConversation(const QString& line)
{
	if(meReplied)
		ui.teHistory->append("\r\n" + peerName + " (" + QDateTime::currentDateTime().toString() + "): ");
	ui.teHistory->append(line);
	peerReplied = true;
	meReplied   = false;
	if(!isVisible())
		show();
}

void ChatWindow::loadHistory(const QString& path)
{
	QFile file(path + "/" + peerName + ".txt");
	if(file.open(QFile::ReadOnly))
	{
		QTextStream is(&file);
		QString history;
		is >> history;
		ui.teHistory->clear();
		ui.teHistory->append(history);
	}
}

ChatWindow* ChatWindow::getChatWindow(const QString& name) {
	return chatWindows.contains(name) ? chatWindows[name] : new ChatWindow(name);
}

void ChatWindow::saveAllHistory(const QString& path)
{
	setHistoryPath(path);
	foreach(ChatWindow* chat, chatWindows)
		chat->saveHistory(path);
}

void ChatWindow::saveHistory(const QString& path)
{
	QFile file(path + "/" + peerName + ".txt");
	if(file.open(QFile::WriteOnly))
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

QString ChatWindow::historyPath = ".";

QMap<QString, ChatWindow*> ChatWindow::chatWindows;