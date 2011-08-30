#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <QGraphicsWidget>
#include <QMap>
#include "ui_ChatWindow.h"

class ChatWindow : public QDialog
{
	Q_OBJECT

public:
	ChatWindow(const QString& name, QWidget *parent = 0);
	void addPeerConversation(const QString& line);
	void saveHistory(const QString& path);
	void loadHistory(const QString& path);

	static ChatWindow* getChatWindow(const QString& name);
	static void saveAllHistory(const QString& path);  // without the last '/'
	static void setHistoryPath(const QString& path) { historyPath = path; }

private slots:
	void onInput();
	void onRecipients();

signals:
	void messageSent(const QString& message);

private:
	Ui::ChatWindow ui;
	QString peerName;
	bool peerReplied;
	bool meReplied;
	QStringList recipients;

	static QMap<QString, ChatWindow*> chatWindows;
	static QString historyPath;
};

#endif // CHATWINDOW_H
