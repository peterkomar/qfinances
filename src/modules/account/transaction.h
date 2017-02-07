/***************************************************************************
 *   Copyright (C) 2016 by Peter Komar                                     *
 *   udldevel@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QDateTime>
#include <QAbstractTableModel>
#include <QSqlRecord>

#include "model.h"

class Filter;
class Account;

class Transaction : public Model, public QAbstractTableModel
{
public:
    enum Type {BOTH=0, INCOMES, EXPENSES};

    Transaction(DataBase *m_db, Account *account, int id = 0, QObject *parent = Q_NULLPTR);
    ~Transaction();

    bool canFetchMore(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void fetchMore(const QModelIndex &parent);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    void read();
    void setFilter(Filter *filter);

    bool save();
    void revert();

    void setId(int id) { m_d.id = id; }
    int id() { return m_d.id; }

    void setDate(QDateTime date) { m_d.date = date; }
    QDateTime date() const { return m_d.date; }    

    int accountId() { return m_d.account_id; }
    Account* account() const { return m_account; }

    void setCategoryId(int id) { m_d.category_id = id; }
    int categoryId() { return m_d.category_id; }

    void setType(Type type);
    Type type() { return m_d.type; }

    bool isIncomes() { return m_d.type == Type::INCOMES; }

    void setValue(double value) { m_d.value = value; }
    double value() { return m_d.value; }
    QString displayValue();

    void setBalance(double value) { m_d.balance = value; }
    double balance() { return m_d.balance; }
    QString displayBalance();

    void setDescription(const QString& str) { m_d.description = str; }
    QString description() const { return m_d.description; }

    void fetch(QSqlQuery *q);
    const QString table() { return QString("transactions"); }
    Model* model() { return new Transaction(m_db, 0); }


private:
    void initView();

    Filter *m_filter;
    QStringList m_columns;
    QList<QStringList> m_records;
    Account *m_account;

    struct {
        int id;
        QDateTime date;
        int account_id;
        int category_id;
        Type type;
        double value;
        double balance;
        QString description;
    } m_d;
};

#endif // TRANSACTION_H
