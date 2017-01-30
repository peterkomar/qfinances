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

#include "maincurrency.h"
#include "currencywidget.h"
#include "currency.h"

MainCurrency::MainCurrency(ModuleParams *params)
    : Module(params)
{
}

void MainCurrency::exec()
{
    Module::exec();

    CurrencyWidget *panel = new CurrencyWidget(m_p);
    panel->gui();

    int index = m_p->m_mainWidget->addWidget(panel);

    m_p->m_cfg->addItem(
                QPixmap(":pictures/currency.png"),
                "Currencies",
               index
                );
}

void MainCurrency::_install()
{
    QStringList list = m_p->m_db->db()->tables();
    if (list.contains("currency")) {
        return;
    }

    QSqlQuery *q = m_p->m_db->query();
    q->prepare(
        "CREATE TABLE currency ("
          "id int,"
          "name varchar,"
          "code varcchar,"
          "symbol_left varchar,"
          "symbol_right varchar,"
          "value double,"
          "is_removed int"
        ")"
    );
    m_p->m_db->exec(q);
    this->_installData("currencies");
}
