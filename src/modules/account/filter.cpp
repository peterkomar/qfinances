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
#include "filter.h"
#include "project.h"

#include <QDateTime>

Filter::Filter()
    :m_type(Transaction::BOTH)
    ,m_period(THIS_YEAR)
    ,i_id_account(0)
    ,s_val1("")
    ,s_sign1("")
    ,s_val2("")
    ,s_sign2("")
    ,s_comment("")
    ,i_id_category(-1)//all categories
{
}

Filter::~Filter()
{

}

QString Filter::toString(const QString& prefix)
{
    QString alias(prefix.isEmpty()? "" : prefix + ".");
    QString condition = QString(alias+"account_id = %1").arg(i_id_account);

    switch (m_type) {
    case Transaction::INCOMES:
        condition += " AND "+alias+"type = 1";
        break;
    case Transaction::EXPENSES:
        condition += " AND "+alias+"type = 2";
    default:
        break;
    }

    QDate d;

    switch (m_period) {
    case THIS_YEAR:
        m_dateTo = QDateTime::currentDateTime();
        m_dateFrom.setDate(QDate(m_dateTo.date().year(), 01, 01));
        m_dateFrom.setTime(QTime(0,0,0));
        break;

    case THIS_MONTH:
        m_dateTo = QDateTime::currentDateTime();
        m_dateFrom.setDate(QDate(m_dateTo.date().year(), m_dateTo.date().month(), 01));
        m_dateFrom.setTime(QTime(0,0,0));
        break;

    case PREV_YEAR:
        m_dateFrom = QDateTime::currentDateTime();
        m_dateFrom = m_dateFrom.addYears(-1);
        m_dateFrom.setDate(QDate(m_dateFrom.date().year(), 1, 1));
        m_dateFrom.setTime(QTime(0,0,0));

        m_dateTo = m_dateFrom.addYears(1).addDays(-1);
        m_dateTo.setTime(QTime(23,59,59));
        break;

    case PREV_MONTH:
        m_dateFrom = QDateTime::currentDateTime();
        m_dateFrom = m_dateFrom.addMonths(-1);
        m_dateFrom.setDate(QDate(m_dateFrom.date().year(), m_dateFrom.date().month(), 01));
        m_dateFrom.setTime(QTime(0,0,0));

        m_dateTo = m_dateFrom.addMonths(1).addDays(-1);
        m_dateTo.setTime(QTime(23,59,59));        
        break;

    case CUSTOM:
    default:
        break;
    }

    if(m_dateFrom.isValid()) {
        condition += QString(" AND "+alias+"created_at >= '%1'").arg(m_dateFrom.toString(Qt::ISODate));
    }

    if(m_dateTo.isValid()) {
        condition += QString(" AND "+alias+"created_at <= '%1'").arg(m_dateTo.toString(Qt::ISODate));
    }

    if(i_id_category >= 0) {
        condition += QString(" AND "+alias+"category_id = %1").arg(i_id_category);
    }

    if(!s_val1.isEmpty() && !s_sign1.isEmpty() ) {
         condition += " AND "+alias+"value " + s_sign1 + " " + s_val1;
         if(!s_val2.isEmpty() && !s_sign2.isEmpty()) {
             condition += " AND "+alias+"value " + s_sign2 + " " + s_val2;
         }
     }

     if(!s_comment.isEmpty()) {
         condition += " AND "+alias+"description LIKE '%" + s_comment + "%'";
     }

    return condition;
}

void Filter::reset()
{    
    i_id_category = -1;
    s_comment     = "";
    s_sign1       = "";
    s_val1        = "";
    s_sign2       = "";
    s_val2        = "";
    m_dateFrom.setDate(QDate());
    m_dateTo.setDate(QDate());
}

QString Filter::typeString(int type)
{
    QString text;
    switch (type) {
    case Transaction::INCOMES: text = QObject::tr("Incomes"); break;
    case Transaction::EXPENSES: text = QObject::tr("Expenses"); break;
    }

    return text;
}
