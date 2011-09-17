#include "RecipientsDlg.h"
#include "../../../../ImageColorBoolModel/ImageColorBoolProxy.h"
#include "../../../../ImageColorBoolModel/ImageColorBoolDelegate.h"
#include "Setting.h"
#include <QTableView>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QSqlQueryModel>
#include <QCheckBox>

RecipientsDlg::RecipientsDlg(const QString& peerName, QWidget* parent) : QDialog(parent)
{
	// model
	QSqlQueryModel peersModel;
	peersModel.setQuery(tr("select Name, Image, Online, Receive from Peers where Name != \"%1\"")
									.arg(Setting::getInstance()->getUserName()));

	model.setColumnCount(4);
	model.setHeaderData(NAME,     Qt::Horizontal, "Name");
	model.setHeaderData(SELECTED, Qt::Horizontal, "Selected");
	model.insertRows(0, peersModel.rowCount());
	for(int row=0; row<peersModel.rowCount(); ++row)
	{
		QString name = peersModel.data(peersModel.index(row, NAME)).toString();
		model.setData(model.index(row, NAME),   name);
		model.setData(model.index(row, IMAGE),  peersModel.data(peersModel.index(row, IMAGE)));
		model.setData(model.index(row, ONLINE), peersModel.data(peersModel.index(row, ONLINE)));
		model.setData(model.index(row, SELECTED), name == peerName);
	}

	// view
	QTableView* tableView = new QTableView(this);
	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setSelectionMode(QAbstractItemView::NoSelection);

	ImageColorBoolProxy* proxy = new ImageColorBoolProxy(this);
	proxy->setColumnType(NAME,     ImageColorBoolProxy::NameColumn);
	proxy->setColumnType(IMAGE,    ImageColorBoolProxy::ImageColumn);
	proxy->setColumnType(ONLINE,   ImageColorBoolProxy::BoolColumn);
	proxy->setColumnType(SELECTED, ImageColorBoolProxy::BoolColumn);
	proxy->setImageColumn(IMAGE);
	proxy->setGrayImageBy(ONLINE);
	proxy->setSourceModel(&model);

	tableView->setModel(proxy);
	ImageColorBoolDelegate* delegate = new ImageColorBoolDelegate(proxy, tableView);
	delegate->setEditTrigger(QEvent::MouseButtonPress);
	tableView->setItemDelegate(delegate);
	tableView->hideColumn(IMAGE);
	tableView->hideColumn(ONLINE);
	tableView->resizeRowsToContents();
	tableView->resizeColumnsToContents();

	QDialogButtonBox* buttons = new QDialogButtonBox(this);
	buttons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	QCheckBox* checkBox = new QCheckBox("Select All", this);
	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(tableView, 0, 0, 1, 2);
	layout->addWidget(checkBox,  1, 0);
	layout->addWidget(buttons,   1, 1);
	setLayout(layout);

	setWindowTitle("Recipients");
	resize(300, 400);

	// signals
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
	connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(onSelectAll(bool)));
}

QStringList RecipientsDlg::getRecipients() const
{
	QStringList result;
	for(int row=0; row<model.rowCount(); ++row)
		result << model.data(model.index(row, NAME)).toString();
	return result;
}

void RecipientsDlg::onSelectAll(bool select) {
	for(int row=0; row<model.rowCount(); ++row)
		model.setData(model.index(row, SELECTED), select);
}
