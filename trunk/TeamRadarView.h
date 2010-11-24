#ifndef MessageReceiver_h__
#define MessageReceiver_h__

#include <QWidget>
#include <coreplugin/inavigationwidgetfactory.h> 
#include "ui_TeamRadarView.h"

namespace TeamRadar {

class TeamRadarNavigationWidgetFactory : public Core::INavigationWidgetFactory 
{ 
public: 
	Core::NavigationView createWidget(); 
	QString displayName() const;
	QString id() const;
}; 

class TeamRadarView : public QWidget
{
	Q_OBJECT

public:
	TeamRadarView(QWidget* parent = 0);

private slots:
	void onNewMessage(const QString& message);

private:
	Ui::TeamRadarViewClass ui;
};

}
#endif // MessageReceiver_h__


