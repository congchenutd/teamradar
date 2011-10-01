#include "RequestEventsDlg.h"
#include "../ImageColorBoolModel/ImageColorBoolProxy.h"
#include "../ImageColorBoolModel/ImageColorBoolDelegate.h"
#include "Connection.h"
#include "PeerManager.h"
#include <QSqlTableModel>

RequestEventsDlg::RequestEventsDlg(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
#if defined(Q_WS_SIMULATOR) || defined(Q_OS_SYMBIAN)
	showMaximized();
#endif
	
	// User list
	allUsersModel = PeerManager::getInstance()->getAllPeersModel();

//	ImageColorBoolProxy* usersProxy = new ImageColorBoolProxy(this);
//	usersProxy->setColumnType(NAME,     ImageColorBoolProxy::NameColumn);
//	usersProxy->setColumnType(IMAGE,    ImageColorBoolProxy::ImageColumn);
//	usersProxy->setColumnType(SELECTED, ImageColorBoolProxy::BoolColumn);
//	usersProxy->setImageColumn(IMAGE);
//	usersProxy->setSourceModel(allUsersModel);

	ui.tvUsers->setModel(allUsersModel);
//	ImageColorBoolDelegate* usersDelegate = new ImageColorBoolDelegate(usersProxy, ui.tvUsers);
//	usersDelegate->setEditTrigger(QEvent::MouseButtonPress);
//	usersDelegate->setCheckedImage  (QPixmap(":/Images/Checked.png"));
//	usersDelegate->setUncheckedImage(QPixmap(":/Images/Unchecked.png"));
//	ui.tvUsers->setItemDelegate(usersDelegate);
//	ui.tvUsers->hideColumn(IMAGE);
	ui.tvUsers->resizeRowsToContents();
	ui.tvUsers->resizeColumnsToContents();
	ui.tvUsers->horizontalHeader()->setStretchLastSection(true);

	// fetch time span from the server
	ui.dtStart->setDisplayFormat(Setting::dateTimeFormat);
	ui.dtEnd  ->setDisplayFormat(Setting::dateTimeFormat);
	ui.dtStart->setDateTime(QDateTime::currentDateTime());
	ui.dtEnd  ->setDateTime(QDateTime::currentDateTime());
	connect(Receiver::getInstance(), SIGNAL(timespan(QDateTime, QDateTime)), this, SLOT(onTimeSpan(QDateTime, QDateTime)));
	Sender::getInstance()->sendTimeSpanRequest();

	connect(ui.sliderFuzziness, SIGNAL(valueChanged(int)), this, SLOT(onFussiness(int)));
}

QStringList RequestEventsDlg::getUserList() const
{
	QStringList result;
	for(int row=0; row<allUsersModel->rowCount(); ++row)
		if(allUsersModel->data(allUsersModel->index(row, SELECTED)).toBool())           // if selected
			result << allUsersModel->data(allUsersModel->index(row, NAME)).toString();  // add name
	return result;
}

QStringList RequestEventsDlg::getEventList() const
{
	QStringList result;
	result << "JOINED" << "DISCONNECTED";   // default ones
	if(ui.checkSave->isChecked())
		result << "SAVE";
	if(ui.checkMode->isChecked())
		result << "MODE";
	if(ui.checkSCM->isChecked())
		result << "SCM_COMMIT";
	if(ui.checkChat->isChecked())
		result << "CHAT";
	return result;
}

QDateTime RequestEventsDlg::getStartTime() const {
	return ui.dtStart->dateTime();
}

QDateTime RequestEventsDlg::getEndTime() const {
	return ui.dtEnd->dateTime();
}

void RequestEventsDlg::onTimeSpan(const QDateTime& start, const QDateTime& end)
{
	ui.dtStart->setDateTime(start);
	ui.dtEnd  ->setDateTime(end);
}

QStringList RequestEventsDlg::getPhases() const
{
	QStringList result;
	if(ui.checkProject->isChecked())
		result << "Project";
	if(ui.checkPrototyping->isChecked())
		result << "Prototyping";
	if(ui.checkCoding->isChecked())
		result << "Coding";
	if(ui.checkTesting->isChecked())
		result << "Teseting";
	if(ui.checkDeployment->isChecked())
		result << "Deployment";
	return result;
}

int RequestEventsDlg::getFuzziness() const {
	return ui.sliderFuzziness->value() * 10;   // percentage
}

void RequestEventsDlg::onFussiness(int value) {
	ui.labelFuzziness->setText(tr("Division fuzziness = %1%").arg(value * 10));
}
