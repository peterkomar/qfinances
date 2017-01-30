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

#include "category.h"
#include "database.h"

Category::Category(DataBase *db, int id)
    : Model(db, id)
{
    init();
    m_d.i_id = id;
}

Category::~Category()
{
}

void Category::init()
{
    m_d.i_id  = 0;
    m_d.i_parent_id = 0;
    m_d.s_name = "";
    m_d.s_description = "";
    m_d.i_type = 0;
    m_d.d_tax = 0.0;
    m_d.value = 0.0;
}

bool Category::hasChildren()
{
    QSqlQuery *q = m_db->query();
    q->prepare("SELECT COUNT(*) FROM "+this->table()+" WHERE parent_id = :id");
    q->bindValue(":id", m_d.i_id);
    m_db->exec(q);

    if (q->first()) {
        return q->value(0).toInt() > 0;
    }
    return false;
}

bool Category::save()
{
    QSqlQuery *q = m_db->query();

    if( m_d.i_id == 0 ) {
        m_d.i_id = m_db->nextId(table());
        q->prepare(""
         "INSERT INTO "+table()+" (id, parent_id, name, description, type, tax, value)"
         " VALUES (:id, :parent_id, :name, :description, :type, :tax, :value)"
        );
    } else {
        q->prepare(""
         "UPDATE "+table()+" SET"
         " id = :id, parent_id = :parent_id, name = :name, description = :description, type = :type, tax = :tax, value = :value"
         " WHERE id = :id"
        );
    }

    q->bindValue(":id", m_d.i_id);
    q->bindValue(":parent_id", m_d.i_parent_id);
    q->bindValue(":name", m_d.s_name);
    q->bindValue(":description", m_d.s_description);
    q->bindValue(":type", m_d.i_type);
    q->bindValue(":tax", m_d.d_tax);
    q->bindValue(":value", m_d.value);
    m_db->exec(q);

    return true;
}

void Category::fetch(QSqlQuery *q)
{
    m_d.i_id           = q->value("id").toInt();
    m_d.i_parent_id    = q->value("parent_id").toInt();
    m_d.s_name         = q->value("name").toString();
    m_d.s_description  = q->value("description").toString();
    m_d.i_type         = q->value("type").toInt();
    m_d.d_tax          = q->value("tax").toDouble();
    m_d.value          = q->value("value").toDouble();
}

void Category::remove()
{
    QSqlQuery *q = m_db->query();
    q->prepare("UPDATE "+this->table()+" SET status = 0 WHERE id = :id");
    q->bindValue(":id", m_d.i_id);
    m_db->exec(q);
}
