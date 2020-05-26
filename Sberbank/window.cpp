#include "window.h"

Window::Window()
{

   this->setCurrentIndex(0);
   this->setMinimumSize(360, 240);

   buttonNew = new QPushButton("Добавить новый счет");
   buttonNew->connect(buttonNew, &QPushButton::clicked, this, &Window::clickButtonNew);
   buttonDetailed = new QPushButton("Подробно");
   buttonDetailed->connect(buttonDetailed, &QPushButton::clicked, this, &Window::clickButtonDetailed);
   buttonDetailed->setDisabled(true);
   buttonUpdate = new QPushButton("Пополнить/снять");
   buttonUpdate->connect(buttonUpdate, &QPushButton::clicked, this, &Window::clickButtonUpdate);
   buttonUpdate->setDisabled(true);

   QList <QString> listHeaders = {"Номер счета", "Тип", "Паспортные данные", "Текущая сумма", "Дата последней операции"};

   treeWidgetDepositors = new QTreeWidget();
   treeWidgetDepositors->setColumnCount(5);
   treeWidgetDepositors->setHeaderLabels(listHeaders);
   treeWidgetDepositors->connect(treeWidgetDepositors, &QTreeWidget::itemClicked, this, &Window::selectItemTreeWidget);

   QVBoxLayout *vboxMainStacked1 = new QVBoxLayout();
   vboxMainStacked1->setMargin(5);

   QHBoxLayout *hbox1 = new QHBoxLayout();
   hbox1->setMargin(0);
   hbox1->setSpacing(0);
   hbox1->addWidget(buttonNew);
   hbox1->addWidget(buttonUpdate);
   hbox1->addWidget(buttonDetailed);

   vboxMainStacked1->addLayout(hbox1);
   vboxMainStacked1->addWidget(treeWidgetDepositors);

   comboboxType = new QComboBox();
   comboboxType->addItem("Без процентный");
   comboboxType->addItem("Депозит");

   editPassport = new QLineEdit();
   editPassport->setInputMask("99 99 999999;_");
   startSum = new QSpinBox();
   startSum->setMinimum(1);
   startSum->setMaximum(1000000);

   buttonOk = new QPushButton("Сохранить");
   buttonOk->connect(buttonOk, SIGNAL(clicked()), this, SLOT(saveButtonClick()));
   buttonCancel = new QPushButton("Отмена");
   buttonCancel->connect(buttonCancel, SIGNAL(clicked()), this, SLOT(clickButtonCancel()));

   QWidget *widget = new QWidget();
   QHBoxLayout *hbox = new QHBoxLayout(widget);
   hbox->setSpacing(0);
   hbox->addStretch(1);
   hbox->addWidget(buttonOk);
   hbox->addWidget(buttonCancel);

   QFormLayout *mainForm = new QFormLayout();
   mainForm->addRow("Тип вклада: ", comboboxType);
   mainForm->addRow("Паспортные данные:", editPassport);
   mainForm->addRow("Начальная сумма: ", startSum);
   mainForm->addWidget(widget);

   treewidgetDetail = new QTreeWidget();
   treewidgetDetail->setColumnCount(4);
   QList <QString> listHeaders2 = {"Номер счета", "Сумма", "Тип операции", "Дата операции"};
   treewidgetDetail->setHeaderLabels(listHeaders2);

   QPushButton *buttonBack = new QPushButton("Назад");
   buttonBack->connect(buttonBack, &QPushButton::clicked, this, &Window::setCurrentIndex);

   QVBoxLayout *vbox = new QVBoxLayout();
   vbox->addWidget(buttonBack, Qt::AlignLeft);
   vbox->addWidget(treewidgetDetail);

   QWidget *widgetStacked1 = new QWidget();
   widgetStacked1->setLayout(vboxMainStacked1);
   QWidget *widgetStacked2 = new QWidget();
   widgetStacked2->setLayout(mainForm);
   QWidget *widgetStacked3 = new QWidget();
   widgetStacked3->setLayout(vbox);

   this->addWidget(widgetStacked1);
   this->addWidget(widgetStacked2);
   this->addWidget(widgetStacked3);

   selectData();
}

Window::~Window()
{
}

void Window::selectData()
{
    treeWidgetDepositors->clear();
    QSqlQuery query = QSqlQuery();
    query.exec("SELECT * FROM depositors");
    while (query.next()){
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidgetDepositors);
        item->setText(0, query.value(1).toString());
        item->setText(1, query.value(2).toString());
        item->setText(2, query.value(3).toString());
        item->setText(3, query.value(4).toString());
        item->setText(4, query.value(5).toString());
    }
}

void Window::clickButtonUpdate()
{
    int _id = currentItem;
    UpdateWin *window = new UpdateWin(_id, &database);
    window->connect(window, &UpdateWin::updateInfo, this, &Window::selectData);
    window->show();
}

void Window::selectItemTreeWidget()
{
    currentItem = treeWidgetDepositors->currentItem()->data(0, 0).toInt();
    buttonUpdate->setDisabled(false);
    buttonDetailed->setDisabled(false);
}

void Window::saveButtonClick()
{
    if (editPassport->text().length() != 12){
        editPassport->setFocus();
        return;
    }

    bool state = database.addingNewAccount(comboboxType->currentText(), editPassport->text(), startSum->value());
    if (state){
        setWindowTitle("Sberbank");
        editPassport->clear();
        startSum->clear();
        selectData();
        this->setCurrentIndex(0);
    }else{
        setWindowTitle("Сохранение не выполнено");
    }
}

void Window::clickButtonNew(){
    this->setCurrentIndex(1);
}

void Window::clickButtonCancel()
{
    editPassport->clear();
    startSum->clear();
    this->setCurrentIndex(0);
}

void Window::clickButtonDetailed()
{
    if (currentItem != -1){
        this->setCurrentIndex(2);
        selectDetailedInfo();
    }
}

void Window::selectDetailedInfo()
{
    treewidgetDetail->clear();
    QSqlQuery query = QSqlQuery();
    query.prepare(QString("SELECT ID_DEPOSITOR, SUM, TYPE_OPERATION, DATE FROM operations WHERE ID_DEPOSITOR = %1 ;").arg(currentItem));
    query.exec();
    while (query.next()){
        QTreeWidgetItem *item = new QTreeWidgetItem(treewidgetDetail);
        item->setText(0, query.value(0).toString());
        item->setText(1, query.value(1).toString());
        item->setText(2, query.value(2).toString());
        item->setText(3, query.value(3).toString());
    }
}
