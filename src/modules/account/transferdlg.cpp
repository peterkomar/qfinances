/***************************************************************************
 *   Copyright (C) 2017 by Peter Komar                                     *
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

#include "dialog.h"
#include "transferdlg.h"
#include "project.h"
#include "account.h"
#include "transaction.h"
#include "../currency/currency.h"

#include <QRadioButton>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>

TransferDlg::TransferDlg(Transaction *transaction, QWidget *parent)
    :Dialog(parent)
{
    m_transaction = transaction;
}

TransferDlg::~TransferDlg()
{
    m_transaction = nullptr;
}

double TransferDlg::getRate()
{
    return m_uiRate->text().toDouble();
}

int TransferDlg::getRelatedAccountId()
{
    return m_uiAccount->currentData().toInt();
}

void TransferDlg::slot_save()
{
    Transaction::Type type = Transaction::EXPENSES;
    if(m_uiFrom->isChecked()) {
        type = Transaction::INCOMES;
    }
    double amount = QLocale::system().toDouble(m_uiAmount->text());
    if (!amount) {
        QMessageBox::warning(this, tr("Missed param"), tr("Please set ammount of transaction"));
        return;
    }

    m_transaction->setDate(m_uiDate->dateTime());
    m_transaction->setValue(amount);
    m_transaction->setDescription(m_uiComment->toPlainText());
    m_transaction->setType(type);
    accept();
}

void TransferDlg::_gui(QGridLayout *layout)
{
    m_uiFrom = new QRadioButton(tr("From"));
    m_uiTo = new QRadioButton(tr("To"));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_uiFrom);
    hlayout->addWidget(m_uiTo);

    m_uiAccount = new QComboBox;
    m_uiAccount->setMinimumWidth(150);

    m_uiDate = new QDateTimeEdit();
    m_uiDate->setMinimumWidth(150);
    m_uiDate->setCalendarPopup(true);

    m_uiAmount = new QLineEdit();
    QDoubleValidator *v = new QDoubleValidator;
    v->setDecimals(2);
    m_uiAmount->setValidator(v);
    m_uiAmount->setPlaceholderText(m_transaction->displayValue().remove(0,1));

    m_uiLabelRate = new QLabel(tr("Rate:"));
    m_uiLabelRate->setVisible(false);
    m_uiRate = new QLineEdit();
    m_uiRate->setValidator(v);
    m_uiRate->setVisible(false);

    m_uiComment = new QTextEdit();
    m_uiComment->setMinimumWidth(300);

    //Layouts
    QGridLayout *gl = new QGridLayout;
    gl->setContentsMargins(20, 1, 5, 1);
    gl->addWidget(new QLabel(tr("Type:")), 0, 0, Qt::AlignRight);
    gl->addLayout(hlayout, 0, 1, Qt::AlignRight);

    layout->addLayout(gl, 0, 0, Qt::AlignRight | Qt::AlignTop);
    gl->addWidget(new QLabel(tr("Account:")), 1, 0, Qt::AlignRight);
    gl->addWidget(m_uiAccount, 1, 1, Qt::AlignRight);
    addLine(layout, 300);

    gl = new QGridLayout;
    gl->setContentsMargins(20, 1, 5, 1);
    gl->addWidget(new QLabel(tr("Date:")), 0, 0, Qt::AlignRight);
    gl->addWidget(m_uiDate, 0, 1, Qt::AlignRight);
    gl->addWidget(new QLabel(tr("Amount:")), 1, 0, Qt::AlignRight);
    gl->addWidget(m_uiAmount, 1, 1, Qt::AlignRight);
    gl->addWidget(m_uiLabelRate, 2, 0, Qt::AlignRight);
    gl->addWidget(m_uiRate, 2, 1, Qt::AlignRight);
    layout->addLayout(gl, 2, 0, Qt::AlignRight | Qt::AlignTop);
    addLine(layout, 300);

    layout->addWidget(m_uiComment, 4, 0, Qt::AlignCenter | Qt::AlignTop);
}

void TransferDlg::_data()
{
    setWindowTitle(tr("Create Transfer"));

    m_uiFrom->setChecked(true);
    m_uiComment->setPlaceholderText(tr("Comments"));

    m_uiDate->setDisplayFormat("yyyy-MM-dd hh:mm");
    m_uiDate->setDateTime(QDateTime::currentDateTime());

    Account *account = new Account(m_transaction->m_db);
    account->condition()->where = QString("status = %1 AND id <> %2")
            .arg(Account::STATUS_ACTIVE)
            .arg(m_transaction->accountId());
    account->condition()->order = "name ASC";

    Models list = account->items();
    delete account;
    while(!list.isEmpty()) {
        Account *a = dynamic_cast<Account*>(list.takeFirst());
        m_uiAccount->addItem(a->name() + " " + a->currency()->symbol(), a->id());
        delete a;
    }
    qDeleteAll(list);

    connect(m_uiAccount, SIGNAL(currentIndexChanged(int)), this, SLOT(slotAccountChanged()));
    connect(m_uiFrom, SIGNAL(toggled(bool)), this, SLOT(slotAccountChanged()));
    connect(m_uiTo, SIGNAL(toggled(bool)), this, SLOT(slotAccountChanged()));
    slotAccountChanged();
}

void TransferDlg::slotAccountChanged()
{
    int account_id = m_uiAccount->currentData().toInt();
    Account *a = new Account(m_transaction->m_db, account_id);
    if (!a->load()) {
        return;
    }

    Currency *currency = nullptr;
    if (m_uiFrom->isChecked()) {
        currency = m_transaction->account()->currency();
    } else {
        currency = a->currency();
    }

    bool visible = a->currencyId() != m_transaction->account()->currencyId();
    m_uiLabelRate->setVisible(visible);
    m_uiRate->setVisible(visible);
    if (visible) {
        m_uiRate->setText(QString::number(currency->rate()));
    } else {
        m_uiRate->setText("");
    }
    currency = nullptr;
}
