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

#include "maincategory.h"
#include "categorywidget.h"
#include "category.h"
#include "../account/filter.h"

#include <QFile>

MainCategory::MainCategory(ModuleParams *params)
    : Module(params)
{
}

void MainCategory::exec()
{
    Module::exec();

    CategoryWidget *panel = new CategoryWidget(m_p);
    panel->gui();

    int index= m_p->m_mainWidget->addWidget(panel);
    m_p->m_cfg->addItem(QPixmap(":pictures/category.png"), tr("Categories"), index);
}

void MainCategory::_install()
{
    QStringList list = m_p->m_db->db()->tables();
    if( list.contains("categories") ) {
        return;
    }

    QSqlQuery *q = m_p->m_db->query();
    q->prepare(
        "CREATE TABLE categories ("
          "id int,"
          "parent_id int,"
          "name varchar,"
          "description varcchar,"
          "type int,"
          "tax double,"
          "value double,"
          "status int"
        ")"
    );
    m_p->m_db->exec(q);    
    this->_installData("categories");
}
