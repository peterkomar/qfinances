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

#include "accountwidget.h"
#include "navigationwidget.h"
#include "account.h"
#include "accountdlg.h"
#include "../currency/currency.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMessageBox>

AccountWidget::AccountWidget(ModuleParams *params)
    : ModuleWidget(params)
    ,m_accountsGroup(0)
{
}

AccountWidget::~AccountWidget()
{
    m_accountsGroup = 0;
}

void AccountWidget::setAccountsGroup(NavigationGroup *accountGroup)
{
    m_accountsGroup = accountGroup;
}

void AccountWidget::topPanel(QVBoxLayout *layout)
{
    Q_UNUSED(layout);
}

void AccountWidget::mainPanel(QVBoxLayout *layout)
{
    m_grid = new Grid;
    m_grid->setColumnCount(7);
    QStringList labels;
    labels << "#" << tr("Name") << tr("Currency") << tr("Protected")  << tr("Status") << tr("Type") << tr("Description");

    m_grid->setHorizontalHeaderLabels(labels);
    layout->addWidget(m_grid);
    _loadData();

    connect(m_grid, SIGNAL(itemSelectionChanged()),this, SLOT(slotEnableEditing()));
}

void AccountWidget::bottomPanel(QVBoxLayout *layout)
{
    QHBoxLayout *phbxLayout = new QHBoxLayout();
    phbxLayout->setAlignment(Qt::AlignLeft);
    phbxLayout->setMargin(2);

    QToolButton *newBtn = createToolButton(tr("New"), tr("Add new account"), ":pictures/new.png");
    newBtn->setProperty("type_btn", BTN_NEW);
    connect(newBtn, SIGNAL(clicked()), this, SLOT(slotEdit()));
    phbxLayout->addWidget(newBtn);

    editBtn = createToolButton(tr("Edit"), tr("Edit account"), ":pictures/edit.png");
    editBtn->setDisabled(true);
    editBtn->setProperty("type_btn", BTN_EDIT);
    connect(editBtn, SIGNAL(clicked()), this, SLOT(slotEdit()));
    phbxLayout->addWidget(editBtn);

    phbxLayout->addStretch(0);

    closeBtn = createToolButton(tr("Close"), tr("Close account"), ":pictures/remove.png");
    closeBtn->setDisabled(true);
    connect(closeBtn, SIGNAL(clicked()), this, SLOT(slotClose()));
    phbxLayout->addWidget(closeBtn);

    remBtn = createToolButton(tr("Remove"), tr("Remove account"), ":pictures/remove.png");
    remBtn->setDisabled(true);
    connect(remBtn, SIGNAL(clicked()), this, SLOT(slotDelete()));
    phbxLayout->addWidget(remBtn);

    layout->addLayout(phbxLayout);
}

void AccountWidget::slotEdit()
{
    int id_account = 0;
    if( QToolButton *btn = qobject_cast<QToolButton *>(sender())) {
        int row = -1;
        if((btn->property("type_btn") == 1) && ((row = m_grid->currentRow()) != -1) ) {
            QTableWidgetItem *item = m_grid->item(row, 0);
            id_account = item->text().toInt();
        }
    }

    Account *a = new Account(m_db, id_account);
    a->load();

    QString title;
    if(!id_account) {
        Currency* c = Currency::getDefaultCurrency(m_db);
        a->setCurrencyId(c->id());
        delete c;
        title = tr("Create new account");
    } else {
        title = tr("Edit account #%2").arg(a->id());
    }

    try {
        AccountDlg *dlg = new AccountDlg(a, this);
        dlg->setWindowTitle(title);

        if( QDialog::Accepted == dlg->dialog() ) {
            _loadData();
            m_accountsGroup->removeItem(a->id());
            m_accountsGroup->addItem(a->id(),
                                  QPixmap(":pictures/account2.png"),
                                  a->name() + "\t " + a->currency()->symbol()
                                  );
        }
        delete dlg;
    } catch(int) {}

    delete a;
}

void AccountWidget::slotEnableEditing()
{
    QTableWidgetItem *cell = m_grid->item(m_grid->currentRow(), 0);
    if( cell && !cell->text().isEmpty() ) {
        editBtn->setEnabled(true);
        remBtn->setEnabled(true);
        closeBtn->setEnabled(true);
    } else {
        editBtn->setEnabled(false);
        remBtn->setEnabled(false);
        closeBtn->setEnabled(false);
    }
}

void AccountWidget::slotDelete()
{
    int row = -1;
    int id = 0;
    if(((row = m_grid->currentRow()) != -1)) {
        QTableWidgetItem *item = m_grid->item(row, 0);
        id = item->text().toInt();
    } else {
        return;
    }

    Account *a = new Account(m_db, id);
    a->load();

    int btn = QMessageBox::question(this, tr("Confirm remove account"), tr("Are you shure want to remove: %1 ?").arg(a->name()));
    if( btn == QMessageBox::Yes ) {
        a->remove();
        m_accountsGroup->removeItem(id);
    }

    delete a;
    _loadData();    

    editBtn->setDisabled(true);
    remBtn->setDisabled(true);
    closeBtn->setDisabled(true);
}

void AccountWidget::slotClose()
{
    int row = -1;
    int id = 0;
    if(((row = m_grid->currentRow()) != -1)) {
        QTableWidgetItem *item = m_grid->item(row, 0);
        id = item->text().toInt();
    } else {
        return;
    }

    Account *a = new Account(m_db, id);
    a->load();

    int btn = QMessageBox::question(this, tr("Confirm closing account"), tr("Are you shure want to close: %1 ?").arg(a->name()));
    if( btn == QMessageBox::Yes ) {
        if(!a->close()) {
            QMessageBox::critical(this, tr("Denied"), tr("Balance of account mast be 0"));
        }
    }

    delete a;
    _loadData();

    editBtn->setDisabled(true);
    remBtn->setDisabled(true);
    closeBtn->setDisabled(true);
}

void AccountWidget::_loadData()
{
    m_grid->clearContents();

    Account *a = new Account(m_db);
    Models list = a->items();
    delete a;
    if( list.size() > m_grid->rowCount() ) {
       m_grid->setRowCount(list.size() + 10);
    }

    int i = 0;
    while(!list.isEmpty()) {

        a = dynamic_cast<Account*>(list.takeFirst());

        //ID
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(a->id()));
        m_grid->setItem(i, 0, newItem);

        //Name
        newItem = new QTableWidgetItem(a->name());
        m_grid->setItem(i, 1, newItem);

        //Currency
        newItem = new QTableWidgetItem(a->currency()->name());
        m_grid->setItem(i, 2, newItem);

        //Password protected
        newItem = new QTableWidgetItem(a->password().isEmpty()? tr("No") : tr("Yes"));
        m_grid->setItem(i, 3, newItem);

        //Status
        newItem = new QTableWidgetItem(a->statusText());
        m_grid->setItem(i, 4, newItem);

        //Type
        newItem = new QTableWidgetItem(a->typeText());
        m_grid->setItem(i, 5, newItem);

        //Description
        newItem = new QTableWidgetItem(!a->description().isEmpty()? a->description().mid(0, 20) + "..." : "");
        m_grid->setItem(i, 6, newItem);

        ++i;
        delete a;
    }
}

