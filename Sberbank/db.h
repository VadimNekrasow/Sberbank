#ifndef DB_H
#define DB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

#include <QDate>
#include <QTime>

class DB
{
public:
    DB();
    ~DB();
private:
    QSqlDatabase db;
    bool autoUpdateProzent();
public:
    bool dbIsOpen = false;
    bool addingNewAccount(QString _category, QString _passport, int _startSum);
    bool addingOperation(int _id, double _sum, QString _type, QString _date);
    bool updateDepositorsSum(int _id, double _sum, int codeType);
    bool updateProzentSum(int _id, double _sum, QString currentDate);
};

#endif // DB_H
