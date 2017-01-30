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

#include "accountdlg.h"
#include "account.h"
#include "project.h"
#include "../currency/currency.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QRadioButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QInputDialog>

AccountDlg::AccountDlg(Account *account, QWidget *parent)
    : Dialog(parent)
{
    if(!AccountDlg::auth(account, this)) {
        throw -1;
    }

    m_account = account;
}

AccountDlg::~AccountDlg()
{
    m_account = 0;
}

bool AccountDlg::auth(Account *account, QWidget *parent)
{
    if (account->password().isEmpty()) {
        return true;
    }

    bool ok = false;
    QString passwd = QInputDialog::getText(parent,
                                           tr("Account Authorization"),
                                           tr("Password:"),
                                           QLineEdit::Password,
                                           QString(""),
                                           &ok,
                                           Qt::Dialog
                                           | Qt::WindowTitleHint
                                           | Qt::CustomizeWindowHint
                                           );
    if (!ok) {
        return false;
    }

    if (!account->auth(passwd)) {
        QMessageBox::critical(parent, tr("Failed authorization"), tr("Access denied"));
        return false;
    }

    return true;
}

void AccountDlg::slot_save()
{
    if(ui_name->text().isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Name account can't be empty"));
        return;
    }

    if(!m_account->password().isEmpty() && !m_account->auth(ui_password3->text())) {
        QMessageBox::critical(this, tr("Error"), tr("Old password wrong"));
        return;
    }

    if(!ui_password->text().isEmpty() && ui_password->text() != ui_password2->text()) {
        QMessageBox::critical(this, tr("Error"), tr("Passwords not match"));
        return;
    }

    m_account->setName(ui_name->text());
    m_account->setDescription(ui_description->toPlainText());

    //Save this values if only when account creating
    if(m_account->id() == 0) {
        m_account->setBalance(ui_balance->text().toDouble());
        m_account->setCurrencyId(ui_currency->currentData().toInt());
    }

    if(!ui_password->text().isEmpty()) {
        m_account->encriptPassword(ui_password->text());
    }

    switch (m_account->type()) {
    case Account::TYPE_ACCOUNT:
        m_account->setParentId(0);
        m_account->setRate(0.0);
        m_account->setRateType(Account::RATE_TYPE_MONTHLY);
        break;
    case Account::TYPE_CREDIT:
    case Account::TYPE_INVESTMENT:

        int parent_id = ui_bindAccount->currentData().toInt();

        if( !parent_id ) {
            QMessageBox::critical(this, tr("Error"), tr("Please select bind account"));
            return;
        }

        m_account->setParentId(ui_bindAccount->currentData().toInt());
        m_account->setRate(ui_rate->text().toDouble());
        m_account->setRateType((Account::TypeRate)ui_rateType->currentData().toInt());

        Account *a = new Account(m_account->m_db, m_account->parentId());
        a->load();
        double balance = a->balance();
        delete a;

        if(balance < m_account->balance()) {
            QMessageBox::critical(this, tr("Error"), tr("Balance of <b>%1</b> can't be bigger than bound account balance").arg(m_account->typeText()));
            return;
        }

        break;
    }

    saveData(m_account);
}

void AccountDlg::slot_typeAccount()
{
    if(ui_chTypeCredit->isChecked() || ui_chTypeInvestment->isChecked()) {

        if( ui_chTypeCredit->isChecked() ) {
            m_account->setType(Account::TYPE_CREDIT);
        } else {
            m_account->setType(Account::TYPE_INVESTMENT);
        }

        ui_currency->setDisabled(true);
        slot_changeAccount();

        ui_l_bindAccount->setVisible(true);
        ui_bindAccount->setVisible(true);
        ui_l_rate->setVisible(true);
        ui_rate->setVisible(true);
        ui_l_rateType->setVisible(true);
        ui_rateType->setVisible(true);
    } else {
        ui_l_bindAccount->setVisible(false);
        ui_bindAccount->setVisible(false);
        ui_l_rate->setVisible(false);
        ui_rate->setVisible(false);
        ui_l_rateType->setVisible(false);
        ui_rateType->setVisible(false);

        ui_currency->setDisabled(false);

        m_account->setType(Account::TYPE_ACCOUNT);
    }
}

void AccountDlg::slot_editPassword(const QString& )
{
    if(!ui_password->text().isEmpty()) {
        ui_l_password2->setVisible(true);
        ui_password2->setVisible(true);

        if(!m_account->password().isEmpty()) {
            ui_l_password3->setVisible(true);
            ui_password3->setVisible(true);
        }

    } else {
        ui_l_password2->setVisible(false);
        ui_password2->setVisible(false);

        ui_l_password3->setVisible(false);
        ui_password3->setVisible(false);
    }
}

void AccountDlg::slot_changeAccount()
{
    int id = ui_bindAccount->currentData().toInt();
    Account *a = new Account(m_account->m_db, id);
    a->load();

    int index = ui_currency->findData(a->currency()->id());
    if( index != -1 ) {
        ui_currency->setCurrentIndex(index);
    }
    delete a;
}

void AccountDlg::_gui(QGridLayout *layout)
{
    ////////////////////////////////////////////////////////////////////
    ui_typeBox = new QGroupBox(tr("Type account:"));

    ui_chTypeAccount = new QRadioButton(tr("Account"));
    ui_chTypeCredit  = new QRadioButton(tr("Credit"));
    ui_chTypeInvestment = new QRadioButton(tr("Investment"));
    ui_chTypeAccount->setChecked(true);

    QGridLayout *gl = new QGridLayout;
    gl->addWidget(ui_chTypeAccount, 0, 0, 2, 1, Qt::AlignLeft | Qt::AlignTop);
    gl->addWidget(ui_chTypeCredit, 0, 1, Qt::AlignLeft | Qt::AlignTop);
    gl->addWidget(ui_chTypeInvestment, 1, 1, Qt::AlignLeft | Qt::AlignTop);

    ui_typeBox->setLayout(gl);
    connect(ui_chTypeAccount, SIGNAL(clicked()), this, SLOT(slot_typeAccount()));
    connect(ui_chTypeCredit, SIGNAL(clicked()), this, SLOT(slot_typeAccount()));
    connect(ui_chTypeInvestment, SIGNAL(clicked()), this, SLOT(slot_typeAccount()));
    ////////////////////////////////////////////////////////////////////////

    ui_name = new QLineEdit();
    ui_name->setMinimumWidth(200);

    ///////////////////////////////////////////////////////////////////////

    int width = 130;
    ui_l_bindAccount = new QLabel(tr("Bind account:"));
    ui_bindAccount = new QComboBox;
    ui_bindAccount->setMaximumWidth(width);
    connect(ui_bindAccount, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_changeAccount()));

    ui_l_balance = new QLabel(tr("Balance:"));
    ui_balance = new QLineEdit;
    ui_balance->setMaximumWidth(width);
    QDoubleValidator *v = new QDoubleValidator(ui_balance);
    v->setDecimals(2);
    v->setBottom(0.0);
    ui_balance->setValidator(v);

    ui_l_currency = new QLabel(tr("Currency:"));
    ui_currency = new QComboBox;
    ui_currency->setMaximumWidth(width);

    ui_l_rate = new QLabel(tr("Rate:"));
    ui_rate = new QDoubleSpinBox();
    ui_rate->setDecimals(2);
    ui_rate->setMaximum(99.0);
    ui_rate->setMinimum(0.0);

    ui_l_rateType = new QLabel(tr("Rate type:"));
    ui_rateType = new QComboBox;

    ///////////////////////////////////////////////////////////////////////

    ui_l_password = new QLabel(tr("Password:"));
    ui_password = new QLineEdit;
    ui_password->setEchoMode(QLineEdit::Password);
    ui_password->setMaximumWidth(width);
    connect(ui_password, SIGNAL(textEdited(QString)), this, SLOT(slot_editPassword(QString)));

    ui_l_password2 = new QLabel(tr("Retype password:"));
    ui_l_password2->setVisible(false);
    ui_password2 = new QLineEdit;
    ui_password2->setEchoMode(QLineEdit::Password);
    ui_password2->setVisible(false);
    ui_password2->setMaximumWidth(width);

    ui_l_password3 = new QLabel(tr("Old password:"));
    ui_l_password3->setVisible(false);
    ui_password3 = new QLineEdit;
    ui_password3->setEchoMode(QLineEdit::Password);
    ui_password3->setVisible(false);
    ui_password3->setMaximumWidth(width);

    ///////////////////////////////////////////////////////////////////////

    QCheckBox *ui_setDesscription = new QCheckBox(tr("Description"));
    ui_setDesscription->setChecked(false);

    ui_description = new QTextEdit;
    ui_description->setMinimumWidth(300);
    ui_description->setVisible(false);
    connect(ui_setDesscription, SIGNAL(toggled(bool)), ui_description, SLOT(setVisible(bool)));

    //////////////////////////////////////////////////////////////////////

    layout->addWidget(ui_typeBox);

    addLine(layout, 300);

    gl = new QGridLayout;
    gl->setContentsMargins(20, 1, 5, 1);
    gl->addWidget(new QLabel(tr("Name:")), 0, 0, Qt::AlignRight);
    gl->addWidget(ui_name, 0, 1, Qt::AlignRight);
    layout->addLayout(gl, 2, 0, Qt::AlignRight | Qt::AlignTop);
    addLine(layout);

    gl = new QGridLayout;
    gl->setContentsMargins(20, 1, 5, 1);
    gl->addWidget(ui_l_bindAccount, 0, 0, Qt::AlignRight);
    gl->addWidget(ui_bindAccount, 0, 1);
    gl->addWidget(ui_l_balance, 1, 0, Qt::AlignRight);
    gl->addWidget(ui_balance, 1, 1);
    gl->addWidget(ui_l_rate, 2, 0, Qt::AlignRight);
    gl->addWidget(ui_rate, 2, 1);
    gl->addWidget(ui_l_rateType, 3, 0, Qt::AlignRight);
    gl->addWidget(ui_rateType, 3, 1);
    gl->addWidget(ui_l_currency, 4, 0, Qt::AlignRight);
    gl->addWidget(ui_currency, 4, 1);
    layout->addLayout(gl, 4, 0, Qt::AlignRight | Qt::AlignTop);
    addLine(layout);

    gl = new QGridLayout;
    gl->setContentsMargins(20, 1, 5, 1);
    gl->addWidget(ui_l_password, 0, 0, Qt::AlignRight);
    gl->addWidget(ui_password, 0, 1);
    gl->addWidget(ui_l_password2, 1, 0, Qt::AlignRight);
    gl->addWidget(ui_password2, 1, 1);
    gl->addWidget(ui_l_password3, 2, 0, Qt::AlignRight);
    gl->addWidget(ui_password3, 2, 1);
    layout->addLayout(gl, 6, 0, Qt::AlignRight | Qt::AlignTop);
    addLine(layout);

    layout->addWidget(ui_setDesscription, 8, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(ui_description, 9, 0, Qt::AlignCenter | Qt::AlignTop);
}

void AccountDlg::_data()
{
    /////////////////////////////////////////////////////////////////
    ui_rateType->clear();
    ui_rateType->addItem(
                Account::rateTypeText(Account::RATE_TYPE_MONTHLY),
                Account::RATE_TYPE_MONTHLY
    );
    ui_rateType->addItem(
                Account::rateTypeText(Account::RATE_TYPE_YEARLY),
                Account::RATE_TYPE_YEARLY
    );

    ui_currency->clear();

    Currency *c = new Currency(m_account->m_db);
    c->condition()->where = "is_removed = 0";

    Models models = c->items();
    delete c;
    while(!models.isEmpty()) {
        c = dynamic_cast<Currency*>(models.takeFirst());
        ui_currency->addItem(c->name(), c->id());
        delete c;
    }

    ui_bindAccount->clear();

    m_account->condition()->where = "status = 0 AND parent_id = 0";
    models = m_account->items();
    while(!models.isEmpty()) {
        Account *a = dynamic_cast<Account*>(models.takeFirst());
        ui_bindAccount->addItem(a->name()+" #"+QString::number(a->id()), a->id());
        delete a;
    }

    ui_rate->setSuffix("%");
    ui_password->setPlaceholderText(tr("Not required"));
    /////////////////////////////////////////////////////////////////

    switch (m_account->type()) {
    case Account::TYPE_ACCOUNT :
        ui_chTypeAccount->setChecked(true);
        break;
    case Account::TYPE_CREDIT :
        ui_chTypeCredit->setChecked(true);
        break;
    case Account::TYPE_INVESTMENT:
        ui_chTypeInvestment->setChecked(true);
        break;
    default:
        throw "Incorrect type account";
    }

    ui_name->setText(m_account->name());
    ui_bindAccount->setCurrentIndex(0);
    ui_rate->setValue(0.0);
    ui_rateType->setCurrentIndex(0);

    int index = ui_currency->findData(m_account->currency()->id());
    if(index != -1) {
        ui_currency->setCurrentIndex(index);
    }

    ui_description->setText(m_account->description());

    slot_typeAccount();

    if(m_account->id() > 0) {
        setEditMode();
        ui_balance->setText(m_account->displayBalance());
    }
}

void AccountDlg::setEditMode()
{
    ui_typeBox->setDisabled(true);
    ui_l_balance->setDisabled(true);
    ui_balance->setDisabled(true);
    ui_currency->setDisabled(true);

    ui_bindAccount->setDisabled(true);
    ui_rate->setDisabled(true);
    ui_rateType->setDisabled(true);
}

