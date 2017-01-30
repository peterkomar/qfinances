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
#include <QHeaderView>

#include "transactionsview.h"
#include "transaction.h"
#include "account.h"
#include "filter.h"

TransactionsView::TransactionsView(QWidget *parent)
    : QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    horizontalHeader()->setStretchLastSection(true);
    setAlternatingRowColors(true);
    verticalHeader()->setDefaultSectionSize(20);
}

TransactionsView::~TransactionsView()
{
    delete ((Transaction*)model());
}

void TransactionsView::reload()
{
    reset();
    ((Transaction*)model())->read();
}

void TransactionsView::loadTransactions(Account *account, Filter *filter)
{
    Transaction *transaction = new Transaction(account->m_db, account);
    transaction->setFilter(filter);
    setModel(transaction);
    resizeColumnsToContents();
    horizontalHeader()->resizeSection(2, horizontalHeader()->sectionSize(2) + 5);
}
