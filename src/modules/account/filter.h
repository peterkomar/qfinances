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
#ifndef FILTER_H
#define FILTER_H

#include <QString>
#include <QDateTime>

#include "transaction.h"

class Filter
{
public:

    enum Period {THIS_YEAR=10, THIS_MONTH, PREV_YEAR, PREV_MONTH, CUSTOM};

    Filter();
    ~Filter();

    QString toString(const QString& prefix = "");

    Transaction::Type type() { return m_type; }
    Period period() { return m_period; }

    void setType(Transaction::Type type) { m_type = type; }
    void setPeriod(Period period) { m_period = period; }
    void setAccountId(int id) { i_id_account = id; }
    void setDateFrom(const QDateTime& date) { m_dateFrom = date; }
    void setDateTo(const QDateTime& date) { m_dateTo = date; }

    void setCategoryId(int id) { i_id_category = id; }

    void setVal1Condition(const QString& val, const QString& sign) { s_val1 = val; s_sign1 = sign; }
    void setVal2Condition(const QString& val, const QString& sign) { s_val2 = val; s_sign2 = sign; }

    void setComment(const QString& comment) { s_comment = comment; }

    void reset();

    QDateTime dateFrom() const { return m_dateFrom; }
    QDateTime dateTo() const { return m_dateTo; }
    QString val1() const { return s_val1; }
    QString sign1() const { return s_sign1; }
    QString val2() const { return s_val1; }
    QString sign2() const { return s_sign2; }
    QString comment() const { return s_comment; }
    int categoryId() { return i_id_category; }
    int accountId() { return i_id_account; }

    static QString typeString(int type);

private:
    Transaction::Type m_type;
    Period m_period;
    int i_id_account;

    QDateTime m_dateFrom;
    QDateTime m_dateTo;

    QString s_val1;
    QString s_sign1;
    QString s_val2;
    QString s_sign2;

    QString s_comment;

    int i_id_category;
};

#endif // FILTER_H
