#ifndef RequestEventsDlg_h__
#define RequestEventsDlg_h__

#include <QDialog>
#include "ui_RequestEventsDlg.h"
#include <QStandardItemModel>

class RequestEventsDlg : public QDialog
{
public:
	RequestEventsDlg(QWidget* parent = 0);
	QStringList getUserList()  const;
	QStringList getEventList() const;
	QDateTime   getStartTime() const;
	QDateTime   getEndTime()   const;

private:
	void initModels();

private:
	Ui::RequestEventsDlgClass ui;
	QStandardItemModel usersModel;
	QStandardItemModel eventsModel;

	enum {NAME, IMAGE, ONLINE, SELECTED};
};


#endif // RequestEventsDlg_h__