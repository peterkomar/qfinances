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

#ifndef ACCOUNTDLG_H
#define ACCOUNTDLG_H

#include "dialog.h"

class Account;
class QRadioButton;
class QLineEdit;
class QLabel;
class QComboBox;
class QTextEdit;
class QDoubleSpinBox;
class QGroupBox;

class AccountDlg : public Dialog
{
    Q_OBJECT
public:
    AccountDlg(Account *account, QWidget *parent = 0);
    ~AccountDlg();

    static bool auth(Account *account, QWidget *parent);

protected slots:
    void slot_save();
    void slot_typeAccount();
    void slot_editPassword(const QString& );
    void slot_changeAccount();

private:
    Account *m_account;

    QGroupBox *ui_typeBox;
    QRadioButton *ui_chTypeAccount;
    QRadioButton *ui_chTypeCredit;
    QRadioButton *ui_chTypeInvestment;
    QLineEdit *ui_name;
    QLabel *ui_l_bindAccount;
    QComboBox *ui_bindAccount;
    QLabel *ui_l_balance;
    QLineEdit *ui_balance;
    QLabel *ui_l_rate;
    QDoubleSpinBox *ui_rate;
    QLabel *ui_l_rateType;
    QComboBox *ui_rateType;
    QLabel *ui_l_currency;
    QComboBox *ui_currency;
    QLabel *ui_l_password;
    QLineEdit *ui_password;
    QLabel *ui_l_password2;
    QLineEdit *ui_password2;
    QLabel *ui_l_password3;
    QLineEdit *ui_password3;
    QTextEdit *ui_description;

    void _gui(QGridLayout *layout);
    void _data();

    void setEditMode();
};

#endif // ACCOUNTDLG_H
