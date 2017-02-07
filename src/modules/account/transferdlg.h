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
#ifndef TRANSFERDLG_H
#define TRANSFERDLG_H

#include "dialog.h"
#include "database.h"
#include "transaction.h"

class QRadioButton;
class QDateTimeEdit;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QLabel;

class TransferDlg : public Dialog
{
    Q_OBJECT
public:
    TransferDlg(Transaction *transaction, QWidget *parent = 0);
    ~TransferDlg();

    int getRelatedAccountId();
    double getRate();

protected slots:
    void slot_save();
    void slotAccountChanged();

private:
    QRadioButton *m_uiFrom;
    QRadioButton *m_uiTo;
    QDateTimeEdit *m_uiDate;
    QLineEdit *m_uiAmount;
    QLabel *m_uiLabelRate;
    QLineEdit *m_uiRate;
    QTextEdit *m_uiComment;
    QComboBox *m_uiAccount;

    Transaction *m_transaction;

    void _gui(QGridLayout *layout);
    void _data();
};

#endif // TRANSFERDLG_H
