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

#ifndef FNCMODEL_H
#define FNCMODEL_H

#include <QString>
#include <QList>

class DataBase;
class QSqlQuery;
class Model;

typedef QList<Model*> Models;

class Condition {
public:
    Condition(Model *m);
    ~Condition();

    Model *model;
    QString where;
    QString order;
    int start;
    int limit;
};

class Model
{
public:
    Model(DataBase *m_db, int id = 0);
    virtual ~Model();

    virtual int id() = 0;
    virtual bool save() = 0;
    void remove();
    bool load();

    DataBase *m_db;
    QString lastErrorMessage() const { return m_error_message; }

    Condition* condition();
    Models items(bool resetCondition = true);

protected:
    virtual void fetch(QSqlQuery *q) = 0;
    virtual const QString table() = 0;
    virtual Model* model() = 0;

    QString m_error_message;

private:
    Condition* m_condition;
};

#endif // FNCMODEL_H
