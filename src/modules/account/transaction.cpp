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
#include "transaction.h"
#include "database.h"
#include "project.h"
#include "filter.h"
#include "../currency/currency.h"
#include "../category/category.h"
#include "account.h"

#include <QMessageBox>
#include <QApplication>

Transaction::Transaction(DataBase *db, Account *account, int id, QObject *parent)
    :Model(db, id)
    ,QAbstractTableModel(parent)
    ,m_filter(0)
    ,m_account(account)
{   
    m_d.account_id  = account->id();
    m_d.balance     = 0.0;
    m_d.category_id = 0;
    m_d.date        = QDateTime::currentDateTime();
    m_d.description = "";
    m_d.id          = id? id : 0;
    m_d.type        = INCOMES;
    m_d.value       = 0.0;

    initView();
}

Transaction::~Transaction()
{
    m_filter = 0;
    m_account = 0;
}

void Transaction::setType(Type type)
{
    if (type == Type::BOTH) {
#ifdef MFN_DEBUG
        qWarning() << "Incorrect transaction type";
#endif
    } else {
        m_d.type = type;
    }
}

void Transaction::fetch(QSqlQuery *q)
{
    m_d.id          = q->value("id").toInt();
    m_d.date        = q->value("created_at").toDateTime();
    m_d.account_id  = q->value("account_id").toInt();
    m_d.category_id = q->value("category_id").toInt();
    m_d.type        = (Type)q->value("type").toInt();
    m_d.value       = q->value("value").toDouble();
    m_d.balance     = q->value("balance").toDouble();
    m_d.description = q->value("description").toString();
}

bool Transaction::save()
{
    QSqlQuery *q = m_db->query();

    m_d.id = m_db->nextId(table());
    q->prepare(""
       "INSERT INTO "+table()+
       " (id, created_at, account_id, category_id, type, value, balance, description, status)"
       " VALUES "
       " (:id, :created_at, :account_id, :category_id, :type, :value, :balance, :description, :status)"
     );

     q->bindValue(":id", m_d.id);
     q->bindValue(":created_at", m_d.date.toString(Qt::ISODate));
     q->bindValue(":account_id", m_d.account_id);
     q->bindValue(":category_id", m_d.category_id);
     q->bindValue(":type", m_d.type);
     q->bindValue(":value", m_d.value);
     q->bindValue(":balance", m_d.balance);
     q->bindValue(":description", m_d.description);
     q->bindValue(":status", 1);
     m_db->exec(q);
     return true;
}

void Transaction::revert()
{
    QSqlQuery *q = m_db->query();
    q->prepare(""
         "UPDATE "+table()+" SET "
         "status = 0 "
         "WHERE id = :id"
    );

    q->bindValue(":id", m_d.id);
    m_db->exec(q);
}

void Transaction::initView()
{
    m_columns << tr("#") << tr("Date") << tr("Category")
              <<tr("Value") << tr("Balance") << tr("Comment");
}

bool Transaction::canFetchMore(const QModelIndex &/*parent*/) const
{
    return false;
}

int Transaction::columnCount(const QModelIndex &/*parent*/) const
{
    return m_columns.count();
}

QVariant Transaction::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= m_records.size()) {
        return QVariant();
    }

    QStringList row = m_records.at(index.row());
    if (role == Qt::DisplayRole) {
        return row.at(index.column());
    } else if(index.column() == 3) {
        if (role == Qt::ForegroundRole) {
            QString value = row.at(index.column());
            if (value.startsWith("+")) {
                return QBrush(Qt::darkGreen);
            }
            return QBrush(Qt::darkRed);
        } else if (role == Qt::FontRole) {
            QFont font =  qApp->font();
            font.setBold(true);
            return font;
        }
    }
    return QVariant();
}

void Transaction::fetchMore(const QModelIndex &/*parent*/)
{    
}

QVariant Transaction::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole && section < m_columns.size()) {
        return m_columns.at(section);
    }
    return QVariant();
}

int Transaction::rowCount(const QModelIndex &/*parent*/) const
{
    int rows = 50;
    return m_records.size() > rows? m_records.size() : rows;
}

void Transaction::read()
{
    QSqlQuery *q = m_db->query();
    Category category(m_db);

    QString query = ""
            "SELECT tr.*, "
            "case when cat_parent.name NOT NULL then printf('%s > %s', cat_parent.name, cat.name) else cat.name end as category "
            "FROM "+table()+" AS tr "
            "LEFT JOIN "+category.table()+" AS cat "
            "ON cat.id = tr.category_id "
            "LEFT JOIN "+category.table()+" AS cat_parent "
            "ON cat.parent_id = cat_parent.id "
            "WHERE "+m_filter->toString("tr")+" AND tr.status = 1 "
            "ORDER BY tr.created_at DESC";
    q->prepare(query);
    try {
        m_db->exec(q);
    } catch (int ) {
        return;
    }

    Transaction tr(m_db, m_account);
    m_records.clear();
    while (q->next()) {
        tr.fetch(q);

        QVariant category = q->value("category");

        QStringList rec;
        rec << QString::number(tr.id())
            << tr.date().toString()
            << (category.isValid()? category.toString() : "")
            << tr.displayValue()
            << tr.displayBalance()
            << tr.description();
        m_records.append(rec);
    }
    delete q;
}

void Transaction::setFilter(Filter *filter)
{
    m_filter = filter;
    m_filter->setAccountId(m_d.account_id);
    read();
}

QString Transaction::displayValue()
{
    QString value = m_account->currency()->displayPrice(m_d.value);
    if (isIncomes()) {
        value.prepend("+ ");
    } else {
        value.prepend("- ");
    }
    return value;
}

QString Transaction::displayBalance()
{
    return m_account->currency()->displayPrice(m_d.balance);
}

