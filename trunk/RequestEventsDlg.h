#ifndef RequestEventsDlg_h__
#define RequestEventsDlg_h__

#include <QDialog>
#include "ui_RequestEventsDlg.h"
#include <QStandardItemModel>

class RequestEventsDlg : public QDialog
{
	Q_OBJECT

public:
	RequestEventsDlg(QWidget* parent = 0);
	QStringList getUserList()  const;
	QStringList getEventList() const;
	QDateTime   getStartTime() const;
	QDateTime   getEndTime()   const;
	QStringList getPhases()    const;
	int         getFuzziness() const;

private slots:
	void onTimeSpan(const QDateTime& start, const QDateTime& end);

private:
	void initModels();

private:
	Ui::RequestEventsDlgClass ui;
	QStandardItemModel usersModel;

	enum {NAME, IMAGE, ONLINE, SELECTED};
};


#endif // RequestEventsDlg_h__