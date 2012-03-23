#ifndef RecipientsDlg_h__
#define RecipientsDlg_h__

#include <QDialog>
#include <QStandardItemModel>

namespace TeamRadar {

class RecipientsDlg : public QDialog
{
	Q_OBJECT

public:
	RecipientsDlg(const QString& peerName, QWidget* parent = 0);
	QStringList getRecipients() const;

private slots:
	void onSelectAll(bool select);

private:
	QStandardItemModel model;
	enum {NAME, IMAGE, ONLINE, SELECTED};
};

} // namespace TeamRadar

#endif // RecipientsDlg_h__
