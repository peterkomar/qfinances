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
#ifndef TRANSACTIONDLG_H
#define TRANSACTIONDLG_H

#include "dialog.h"
#include "database.h"
#include "transaction.h"

class QLineEdit;
class QDateTimeEdit;
class QTextEdit;
class QLabel;
class Account;
class TreeComboBox;
class QRadioButton;

class TransactionDlg : public Dialog
{
    Q_OBJECT
public:
    TransactionDlg(Transaction *transaction, QWidget *parent = 0, bool readOnly = false);
    ~TransactionDlg();

protected slots:
    void slot_save();
    void slot_categoryChanged();

private:
    QRadioButton *m_uiIncomes;
    QRadioButton *m_uiExpences;
    QDateTimeEdit *m_uiDate;
    TreeComboBox *m_uiCategory;
    QLineEdit *m_uiAmount;
    QTextEdit *m_uiComment;

    Transaction *m_transaction;

    QString m_prevDescription;

    void _gui(QGridLayout *layout);
    void _data();
    void _readOnly();
};

#endif // TRANSACTIONDLG_H
