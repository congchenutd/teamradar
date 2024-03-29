#include "RequestEventsDlg.h"
#include "ImageColorBoolProxy.h"
#include "ImageColorBoolDelegate.h"
#include "Connection.h"
#include "PeerManager.h"
#include "Defines.h"
#include <QSqlTableModel>

namespace TeamRadar {

RequestEventsDlg::RequestEventsDlg(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
#ifdef OS_MOBILE
	showMaximized();
#endif

	// Copy from peerModel
	QSqlTableModel* peerModel = PeerManager::getInstance()->getPeerModel();
	peerModel->setFilter("1");    // reset filter to show everybody
	model.setColumnCount(3);
	model.setHeaderData(NAME,     Qt::Horizontal, tr("Name"));
	model.setHeaderData(IMAGE,    Qt::Horizontal, tr("Image"));
	model.setHeaderData(SELECTED, Qt::Horizontal, tr("Selected"));
	for(int row = 0; row < peerModel->rowCount(); ++row)
	{
		model.insertRow(row);
		model.setData(model.index(row, NAME),  peerModel->data(peerModel->index(row, PeerModel::NAME)));
		model.setData(model.index(row, IMAGE), peerModel->data(peerModel->index(row, PeerModel::IMAGE)));
		model.setData(model.index(row, SELECTED), false);
	}

	ImageColorBoolProxy* proxy = new ImageColorBoolProxy(this);
	proxy->setColumnType(NAME,     ImageColorBoolProxy::NameColumn);
	proxy->setColumnType(IMAGE,    ImageColorBoolProxy::ImageColumn);
	proxy->setColumnType(SELECTED, ImageColorBoolProxy::BoolColumn);
	proxy->setImageColumn(IMAGE);
	proxy->setSourceModel(&model);

	ui.tvUsers->setModel(proxy);
	ImageColorBoolDelegate* usersDelegate = new ImageColorBoolDelegate(proxy, ui.tvUsers);
	usersDelegate->setEditTrigger(QEvent::MouseButtonPress);
	usersDelegate->setCheckedImage  (QPixmap(":/Images/Checked.png"));
	usersDelegate->setUncheckedImage(QPixmap(":/Images/Unchecked.png"));
	ui.tvUsers->setItemDelegate(usersDelegate);
	ui.tvUsers->hideColumn(IMAGE);
	ui.tvUsers->resizeRowsToContents();
	ui.tvUsers->resizeColumnsToContents();
	ui.tvUsers->horizontalHeader()->setStretchLastSection(true);

	// init fussiness
	onFussiness(Setting::getInstance()->value("PartitionFuzziness").toInt());

	// fetch time span from the server
	ui.dtStart->setDisplayFormat(Setting::dateTimeFormat);
	ui.dtEnd  ->setDisplayFormat(Setting::dateTimeFormat);
	ui.dtStart->setDateTime(QDateTime::currentDateTime());
	ui.dtEnd  ->setDateTime(QDateTime::currentDateTime());
	connect(Receiver::getInstance(), SIGNAL(timespanReply(QDateTime, QDateTime)), this, SLOT(onTimeSpan(QDateTime, QDateTime)));
	Sender::getInstance()->sendTimeSpanRequest();

	connect(ui.sliderFuzziness, SIGNAL(valueChanged(int)), this, SLOT(onFussiness(int)));
}

QStringList RequestEventsDlg::getUserList() const
{
	QStringList result;
	for(int row=0; row<model.rowCount(); ++row)
		if(model.data(model.index(row, SELECTED)).toBool())           // if selected
			result << model.data(model.index(row, NAME)).toString();  // add name
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

void RequestEventsDlg::onFussiness(int value)
{
	ui.sliderFuzziness->setValue(value);
	ui.labelFuzziness->setText(tr("Partition fuzziness = %1%").arg(value * 10));
}

void RequestEventsDlg::accept()
{
	Setting::getInstance()->setValue("PartitionFuzziness", ui.sliderFuzziness->value());
	QDialog::accept();
}

} // namespace TeamRadar
