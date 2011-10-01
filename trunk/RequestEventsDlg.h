#ifndef RequestEventsDlg_h__
#define RequestEventsDlg_h__

#include <QDialog>
#include "ui_RequestEventsDlg.h"

class QSqlTableModel;
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
	void accept();

private slots:
	void onTimeSpan(const QDateTime& start, const QDateTime& end);
	void onFussiness(int value);

private:
	void initModels();

private:
	Ui::RequestEventsDlgClass ui;
	QSqlTableModel* allUsersModel;

	enum {NAME, IMAGE, SELECTED};   // for users list
};


#endif // RequestEventsDlg_h__
