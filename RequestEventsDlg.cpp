#include "RequestEventsDlg.h"
#include "../ImageColorBoolModel/ImageColorBoolProxy.h"
#include "../ImageColorBoolModel/ImageColorBoolDelegate.h"
#include "Connection.h"
#include <QSqlQueryModel>

RequestEventsDlg::RequestEventsDlg(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
#if defined(Q_WS_SIMULATOR) || defined(Q_OS_SYMBIAN)
	showMaximized();
#endif
	
	initModels();

	// User list
	ImageColorBoolProxy* usersProxy = new ImageColorBoolProxy(this);
	usersProxy->setColumnType(NAME,     ImageColorBoolProxy::NameColumn);
	usersProxy->setColumnType(IMAGE,    ImageColorBoolProxy::ImageColumn);
	usersProxy->setColumnType(SELECTED, ImageColorBoolProxy::BoolColumn);
	usersProxy->setImageColumn(IMAGE);
	usersProxy->setSourceModel(&usersModel);

	ui.tvUsers->setModel(usersProxy);
	ImageColorBoolDelegate* usersDelegate = new ImageColorBoolDelegate(usersProxy, ui.tvUsers);
	usersDelegate->setEditTrigger(QEvent::MouseButtonPress);
	usersDelegate->setCheckedImage  (QPixmap(":/Images/Checked.png"));
	usersDelegate->setUncheckedImage(QPixmap(":/Images/Unchecked.png"));
	ui.tvUsers->setItemDelegate(usersDelegate);
	ui.tvUsers->hideColumn(IMAGE);
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

void RequestEventsDlg::initModels()
{
	// Receive will be displayed as Selected
	QSqlQueryModel peersModel;
	peersModel.setQuery("select Name, Image, Receive from Peers");

	usersModel.setColumnCount(3);
	usersModel.setHeaderData(NAME,     Qt::Horizontal, "Name");
	usersModel.setHeaderData(SELECTED, Qt::Horizontal, "Selected");
	usersModel.insertRows(0, peersModel.rowCount());
	for(int row=0; row<peersModel.rowCount(); ++row)
	{
		usersModel.setData(usersModel.index(row, NAME),   peersModel.data(peersModel.index(row, NAME)));
		usersModel.setData(usersModel.index(row, IMAGE),  peersModel.data(peersModel.index(row, IMAGE)));
		usersModel.setData(usersModel.index(row, SELECTED), true);
	}
}

QStringList RequestEventsDlg::getUserList() const
{
	QStringList result;
	for(int row=0; row<usersModel.rowCount(); ++row)
		if(usersModel.data(usersModel.index(row, SELECTED)).toBool())           // if selected
			result << usersModel.data(usersModel.index(row, NAME)).toString();  // add name
	return result;
}

QStringList RequestEventsDlg::getEventList() const
{
	QStringList result;
	result << "CONNECTED" << "DISCONNECTED" << "OPENPROJECT";   // default ones
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
	ui.labelFuzziness->setText(tr("Phase division fuzziness = %1%").arg(value * 10));
}
