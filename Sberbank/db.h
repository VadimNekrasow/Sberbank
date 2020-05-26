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
private:
    QSqlDatabase db;
public:
    bool dbIsOpen = false;
    bool addingNewAccount(QString _category, QString _passport, int _startSum);
    bool addingOperation(int _id, int _sum, QString _type, QString _date);
    bool updateDepositorsSum(int _id, int _sum);
};

#endif // DB_H
