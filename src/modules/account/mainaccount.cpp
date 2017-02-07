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

#include "mainaccount.h"
#include "transactionwidget.h"
#include "accountwidget.h"
#include "account.h"
#include "../currency/currency.h"

MainAccount::MainAccount(ModuleParams *params)
    : Module(params)
{
}

void MainAccount::exec()
{
    Module::exec();

    TransactionWidget *transactions = new TransactionWidget(m_p);
    transactions->gui();
    int indexModule = m_p->m_mainWidget->addWidget(transactions);

    NavigationGroup *accountGroup = m_p->m_navPanel->addItemGroup(tr("Accounts"), indexModule);

    Account *account = new Account(m_p->m_db);
    account->condition()->where = QString("status = %1").arg(Account::STATUS_ACTIVE);
    Models list = account->items();
    delete account;
    while(!list.isEmpty()) {
        Account *a = dynamic_cast<Account*>(list.takeFirst());
        accountGroup->addItem(a->id(), QPixmap(":pictures/account2.png"), a->name() + "\t " + a->currency()->symbol());
        delete a;
    }
    qDeleteAll(list);

    connect(m_p->m_navPanel, SIGNAL(itemClicked(QTreeWidgetItem*,int)), transactions, SLOT(slotSetAccount(QTreeWidgetItem*)));

    AccountWidget *accountWidget  = new AccountWidget(m_p);
    accountWidget->setAccountsGroup(accountGroup);
    accountWidget->gui();
    indexModule  = m_p->m_mainWidget->addWidget(accountWidget);
    m_p->m_cfg->addItem(QPixmap(":pictures/account.png"), "Accounts", indexModule);
}

void MainAccount::_install()
{
    QStringList list = m_p->m_db->db()->tables();
    if( !list.contains("accounts") ) {
        QSqlQuery *q = m_p->m_db->query();
        q->prepare(
              "CREATE TABLE accounts ("
                    "id int,"
                    "name varchar,"
                    "description varchar,"
                    "balance double,"
                    "created_at datetime,"
                    "currency_id int,"
                    "password varchar,"
                    "status int,"
                    "type int,"
                    "parent_id int,"
                    "rate double,"
                    "rate_type int"
              ")"
        );
        m_p->m_db->exec(q);
    }

    if( !list.contains("transactions") ) {
        QSqlQuery *q = m_p->m_db->query();
        q->prepare(
              "CREATE TABLE transactions ("
                    "id int,"
                    "created_at datetime,"
                    "account_id int,"
                    "category_id int,"
                    "type int,"//0 - income, 1 - expense
                    "value double,"
                    "balance double,"//Current balance of account after transaction
                    "description mediumtext,"
                    "status int"//status of transaction 1 active 0 reverted
              ")"
        );
        m_p->m_db->exec(q);
    }

    if( !list.contains("transfers") ) {
        QSqlQuery *q = m_p->m_db->query();
        q->prepare(
              "CREATE TABLE transfers ("
                    "source_transaction_id int,"
                    "destination_transaction_id int,"
                    "rate double,"
                    "created_at datetime"
              ")"
        );
        m_p->m_db->exec(q);
    }
}
