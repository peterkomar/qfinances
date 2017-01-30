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

#include "currency.h"
#include "database.h"

#include <QLocale>

Currency::Currency(DataBase *db, int id)
 : Model(db, id)
{
    init();
    m_d.i_id = id;
}

Currency::~Currency()
{
}

void Currency::init()
{
    m_d.i_id = 0;
    m_d.d_value = 1.0;
    m_d.s_code = "";
    m_d.s_name = "";
    m_d.s_symbol_left = "";
    m_d.s_symbol_right = "";
    m_d.is_removed = false;
}

QString Currency::symbol() const
{
    return !m_d.s_symbol_left.isEmpty()? m_d.s_symbol_left : m_d.s_symbol_right;
}

bool Currency::save()
{
    QSqlQuery *q = m_db->query();

    if( m_d.i_id == 0 ) {

        /**
        * Check if new currency has code of currency which was created before
        */
        condition()->where = QString("code = '%1'").arg(m_d.s_code);
        condition()->order = "id ASC";
        condition()->limit = 1;

        Models items = this->items();
        if(!items.isEmpty()) {
            Currency * c = dynamic_cast<Currency*>(items.takeFirst());
            if (c->isRemoved()) {
                c->setIsRemoved(false);
                c->save();
            } else {
                m_error_message = QObject::tr("Currency with code %1 already exists").arg(m_d.s_code);
            }
            qDeleteAll(items);
            delete c;
            if (!m_error_message.isEmpty()) {
                return false;
            }
            return true;
        }

        m_d.i_id = m_db->nextId(table());
        q->prepare(""
         "INSERT INTO "+table()+" (id, name, code, symbol_left, symbol_right, value, is_removed)"
         " VALUES (:id, :name, :code, :symbol_left, :symbol_right, :value, :is_removed)"
        );
    } else {
        q->prepare(""
         "UPDATE "+table()+" SET"
         " id = :id, name = :name, code = :code, symbol_left = :symbol_left, symbol_right = :symbol_right, value = :value, is_removed = :is_removed"
         " WHERE id = :id"
        );
    }

    q->bindValue(":id", m_d.i_id);
    q->bindValue(":name", m_d.s_name);
    q->bindValue(":code", m_d.s_code);
    q->bindValue(":symbol_left", m_d.s_symbol_left);
    q->bindValue(":symbol_right", m_d.s_symbol_right);
    q->bindValue(":value", m_d.d_value);
    q->bindValue(":is_removed", (int)m_d.is_removed);
    m_db->exec(q);
    return true;
}

void Currency::remove()
{
#ifdef MFN_DEBUG
    Model::remove();
#else
    m_d.is_removed = true;
    save();
#endif
}

Currency* Currency::getDefaultCurrency(DataBase *db)
{
    QString code = QLocale::system().currencySymbol(QLocale::CurrencyIsoCode);

    Currency *c = new Currency(db);
    c->condition()->where = QString("is_removed = 0 AND code = '%1'").arg(code);
    c->condition()->order = "id";
    c->condition()->limit = 1;

    Models items = c->items(false);
    if(items.isEmpty()) {
        c->condition()->where = "is_removed = 0";
        items = c->items();
    }
    delete c;

    if(items.isEmpty()) {
        throw -1;
    }

    Currency *currency = dynamic_cast<Currency*>(items.takeFirst());
    qDeleteAll(items);
    return currency;
}

void Currency::fetch(QSqlQuery *q)
{
    m_d.i_id           = q->value("id").toInt();
    m_d.s_name         = q->value("name").toString();
    m_d.s_code         = q->value("code").toString();
    m_d.s_symbol_left  = q->value("symbol_left").toString();
    m_d.s_symbol_right = q->value("symbol_right").toString();
    m_d.d_value        = q->value("value").toDouble();
    m_d.is_removed     = q->value("is_removed").toBool();
}

QString Currency::displayPrice(double price)
{
    QString currencyString;

    currencyString.setNum(price, 'f', 2);
    if (m_d.s_symbol_left.isEmpty()) {
        return currencyString + " " + m_d.s_symbol_right;
    }
    return m_d.s_symbol_left + " " + currencyString;
}
