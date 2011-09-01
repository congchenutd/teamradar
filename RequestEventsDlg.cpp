#include "RequestEventsDlg.h"
#include "ImageColorBoolModel.h"
#include "ImageColorBoolDelegate.h"
#include "Connection.h"
#include <QSqlQueryModel>

RequestEventsDlg::RequestEventsDlg(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	
	initModels();

	// User list
	ImageColorBoolProxy* usersProxy = new ImageColorBoolProxy(this);
	usersProxy->setColumnType(NAME,     ImageColorBoolProxy::NameColumn);
	usersProxy->setColumnType(IMAGE,    ImageColorBoolProxy::ImageColumn);
	usersProxy->setColumnType(ONLINE,   ImageColorBoolProxy::BoolColumn);
	usersProxy->setColumnType(SELECTED, ImageColorBoolProxy::BoolColumn);
	usersProxy->setImageColumn(IMAGE);
	usersProxy->setGrayImageBy(ONLINE);
	usersProxy->setSourceModel(&usersModel);

	ui.tvUsers->setModel(usersProxy);
	ImageColorBoolDelegate* uersDelegate = new ImageColorBoolDelegate(usersProxy, ui.tvUsers);
	uersDelegate->setEditTrigger(QEvent::MouseButtonPress);
	ui.tvUsers->setItemDelegate(uersDelegate);
	ui.tvUsers->hideColumn(IMAGE);
	ui.tvUsers->hideColumn(ONLINE);
	ui.tvUsers->resizeRowsToContents();
	ui.tvUsers->resizeColumnsToContents();

	// fetch time span from the server
	ui.dtStart->setDisplayFormat(Setting::dateTimeFormat);
	ui.dtEnd  ->setDisplayFormat(Setting::dateTimeFormat);
	ui.dtStart->setDateTime(QDateTime::currentDateTime());
	ui.dtEnd  ->setDateTime(QDateTime::currentDateTime());
	connect(Receiver::getInstance(), SIGNAL(timespan(QDateTime, QDateTime)), this, SLOT(onTimeSpan(QDateTime, QDateTime)));
	Sender::getInstance()->sendTimeSpanRequest();
}

void RequestEventsDlg::initModels()
{
	QSqlQueryModel peersModel;
	peersModel.setQuery("select Name, Image, Online, Receive from Peers");

	usersModel.setColumnCount(4);
	usersModel.setHeaderData(NAME,     Qt::Horizontal, "Name");
	usersModel.setHeaderData(SELECTED, Qt::Horizontal, "Selected");
	usersModel.insertRows(0, peersModel.rowCount());
	for(int row=0; row<peersModel.rowCount(); ++row)
	{
		usersModel.setData(usersModel.index(row, NAME),   peersModel.data(peersModel.index(row, NAME)));
		usersModel.setData(usersModel.index(row, IMAGE),  peersModel.data(peersModel.index(row, IMAGE)));
		usersModel.setData(usersModel.index(row, ONLINE), peersModel.data(peersModel.index(row, ONLINE)));
		usersModel.setData(usersModel.index(row, SELECTED), true);
	}
}

QStringList RequestEventsDlg::getUserList() const
{
	QStringList result;
	for(int row=0; row<usersModel.rowCount(); ++row)
	{
		bool selected = usersModel.data(usersModel.index(row, SELECTED)).toBool();
		if(selected)
			result << usersModel.data(usersModel.index(row, NAME)).toString();
	}
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