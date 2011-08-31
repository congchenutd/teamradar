#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QDialog>
#include <QGraphicsWidget>
#include <QMap>
#include "ui_ChatWindow.h"

// A chat dialog
class ChatWindow : public QDialog
{
	Q_OBJECT

public:
	ChatWindow(const QString& name, QWidget* parent = 0);

	void addPeerConversation(const QString& line);
	void show();

	static ChatWindow* getChatWindow(const QString& name);
	static void saveAllHistory();
	static void closeAllWindows();

private:
	void saveHistory();
	void loadHistory();

private slots:
	void onInput();
	void onRecipients();

private:
	Ui::ChatWindow ui;
	QString peerName;
	bool peerReplied;    // used to combine lines from/to the same person
	bool meReplied;
	QStringList recipients;

	static QMap<QString, ChatWindow*> chatWindows;
};

#endif // CHATWINDOW_H
