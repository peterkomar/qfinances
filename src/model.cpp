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

#include "model.h"
#include "database.h"

Condition::Condition(Model *m)
    :model(m)
    ,where("")
    ,order("")
    ,start(0)
    ,limit(0)
{
}

Condition::~Condition()
{
   model = nullptr;
}

Model::Model(DataBase *db, int id)
{
    Q_UNUSED(id);
    m_db = db;
    m_error_message = "";
    m_condition = nullptr;
}

Model::~Model()
{
    m_db = nullptr;
    if (m_condition) {
        delete m_condition;
        m_condition = nullptr;
    }
}

void Model::remove()
{
    QSqlQuery *q = m_db->query();
    q->prepare("DELETE FROM "+this->table()+" WHERE id = :id");
    q->bindValue(":id", this->id());
    m_db->exec(q);
}

bool Model::load()
{
    QSqlQuery *q = m_db->query();
    q->prepare("SELECT * FROM "+this->table()+" WHERE id = :id");
    q->bindValue(":id", this->id());
    m_db->exec(q);

    bool result = false;
    if ((result = q->first())) {
        this->fetch(q);
    }

    delete q;
    return result;
}

Condition* Model::condition()
{
    if (!m_condition) {
        m_condition = new Condition(this);
    }
    return m_condition;
}

Models Model::items(bool resetCondition)
{
    this->condition();
    QSqlQuery *q = m_db->query();

    QString query = "SELECT * FROM "+m_condition->model->table();
    if(!m_condition->where.isEmpty()) {
        query += " WHERE " + m_condition->where;
    }

    if (!m_condition->order.isEmpty()) {
        query += " ORDER BY " + m_condition->order;
    }

    if (m_condition->start != 0 || m_condition->limit != 0) {
        query += QString(" LIMIT %1, %2")
                .arg(m_condition->start)
                .arg(m_condition->limit);
    }

    q->prepare(query);
    try {
        m_db->exec(q);
    } catch (int code) {
        if (resetCondition) delete m_condition;
        throw code;
    }

    Models list;
    while (q->next()) {
        Model* item = m_condition->model->model();
        item->fetch(q);
        list.append(item);
    }

    if (resetCondition) {
        delete m_condition;
        m_condition = nullptr;
    }
    delete q;
    return list;
}

