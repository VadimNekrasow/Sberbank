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
                                        \"CURRENT_SUM\"	NUMERIC NOT NULL,\
                                        \"DATE_LAST_OPERATION\"	TEXT NOT NULL);");

    query.exec("CREATE TABLE IF NOT EXISTS \"operations\" ( \
                                        \"ID\"	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,\
                                        \"ID_DEPOSITOR\"	INTEGER NOT NULL,\
                                        \"SUM\"	NUMERIC NOT NULL,\
                                        \"TYPE_OPERATION\"	TEXT NOT NULL,\
                                        \"DATE\"	TEXT NOT NULL);");
    autoUpdateProzent();

}

DB::~DB()
{
    db.close();
}

bool DB::autoUpdateProzent()
{
    //QString currentDate = QDate::currentDate().toString("dd.MM.yyyy") + "_" + QTime::currentTime().toString("HH:mm:ss");

    QSqlQuery query = QSqlQuery();
    //query.prepare("SELECT ACC_NUMBER, CURRENT_SUM, DATE_LAST_OPERATION FROM depositors WHERE CATEGORY = \"Депозит\"");
    query.prepare("SELECT ACC_NUMBER as id, CURRENT_SUM, (SELECT DATE FROM operations WHERE ID_DEPOSITOR = depositors.ACC_NUMBER AND (TYPE_OPERATION = \"Создание\" OR TYPE_OPERATION = \"Проценты\") ORDER BY ID DESC LIMIT 1) as DATE FROM depositors WHERE CATEGORY = \"Депозит\";");
    query.exec();

    while (query.next()){

        int id = query.value(0).toInt();
        double current_sum = query.value(1).toDouble();
        QString date = query.value(2).toString();

        qDebug() << id << current_sum << date << endl;

        int D0 = date.section('_', 0, 0).section('.', 0,0).toInt();
        int M0 = date.section('_', 0, 0).section('.', 1,1).toInt();
        int Y0 = date.section('_', 0, 0).section('.', 2,2).toInt();

        int H0 =      date.section('_', 1, 1).section(':', 0,0).toInt();
        int Minuts0 = date.section('_', 1, 1).section(':', 1,1).toInt();
        int S0 =      date.section('_', 1, 1).section(':', 2,2).toInt();

        QDateTime datetime0(QDate(Y0, M0, D0), QTime(H0, Minuts0, S0));
        QDateTime datetimeNow(QDate::currentDate(), QTime::currentTime());

        double sumProzent = 0;
        while (datetime0.addSecs(180) < datetimeNow){
            double prozent = current_sum / 100 * 0.1;
            sumProzent += prozent;
            current_sum += prozent;
            qDebug() << "\t" << datetime0.toString() << datetimeNow.toString() <<endl;
            datetime0 = datetime0.addSecs(180);
        }
        if (sumProzent){
           //updateDepositorsSum(id, sumProzent, 2);
            updateProzentSum(id, sumProzent, datetime0.toString("dd.MM.yyyy_HH:mm:ss"));
        }
    }
    return true;

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

    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy") + "_" + QTime::currentTime().toString("HH:mm:ss");

    addingOperation(acc_number, _startSum, "Создание", currentDate);

    query.prepare("INSERT INTO depositors(ACC_NUMBER, CATEGORY, PASSPORT_DATA, CURRENT_SUM, DATE_LAST_OPERATION) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(acc_number);
    query.addBindValue(_category);
    query.addBindValue(_passport);
    query.addBindValue(_startSum);
    query.addBindValue(currentDate);
    return query.exec();

}

bool DB::addingOperation(int _id, double _sum, QString _type, QString _date)
{
     QSqlQuery query = QSqlQuery();
     query.prepare("INSERT INTO operations(ID_DEPOSITOR, SUM, TYPE_OPERATION, DATE) VALUES (?, ?, ?, ?)");
     query.addBindValue(_id);
     query.addBindValue(round(_sum*100)/100);
     query.addBindValue(_type);
     query.addBindValue(_date);
     return query.exec();
}

bool DB::updateDepositorsSum(int _id, double _sum, int codeType)
{
    QString currentDate = QDate::currentDate().toString("dd.MM.yyyy") + "_" + QTime::currentTime().toString("HH:mm:ss");
    QSqlQuery query = QSqlQuery();
    query.prepare(QString("UPDATE depositors SET CURRENT_SUM = (SELECT CURRENT_SUM FROM depositors WHERE ACC_NUMBER = %1) + %2, DATE_LAST_OPERATION = \"%3\" WHERE ACC_NUMBER = %1;").arg(_id).arg(round(_sum*100)/100).arg(currentDate));

    //addingOperation(_id, _sum, _sum > 0?"Пополнение":"Снятие", currentDate);

    QString _type;
    if (codeType == 0){
        _type = "Снятие";
    }else if (codeType == 1){
        _type = "Пополнение";
    }else if (codeType == 2){
        _type = "Проценты";
    }else{
        _type = "ERROR";
    }

    addingOperation(_id, _sum, _type, currentDate);
    return query.exec();
}

bool DB::updateProzentSum(int _id, double _sum, QString currentDate)
{
    QSqlQuery query = QSqlQuery();
    query.prepare(QString("UPDATE depositors SET CURRENT_SUM = (SELECT CURRENT_SUM FROM depositors WHERE ACC_NUMBER = %1) + %2, DATE_LAST_OPERATION = \"%3\" WHERE ACC_NUMBER = %1;").arg(_id).arg(round(_sum*100)/100).arg(currentDate));
    addingOperation(_id, round(_sum*100)/100, "Проценты", currentDate);
    return query.exec();
}





