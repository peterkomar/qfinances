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

#include "account.h"
#include "transaction.h"
#include "database.h"
#include "../currency/currency.h"
#include "project.h"
#include "../category/category.h"
#include <QObject>
#include <QCryptographicHash>

Account::Account(DataBase *db, int id)
    :Model(db, id)
    ,m_currency(0)
{
    m_d.id        = id;
    m_d.balance   = 0.0;
    m_d.status    = STATUS_ACTIVE;
    m_d.type      = TYPE_ACCOUNT;
    m_d.password  = "";
    m_d.parent_id = 0;
    m_d.rate      = 0.0;
    m_d.rate_type = RATE_TYPE_MONTHLY;
}

Account::~Account()
{
    if (m_currency) {
        delete m_currency;
    }
}

void Account::setCurrencyId(int id) {
    if (m_d.currency_id != id && m_currency) {
        delete m_currency;
        m_currency = nullptr;
    }
    m_d.currency_id = id;
}

bool Account::addTransaction(Transaction *transaction)
{
   try {
        m_db->db()->transaction();        
        m_d.balance = addTransaction(transaction, m_d.balance);
        m_db->db()->commit();
    } catch(int code) {
        m_db->db()->rollback();
        m_error_message = QObject::tr("Can't add transaction");
        return false;
    }
    return true;
}

double Account::addTransaction(Transaction *transaction, double accountBalance)
{
    double balance = accountBalance;
    if (transaction->isIncomes()) {
        balance += transaction->value();
    } else {
        balance -= transaction->value();
    }

    QSqlQuery *q = m_db->query();
    q->prepare(""
        "UPDATE "+table()+" SET "
        "balance = :balance "
        "WHERE id = :id"
    );

    q->bindValue(":id", transaction->accountId());
    q->bindValue(":balance", balance);
    m_db->exec(q);

    transaction->setBalance(balance);
    transaction->save();
    return balance;
}

bool Account::revertTransaction(Transaction *transaction)
{
    double balance = m_d.balance;
    if (transaction->isIncomes()) {
        balance -= transaction->value();
    } else {
        balance += transaction->value();
    }

    try {
        m_db->db()->transaction();

        QSqlQuery *q = m_db->query();
        q->prepare(""
             "UPDATE "+table()+" SET "
             "balance = :balance "
             "WHERE id = :id"
        );

        q->bindValue(":id", m_d.id);
        q->bindValue(":balance", balance);
        m_db->exec(q);

        transaction->revert();
        m_d.balance = balance;
        m_db->db()->commit();

    } catch(int code) {
        m_db->db()->rollback();
        m_error_message = QObject::tr("Can't revert transaction");
        return false;
    }
    return true;
}

bool Account::addTransfer(Transaction *transaction, int accountId, double rate)
{
    Account* relatedAccount = new Account(m_db, accountId);
    if(!relatedAccount->load()) {
        delete relatedAccount;
        return false;
    }

    Transaction *related = new Transaction(m_db, relatedAccount);
    if (transaction->isIncomes()) {
        related->setType(Transaction::EXPENSES);
        related->setDescription(QObject::tr("Transfer to ") + m_d.name);
        transaction->setDescription(QObject::tr("Transfer from ") + relatedAccount->name());
    } else {
        related->setType(Transaction::INCOMES);
        related->setDescription(QObject::tr("Transfer from ") + m_d.name);
        transaction->setDescription(QObject::tr("Transfer to ") + relatedAccount->name());
    }

    int category_id = 24;//transfer
    double value = transaction->value();
    if (rate > 0) {
        category_id = 23;//exchenge
        value *= rate;
    }

    related->setValue(value);
    related->setDate(transaction->date());
    related->setCategoryId(category_id);

    transaction->setCategoryId(category_id);

    bool result = false;
    try {
         m_db->db()->transaction();
         double balance = addTransaction(transaction, m_d.balance);
         addTransaction(related, relatedAccount->balance());

         QSqlQuery *q = m_db->query();
         q->prepare(""
                    "INSERT INTO transfers"
                    " (source_transaction_id, destination_transaction_id, rate, created_at)"
                    " VALUES "
                    "(:source_transaction_id, :destination_transaction_id, :rate, :created_at)"
         );
         q->bindValue(":source_transaction_id", transaction->id());
         q->bindValue(":destination_transaction_id", related->id());
         q->bindValue(":rate", rate);
         q->bindValue(":created_at", transaction->date().toString(Qt::ISODate));
         m_db->exec(q);

         m_db->db()->commit();
         m_d.balance = balance;
         result = true;
    } catch(int code) {
        m_db->db()->rollback();
        m_error_message = QObject::tr("Can't do transfer");
    }

    delete related;
    delete relatedAccount;

    return result;
}

bool Account::create()
{
    m_d.id = m_db->nextId(table());
    m_d.created_at = QDateTime::currentDateTime();

    try {
        m_db->db()->transaction();
        QSqlQuery *q = m_db->query();
        q->prepare(""
                   "INSERT INTO "+table()+
                   " (id, name, currency_id, created_at, balance, description, password, status, type, parent_id, rate, rate_type)"
                   " VALUES "
                   "(:id, :name, :currency_id, :created_at, :balance, :description, :password, :status, :type, :parent_id, :rate, :rate_type)"
        );
        q->bindValue(":id", m_d.id);
        q->bindValue(":currency_id", m_d.currency_id);
        q->bindValue(":created_at", m_d.created_at.toString(Qt::ISODate));
        q->bindValue(":type", m_d.type);
        q->bindValue(":parent_id", m_d.parent_id);
        q->bindValue(":rate", m_d.rate);
        q->bindValue(":rate_type", m_d.rate_type);
        q->bindValue(":name", m_d.name);
        q->bindValue(":description", m_d.description);
        q->bindValue(":balance", m_d.balance);
        q->bindValue(":password", m_d.password);
        q->bindValue(":status", m_d.status);
        m_db->exec(q);

        Transaction* transaction = new Transaction(m_db, this);
        transaction->setType(Transaction::INCOMES);
        transaction->setBalance(m_d.balance);
        transaction->setValue(m_d.balance);
        transaction->setDescription(QObject::tr("Create account"));
        transaction->save();
        delete transaction;
        m_db->db()->commit();
    } catch(int code) {
        m_db->db()->rollback();
        m_error_message = QObject::tr("Can't create accunt");
        return false;
    }
    return true;
}

bool Account::save()
{
    if( m_d.id == 0 ) {
        return this->create();
    }

    QSqlQuery *q = m_db->query();
    q->prepare(""
         "UPDATE "+table()+" SET "
         "name = :name, "
         "description = :description, "
         "balance = :balance, "
         "password = :password, "
         "status = :status "
         "WHERE id = :id"
    );

    q->bindValue(":id", m_d.id);
    q->bindValue(":name", m_d.name);
    q->bindValue(":description", m_d.description);
    q->bindValue(":balance", m_d.balance);
    q->bindValue(":password", m_d.password);
    q->bindValue(":status", m_d.status);
    try {
        m_db->exec(q);
    } catch(int code) {
        m_error_message = QObject::tr("Can't save account details");
        return false;
    }
    return true;
}

bool Account::close()
{
    if( m_d.balance != 0.0  ) {
        return false;
    }

    m_d.status = STATUS_CLOSED;

    QSqlQuery *q = m_db->query();
    q->prepare("UPDATE "+table()+" SET status = :status WHERE id = :id");
    q->bindValue(":id", m_d.id);
    q->bindValue(":status", m_d.status);
    m_db->exec(q);
    return true;
}

void Account::remove()
{
    try {
        m_db->db()->transaction();
        QSqlQuery *q = m_db->query();
        q->prepare("DELETE FROM transactions WHERE account_id = :account_id");
        q->bindValue(":account_id", m_d.id);
        m_db->exec(q);

        q = m_db->query();
        q->prepare("DELETE FROM accounts WHERE id = :id");
        q->bindValue(":id", m_d.id);
        m_db->exec(q);
        m_db->db()->commit();
    } catch(int code) {
        m_db->db()->rollback();
        m_error_message = QObject::tr("Can't remove account");
    }
}

Currency* Account::currency()
{
    if (!m_currency) {
        m_currency = new Currency(m_db, m_d.currency_id);
        m_currency->load();
    }
    return m_currency;
}

QString Account::displayBalance()
{
     return currency()->displayPrice(m_d.balance);
}

QString Account::date(const QString& format) const
{
    return m_d.created_at.toString(format);
}

bool Account::auth(const QString &password)
{
    return encript(password) == m_d.password;
}

void Account::encriptPassword(const QString &password)
{
    m_d.password = encript(password);
}

QString Account::encript(const QString &string)
{
    QByteArray hash = QCryptographicHash::hash( string.toUtf8(), QCryptographicHash::Sha1);
    return QString(hash.toHex());
}

QString Account::statusText()
{
    QString text;
    switch (m_d.status) {
    case STATUS_ACTIVE:
        text  = QObject::tr("Active");
        break;
    case STATUS_CLOSED:
        text = QObject::tr("Closed");
        break;
    default:
        throw "Unknown status " + QString::number(m_d.status);
    }

    return text;
}

QString Account::typeText()
{
    QString text;
    switch (m_d.type) {
    case TYPE_ACCOUNT:
        text = QObject::tr("Account");
        break;
    case TYPE_CREDIT:
        text = QObject::tr("Credit");
        break;
    case TYPE_INVESTMENT:
        text = QObject::tr("Investment");
        break;
    default:
        throw "Unknown type " + QString::number(m_d.type);
    }

    return text;
}

QString Account::rateTypeText(TypeRate type)
{
    QString text;
    switch (type) {
    case RATE_TYPE_MONTHLY:
        text  = QObject::tr("Monthly");
        break;
    case RATE_TYPE_YEARLY:
        text = QObject::tr("Yearly");
        break;
    default:
        text = QObject::tr("Unknow rate type");
    }

    return text;
}

void Account::fetch(QSqlQuery *q)
{
    m_d.id            = q->value("id").toInt();
    m_d.name          = q->value("name").toString();
    m_d.description   = q->value("description").toString();
    m_d.balance       = q->value("balance").toDouble();
    m_d.created_at    = q->value("created_at").toDateTime();
    m_d.currency_id   = q->value("currency_id").toInt();
    m_d.password      = q->value("password").toString();
    m_d.status        = (Status)q->value("status").toInt();
    m_d.type          = (Type)q->value("type").toInt();
    m_d.parent_id     = q->value("parent_id").toInt();
    m_d.rate          = q->value("rate").toDouble();
    m_d.rate_type     = (TypeRate)q->value("rate_type").toInt();
}
