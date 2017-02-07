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

#include "transactionwidget.h"
#include "filterdlg.h"
#include "project.h"
#include "filter.h"
#include "navigationwidget.h"
#include "transactiondlg.h"
#include "transferdlg.h"
#include "transaction.h"
#include "account.h"
#include "transactionsview.h"
#include "accountinfo.h"
#include "accountdlg.h"

#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QMap>
#include <QMenu>
#include <QAction>

TransactionWidget::TransactionWidget(ModuleParams *params)
    :ModuleWidget(params)
    ,m_view(0)
    ,m_account(0)
    ,m_thread(0)
{
    m_filter = new Filter;
}

TransactionWidget::~TransactionWidget()
{    
    stopAccountInfo();
    delete m_view;
    delete m_filter;
    if (m_account) {
        delete m_account;
        m_account = nullptr;
    }
}

void TransactionWidget::topPanel(QVBoxLayout *layout)
{
    QHBoxLayout *phbxLayout = new QHBoxLayout();

    //Init filter buttons
    phbxLayout->setAlignment(Qt::AlignLeft);
    phbxLayout->addWidget(createFiterButton(tr("All"), Transaction::BOTH, "type", true));
    phbxLayout->addWidget(createFiterButton(tr("Incomes"), Transaction::INCOMES, "type"));
    phbxLayout->addWidget(createFiterButton(tr("Expenses"), Transaction::EXPENSES, "type"));
    phbxLayout->addWidget(createFiterButton(tr("This Year"), Filter::THIS_YEAR, "report", true));
    phbxLayout->addWidget(createFiterButton(tr("This Month"), Filter::THIS_MONTH, "report"));
    phbxLayout->addWidget(createFiterButton(tr("Prev Year"), Filter::PREV_YEAR, "report"));
    phbxLayout->addWidget(createFiterButton(tr("Prev Month"), Filter::PREV_MONTH, "report"));

    QPushButton *btn = createFiterButton(tr("Custom"), Filter::CUSTOM, "report");
    connect(btn, &QPushButton::clicked, this, &TransactionWidget::slotCustomFilter);
    phbxLayout->addWidget(btn);

    layout->addLayout(phbxLayout);
}

void TransactionWidget::mainPanel(QVBoxLayout *layout)
{
    m_view = new TransactionsView();
    layout->addWidget(m_view);

    connect(m_view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotView()));
}

void TransactionWidget::bottomPanel(QVBoxLayout *layout)
{
    QHBoxLayout *phbxLayout = new QHBoxLayout();
    phbxLayout->setAlignment(Qt::AlignLeft);

    QMenu *createMenu = new QMenu;
    createMenu->addAction(tr("Transfer"), this, &TransactionWidget::slotTransfer);

    QToolButton *newBtn = createToolButton(tr("New"), tr("Add new transaction"), ":pictures/new.png");
    newBtn->setMenu(createMenu);
    connect(newBtn, &QToolButton::clicked, this, &TransactionWidget::slotNew);
    phbxLayout->addWidget(newBtn);

    QToolButton *revertBtn = createToolButton(tr("Revert"), tr("Revert last  transaction"), ":pictures/revert.png");
    connect(revertBtn, SIGNAL(clicked()), this, SLOT(slotRevert()));
    phbxLayout->addWidget(revertBtn);

    QToolButton *viewBtn = createToolButton(tr("View"), tr("View transactions details"), ":pictures/view.png");
    connect(viewBtn, SIGNAL(clicked()), this, SLOT(slotView()));
    phbxLayout->addWidget(viewBtn);

    phbxLayout->addStretch(0);

    QToolButton *printBtn = createToolButton(tr("Print"), tr("Print transactions"), ":pictures/print.png");
    //TODO: add connection here
    phbxLayout->addWidget(printBtn);

    QToolButton *chartBtn = createToolButton(tr("Reports"), tr("View repors with charts  transactions"), ":pictures/view-statistics.png");
    chartBtn->setEnabled(false);
    //TODO: add connection here
    phbxLayout->addWidget(chartBtn);

    layout->addLayout(phbxLayout);
}

QPushButton* TransactionWidget::createFiterButton(const QString& text, int property, const char* group, bool bChecked)
{
  QPushButton *button = new QPushButton(text);
  button->setFlat(true);
  button->setCheckable(true);
  button->setProperty("name", property);
  button->setProperty("group", group);
  button->setChecked(bChecked);

  connect(button, &QPushButton::clicked, this, &TransactionWidget::slotFilterCheck);

  if( bChecked ) {
    m_filterPanelGroups.insert(group, button);
  }

  return button;
}

void TransactionWidget::slotFilterCheck()
{
  if (QPushButton *button = qobject_cast<QPushButton *>(sender())) {

    QString group = button->property("group").toString();

    //Check if user click on the same buttom
    if( m_filterPanelGroups[group]->property("name") == button->property("name") ) {
      button->toggle();
      return;
    }

    m_filterPanelGroups[group]->setChecked(false);
    m_filterPanelGroups[group] = button;

    //If standart filter
    if (group == "type") {
        m_filter->setType((Transaction::Type)button->property("name").toInt());
        slotDoFilter();
        return;
    }

    m_filter->setPeriod((Filter::Period)button->property("name").toInt());

    //if custom filter
    if(button->property("name").toInt() != Filter::CUSTOM) {
        m_filter->reset();
        slotDoFilter();
    }
  }
}

void TransactionWidget::slotCustomFilter()
{
    FilterDlg *filterDlg = new FilterDlg(m_filter, m_db);
    connect(filterDlg, &FilterDlg::filter, this, &TransactionWidget::slotDoFilter);
    filterDlg->exec();
    disconnect(filterDlg, &FilterDlg::filter, this, &TransactionWidget::slotDoFilter);
    delete filterDlg;
}

void TransactionWidget::slotDoFilter()
{      
    m_view->reload();
    loadAccountInfo();
}

void TransactionWidget::stopAccountInfo()
{
    if (m_thread) {
        if (m_thread->isRunning()) {
            m_thread->terminate();
            m_thread->wait();
        }
        delete m_thread;
        m_thread = nullptr;
    }
}

void TransactionWidget::loadAccountInfo()
{
    stopAccountInfo();
    m_thread = new AccountInfo(m_account, m_filter, this);
    connect(m_thread, &AccountInfo::resultReady, this, &TransactionWidget::slotSetProperties);
    m_thread->start();
}

void TransactionWidget::slotSetAccount(QTreeWidgetItem *item)
{
    NavigationItem* navItem = static_cast<NavigationItem*>(item);

    if (m_account) {
        if (m_account->id() == navItem->indexItem()) {
            return;
        }
        stopAccountInfo();
        delete m_account;
        m_account = nullptr;
    }

    m_account = new Account(m_db, navItem->indexItem());
    m_account->load();

    if(!AccountDlg::auth(m_account, this)) {
        delete m_account;
        m_account = nullptr;
        m_view->reset();
        return;
    }

    m_view->loadTransactions(m_account, m_filter);
    loadAccountInfo();
}

void TransactionWidget::slotSetProperties()
{
    m_properties->clear();

    PropertiesInfo info = m_thread->getResults();
    QMapIterator<QString, QMap<QString, QString>> i(info);
    while (i.hasNext()) {
        i.next();

        QTreeWidgetItem *parent = m_properties->addGroup(i.key());

        QMapIterator<QString, QString> j(i.value());
        while (j.hasNext()) {
            j.next();
            m_properties->addProperty(j.key(), j.value(), parent);
        }
    }

    m_properties->expandAll();
}

void TransactionWidget::slotNew()
{
    Transaction* transaction = new Transaction(m_db, m_account);
    TransactionDlg *dlg = new TransactionDlg(transaction);
    if( QDialog::Accepted == dlg->dialog() ) {

        if(m_account->addTransaction(transaction)) {
            slotDoFilter();
        } else {
            QMessageBox::critical(this, tr("Transaction"), m_account->lastErrorMessage());
        }       
    }
    delete dlg;
    delete transaction;
}

void TransactionWidget::slotTransfer()
{
    Transaction* transaction = new Transaction(m_db, m_account);
    TransferDlg *dlg = new TransferDlg(transaction);
    if( QDialog::Accepted == dlg->dialog() ) {

        if(m_account->addTransfer(transaction, dlg->getRelatedAccountId(), dlg->getRate())) {
            slotDoFilter();
        } else {
            QMessageBox::critical(this, tr("Transaction"), m_account->lastErrorMessage());
        }
    }
    delete dlg;
    delete transaction;
}

void TransactionWidget::slotRevert()
{
    Transaction *transaction = getSelectedTransaction();
    if (!transaction) {
        return;
    }

    int btn = QMessageBox::question(this, tr("Revert transaction"),
                                    tr("Are you shure want to revert: %1 ?").arg(transaction->id()));
    if( btn == QMessageBox::Yes ) {
        if (m_account->revertTransaction(transaction)) {
            slotDoFilter();
        } else {
            QMessageBox::critical(this, tr("Transaction"), m_account->lastErrorMessage());
        }
    }
    delete transaction;
}

void TransactionWidget::slotView()
{
    Transaction *transaction = getSelectedTransaction();
    if (!transaction) {
        return;
    }

    TransactionDlg *dlg = new TransactionDlg(transaction, this, true);
    dlg->dialog();

    delete dlg;
    delete transaction;

}

Transaction* TransactionWidget::getSelectedTransaction()
{
    Transaction *transaction = nullptr;
    if(!m_view->selectionModel()->hasSelection()) {
        return transaction;
    }

    QModelIndexList selection = m_view->selectionModel()->selectedRows();
    QModelIndex index = selection.at(0);
    int transaction_id = index.data().toInt();

    transaction = new Transaction(m_db, m_account);
    transaction->setId(transaction_id);
    if (!transaction->load()) {
        delete transaction;
        transaction = nullptr;
    }
    return transaction;
}
