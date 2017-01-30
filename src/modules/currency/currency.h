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

#ifndef CURRENCY_H
#define CURRENCY_H

#include "model.h"

class Currency : public Model
{
public:
    Currency(DataBase *m_db, int id = 0);
    ~Currency();

    bool save();
    void remove();
    void init();

    int id() { return m_d.i_id; }
    void setId(int id) { m_d.i_id = id; }

    QString name() const { return m_d.s_name; }
    void setName(const QString& name) { m_d.s_name = name; }

    QString code() const { return m_d.s_code; }
    void setCode(const QString& code) { m_d.s_code = code; }

    QString symbol() const;

    QString symbolLeft() const { return m_d.s_symbol_left; }
    void setSymbolLeft(const QString& symbol) { m_d.s_symbol_left = symbol; }

    QString symbolRight() const { return m_d.s_symbol_right; }
    void setSymbolRight(const QString& symbol) { m_d.s_symbol_right = symbol; }

    double rate() { return m_d.d_value; }
    void setRate(double rate) { m_d.d_value = rate; }

    bool isRemoved() { return m_d.is_removed; }
    void setIsRemoved(bool is_removed) { m_d.is_removed = is_removed; }

    QString displayPrice(double price);

    static Currency* getDefaultCurrency(DataBase *db);

protected:
    void fetch(QSqlQuery *q);
    const QString table() { return QString("currency"); }
    Model* model() { return new Currency(m_db); }

private:
    struct {
        int  i_id;
        QString s_name;
        QString s_code;
        QString s_symbol_left;
        QString s_symbol_right;
        double d_value;
        bool is_removed;
    } m_d;
};

#endif // CURRENCY_H
