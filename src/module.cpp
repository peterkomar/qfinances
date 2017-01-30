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

#include "module.h"

#include <QLocale>
#include <QFile>

Module::Module(ModuleParams *params)
{
    m_p = params;
}

Module::~Module()
{

}

void Module::exec()
{
    this->_install();
}

void Module::_installData(const QString& name)
{
    QString file = name + QLocale::system().name();
    if (!QFile::exists(":/demo/sql/"+file+".sql")) {
        file = name;
    }

    QFile data("://demo/sql/"+file+".sql");
    if (!data.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&data);
    while (!in.atEnd()) {
        QString sql = in.readLine();
        QSqlQuery *q = m_p->m_db->query();
        q->prepare(sql);
        m_p->m_db->exec(q);
    }
    data.close();
}
