#include "updatewindow.h"

UpdateWin::UpdateWin(int _id, DB *_db)
{

    this->setWindowModality(Qt::ApplicationModal);

    id = _id;
    db = _db;

    setAttribute(Qt::WA_DeleteOnClose);
    labelId = new QLabel(QString::number(_id));
    labelId->setFont(QFont(font().family(), 20));

    combobox = new QComboBox();
    combobox->addItem("Снятие");
    combobox->addItem("Пополнение");

    editSum = new QLineEdit();
    QIntValidator *validator = new QIntValidator();
    validator->setBottom(1);
    editSum->setValidator(validator);

    buttonApply = new QPushButton("Сохранить");
    buttonApply->connect(buttonApply, SIGNAL(clicked()), this, SLOT(clickButtonApply()));

    QFormLayout *mainForm = new QFormLayout(this);
    mainForm->addWidget(labelId);
    mainForm->addRow("Вид операции: ", combobox);
    mainForm->addRow("Сумма: ", editSum);
    mainForm->addWidget(buttonApply);
}

UpdateWin::~UpdateWin()
{
}

void UpdateWin::clickButtonApply()
{
    if (editSum->text().isEmpty()){
        editSum->setFocus();
        return;
    }

    int sum = editSum->text().toInt();
    sum = combobox->currentIndex() == 0?sum*=-1:sum;

    if (db->updateDepositorsSum(id, sum)){
        this->close();
        emit updateInfo();
    }

}
