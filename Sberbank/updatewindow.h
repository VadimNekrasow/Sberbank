#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include <QtWidgets>
#include "db.h"

class UpdateWin : public QWidget
{
    Q_OBJECT
public:
    UpdateWin(int _id, DB *_db);
    ~UpdateWin();
private:
    DB *db;
    QComboBox *combobox;
    QLineEdit *editSum;
    QLabel *labelId;
    QPushButton *buttonApply;
    int id;

signals:
    void updateInfo();
private slots:
    void clickButtonApply();
};

#endif // UPDATEWINDOW_H
