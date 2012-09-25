#ifndef CONNECTIONTAB_H
#define CONNECTIONTAB_H

#include "ui_ConnectionOptionWidget.h"
#include "Setting.h"

namespace TeamRadar {

class Setting;
class ConnectionOptionWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConnectionOptionWidget(QWidget *parent = 0);
    void save();
    
private:
    Ui::ConnectionOptionWidget ui;
    Setting* setting;
};

}

#endif // CONNECTIONTAB_H
