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

#ifndef CATEGORY_H
#define CATEGORY_H

#include "model.h"

class Category : public Model
{
public:
    Category(DataBase *m_db, int id = 0);
    ~Category();

    bool save();
    void init();
    void remove();

    int id() { return m_d.i_id; }
    int parentId() { return m_d.i_parent_id; }
    void setParentId(int id) { m_d.i_parent_id = id; }

    bool hasChildren();

    QString name() const { return m_d.s_name; }
    void setName(const QString& name) { m_d.s_name = name; }

    QString description() const { return m_d.s_description; }
    void setDescription(const QString& description) { m_d.s_description = description; }

    int type() { return m_d.i_type; }
    void setType(int type) { m_d.i_type = type; }

    double tax() { return m_d.d_tax; }
    void setTax(double tax) { m_d.d_tax = tax; }

    double value() { return m_d.value; }
    void setValue(double value) { m_d.value = value; }

    void fetch(QSqlQuery *q);
    const QString table() { return QString("categories"); }
    Model* model() { return new Category(m_db); }


private:
    struct {
        int  i_id;
        int i_parent_id;
        QString s_name;
        QString s_description;
        int i_type;//decrement, incremet
        double d_tax;
        double value;
    } m_d;
 };

#endif // CATEGORY_H
