#include "db.h"

DB::DB()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");

    if (db.open()){
        dbIsOpen = true;
    }

    QSqlQuery query = QSqlQuery();
    query.exec("CREATE TABLE IF NOT EXISTS \"depositors\" (\
                                        \"ID\"	INTEGER PRIMARY KEY AUTOINCREMENT,\
                                        \"ACC_NUMBER\"	INTEGER NOT NULL UNIQUE,\
                                        \"CATEGORY\"	TEXT NOT NULL,\
                                        \"PASSPORT_DATA\" TEXT NOT NULL,\
                                        \"CURRENT_SUM\"	INTEGER NOT NULL,\
                                        \"DATE_LAST_OPERATION\"	TEXT NOT NULL);");

    query.exec("CREATE TABLE IF NOT EXISTS \"operations\" ( \
                                        \"ID\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
                                        \"ID_DEPOSITOR\"	INTEGER NOT NULL,\
                                        \"SUM\"	INTEGER NOT NULL,\
                                        \"TYPE_OPERATION\"	TEXT NOT NULL,\
                                        \"DATE\"	TEXT NOT NULL);");

}

bool DB::addingNewAccount(QString _category, QString _passport, int _startSum)
{

    QSqlQuery query = QSqlQuery();
    query.exec("SELECT MAX(ACC_NUMBER) FROM depositors;");
    int acc_number;
    if (query.next()){
        //acc_number = query.value(0).toInt();
        acc_number = query.value(0).isNull()?100000:query.value(0).toInt()+1;
    }else{
        acc_number = 100000;
    }

    QString currentDate = QDate::currentDate().toString("dd.MM.yy") + "_" + QTime::currentTime().toString("HH:mm:ss");

    addingOperation(acc_number, _startSum, "Создание", currentDate);

    query.prepare("INSERT INTO depositors(ACC_NUMBER, CATEGORY, PASSPORT_DATA, CURRENT_SUM, DATE_LAST_OPERATION) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(acc_number);
    query.addBindValue(_category);
    query.addBindValue(_passport);
    query.addBindValue(_startSum);
    query.addBindValue(currentDate);
    return query.exec();

}

bool DB::addingOperation(int _id, int _sum, QString _type, QString _date)
{
     QSqlQuery query = QSqlQuery();
     query.prepare("INSERT INTO operations(ID_DEPOSITOR, SUM, TYPE_OPERATION, DATE) VALUES (?, ?, ?, ?)");
     query.addBindValue(_id);
     query.addBindValue(_sum);
     query.addBindValue(_type);
     query.addBindValue(_date);
     return query.exec();
}

bool DB::updateDepositorsSum(int _id, int _sum)
{
    QString currentDate = QDate::currentDate().toString("dd.MM.yy") + "_" + QTime::currentTime().toString("HH:mm:ss");

    QSqlQuery query = QSqlQuery();
    query.prepare(QString("UPDATE depositors SET CURRENT_SUM = (SELECT CURRENT_SUM FROM depositors WHERE ACC_NUMBER = %1) + %2, DATE_LAST_OPERATION = \"%3\" WHERE ACC_NUMBER = %1;").arg(_id).arg(_sum).arg(currentDate));
    addingOperation(_id, _sum, _sum > 0?"Пополнение":"Снятие", currentDate);
    return query.exec();
}





