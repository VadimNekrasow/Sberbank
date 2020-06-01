#ifndef WINDOW_H
#define WINDOW_H

#include "db.h"
#include "updatewindow.h"

#include <QtWidgets>

class Window : public QStackedWidget
{
    Q_OBJECT

public:
    Window();
    ~Window();
private:
    DB database;

    QPushButton *buttonNew;
    QPushButton *buttonUpdate;
    QPushButton *buttonDetailed;
    QTreeWidget *treeWidgetDepositors;
    int currentItemID = -1;

    QComboBox *comboboxType;
    QLineEdit *editPassport;
    QSpinBox *startSum;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;

    QTreeWidget *treewidgetDetail;

private slots:
    void clickButtonUpdate();
    void selectItemTreeWidget();
    void saveButtonClick();
    void clickButtonNew();
    void clickButtonCancel();
    void clickButtonDetailed();
    void selectDetailedInfo();
    void selectData();

};
#endif // WINDOW_H
