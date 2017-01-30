/***************************************************************************
 *   Copyright (C) 2014 by Peter Komar                                     *
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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QDateTime>
#include <QStack>

#include "model.h"
#include "filter.h"
#include "transaction.h"

class Currency;

class Account : public Model
{
public:
    enum Status {STATUS_ACTIVE = 0, STATUS_CLOSED};
    enum Type {TYPE_ACCOUNT = 1, TYPE_CREDIT, TYPE_INVESTMENT};
    enum TypeRate {RATE_TYPE_MONTHLY = 1, RATE_TYPE_YEARLY};

    Account(DataBase *m_db, int id = 0);
    ~Account();

    bool save();
    void remove();
    bool close();
    bool create();

    bool addTransaction(Transaction *transaction);
    bool revertTransaction(Transaction *transaction);

    Currency* currency();
    QString displayBalance();
    QString date(const QString& format) const;
    bool auth(const QString& password);
    void encriptPassword(const QString& password);

    int id() { return m_d.id; }

    QString name() const { return m_d.name; }
    void setName(const QString& name) { m_d.name = name; }

    QString description() const { return m_d.description; }
    void setDescription(const QString& description) {m_d.description = description;}

    QString password() const { return m_d.password; }
    void setPassword(const QString& password) { m_d.password = password; }

    void setBalance(double balance) { m_d.balance = balance; }
    double balance() { return m_d.balance; }

    void setCurrencyId(int id);
    int currencyId() { return m_d.currency_id; }

    Status status() { return m_d.status; }
    QString statusText();

    void setType(Type type) { m_d.type = type; }
    Type type() {return m_d.type; }
    QString typeText();

    void setParentId(int parent_id) { m_d.parent_id = parent_id; }
    int parentId() { return m_d.parent_id; }

    void setRate(double rate) { m_d.rate = rate; }
    double rate() { return m_d.rate; }

    void setRateType(TypeRate type) { m_d.rate_type = type; }
    TypeRate  rateType() { return m_d.rate_type; }
    static QString rateTypeText(TypeRate type);

    void fetch(QSqlQuery *q);
    const QString table() { return QString("accounts"); }
    Model* model() { return new Account(m_db); }

private:
    QString encript(const QString& string);

    Currency *m_currency;

    struct {
        int id;
        QString name;
        int currency_id;
        QDateTime created_at;
        double balance;
        QString description;
        QString password;
        Status status;
        Type type;
        int parent_id;
        double rate;
        TypeRate rate_type;
    } m_d;
};

#endif // ACCOUNT_H
