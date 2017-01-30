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

#include "currencywidget.h"
#include "currencydlg.h"
#include "currency.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMessageBox>


CurrencyWidget::CurrencyWidget(ModuleParams *params) :
    ModuleWidget(params)
{    
}

void CurrencyWidget::topPanel(QVBoxLayout *layout)
{
    Q_UNUSED(layout);
}

void CurrencyWidget::mainPanel(QVBoxLayout *layout)
{
    m_grid = new Grid;
    m_grid->setColumnCount(4);
    QStringList labels;
    labels << "#" << tr("Name") << tr("Code") << tr("Rate");

    m_grid->setHorizontalHeaderLabels(labels);
    layout->addWidget(m_grid);
    _loadData();

    connect(m_grid, SIGNAL(itemSelectionChanged()), this, SLOT(slotEnableEditing()));
}

void CurrencyWidget::bottomPanel(QVBoxLayout *layout)
{
    QHBoxLayout *phbxLayout = new QHBoxLayout();
    phbxLayout->setAlignment(Qt::AlignLeft);
    phbxLayout->setMargin(2);

    QToolButton *newBtn = createToolButton(tr("New"), tr("Add new currency"), ":pictures/new.png");
    newBtn->setProperty("type_btn", BTN_NEW);
    connect(newBtn, SIGNAL(clicked()), this, SLOT(slotEditCurrency()));
    phbxLayout->addWidget(newBtn);

    editBtn = createToolButton(tr("Edit"), tr("Edit currency"), ":pictures/edit.png");
    editBtn->setDisabled(true);
    editBtn->setProperty("type_btn", BTN_EDIT);
    connect(editBtn, SIGNAL(clicked()), this, SLOT(slotEditCurrency()));
    phbxLayout->addWidget(editBtn);

    phbxLayout->addStretch(0);

    remBtn = createToolButton(tr("Remove"), tr("Remove currency"), ":pictures/remove.png");
    remBtn->setDisabled(true);
    connect(remBtn, SIGNAL(clicked()), this, SLOT(slotDeleteCurrency()));
    phbxLayout->addWidget(remBtn);

    layout->addLayout(phbxLayout);
}


void CurrencyWidget::slotEditCurrency()
{
    int id_currency = 0;
    if( QToolButton *btn = qobject_cast<QToolButton *>(sender())) {
        int row = -1;
        if((btn->property("type_btn") == BTN_EDIT) && ((row = m_grid->currentRow()) != -1) ) {
            QTableWidgetItem *item = m_grid->item(row, 0);
            id_currency = item->text().toInt();
        }
    }

    Currency *c = new Currency(m_db, id_currency);
    c->load();

    CurrencyDlg *dlg = new CurrencyDlg(c, this);
    if( id_currency ) {
        dlg->setWindowTitle(QString("%1 %2").arg(tr("Edit currency")).arg(c->name()));
    } else {
        dlg->setWindowTitle(tr("Add new currency"));
    }

    if( QDialog::Accepted == dlg->dialog() ) {
        _loadData();
    }

    delete dlg;
    delete c;
}

void CurrencyWidget::slotEnableEditing()
{
    QTableWidgetItem *cell = m_grid->item(m_grid->currentRow(), 0);
    if( cell && !cell->text().isEmpty() ) {
        editBtn->setEnabled(true);
        remBtn->setEnabled(true);
    } else {
        editBtn->setEnabled(false);
        remBtn->setEnabled(false);
    }
}

void CurrencyWidget::slotDeleteCurrency()
{
    int row = -1;
    int id_currency = 0;
    if(((row = m_grid->currentRow()) != -1)) {
        QTableWidgetItem *item = m_grid->item(row, 0);
        id_currency = item->text().toInt();
    } else {
        return;
    }

    Currency *c = new Currency(m_db, id_currency);
    if(c->load()) {
        QMessageBox::critical(this, tr("Error"), tr("Can't find currency"));
        return;
    }

    int btn = QMessageBox::question(this,tr("Confirm remove currency"), tr("Are you shure want  to remove: %1 ?").arg(c->name()));
    if( btn == QMessageBox::Yes ) {
        try {
            c->remove();
        } catch(int code) {
            QMessageBox::critical(this, tr("Error delete"), tr("Can't remove currency"));
            delete c;
            return;
        }
    }
    delete c;
    _loadData();

    editBtn->setDisabled(true);
    remBtn->setDisabled(true);
}

void CurrencyWidget::_loadData()
{
    m_grid->clearContents();
    Currency *currency = new Currency(m_db);
    currency->condition()->where = "is_removed = 0";
    Models list = currency->items();
    delete currency;

    if( list.size() > m_grid->rowCount() ) {
       m_grid->setRowCount(list.size() + 10);
    }

    int i = 0;
    while(!list.isEmpty()) {
        Currency *c = dynamic_cast<Currency*>(list.takeFirst());

        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(c->id()));
        m_grid->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(tr("%1").arg(c->name()));
        m_grid->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(tr("%1").arg(c->code()));
        m_grid->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(tr("%1").arg(c->rate()));
        m_grid->setItem(i, 3, newItem);

        ++i;
        delete c;
    }
}
