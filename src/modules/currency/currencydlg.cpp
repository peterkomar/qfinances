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

#include "currencydlg.h"
#include "currency.h"

#include <QMessageBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QFrame>

CurrencyDlg::CurrencyDlg(Currency *currency, QWidget *parent)
    : Dialog(parent)
{
    m_currency = currency;
}

CurrencyDlg::~CurrencyDlg()
{
    m_currency = 0;
}

void CurrencyDlg::slot_save()
{
    if( ui_name->text().isEmpty() ) {
        QMessageBox::warning(this, tr("Wrong value"), tr("Currency name is empty"));
        return;
    }

    if( ui_code->text().isEmpty() ) {
        QMessageBox::warning(this, tr("Wrong value"), tr("Currency code is empty"));
        return;
    }

    if( ui_symbolLeft->text().isEmpty() && ui_symbolRight->text().isEmpty() ) {
        QMessageBox::warning(this, tr("Wrong value"), tr("Left of right symbol of currency must be set"));
        return;
    }

    m_currency->setName(ui_name->text());
    m_currency->setCode(ui_code->text());
    m_currency->setSymbolLeft(ui_symbolLeft->text());
    m_currency->setSymbolRight(ui_symbolRight->text());
    m_currency->setRate(ui_curs->value());
    saveData(m_currency);
}

void CurrencyDlg::_gui(QGridLayout *layout)
{
    ui_name = new QLineEdit();
    ui_name->setMinimumWidth(200);

    ui_code = new QLineEdit();
    ui_code->setMaxLength(3);
    ui_code->setMaximumWidth(50);

    ui_symbolLeft = new QLineEdit();
    ui_symbolLeft->setMaxLength(1);
    ui_symbolLeft->setMaximumWidth(50);

    ui_symbolRight = new QLineEdit();
    ui_symbolRight->setMaxLength(1);
    ui_symbolRight->setMaximumWidth(50);

    ui_curs = new QDoubleSpinBox();
    ui_curs->setMinimumWidth(150);
    ui_curs->setMaximum(500.99);
    ui_curs->setMinimum(0.0);
    ui_curs->setValue(1.0);

    QFormLayout *fieldsLayout = getFormLayout();
    fieldsLayout->addRow(tr("Name:"), ui_name);
    layout->addLayout(fieldsLayout, 0, 0, Qt::AlignCenter);
    addLine(layout, 300);

    fieldsLayout = getFormLayout();
    fieldsLayout->addRow(tr("Code:"), ui_code);
    fieldsLayout->addRow(tr("Symbol Left:"), ui_symbolLeft);
    fieldsLayout->addRow(tr("Symbol Right:"), ui_symbolRight);
    layout->addLayout(fieldsLayout, 2, 0, 1, 1, Qt::AlignRight);

    addLine(layout, 300);

    fieldsLayout = getFormLayout();
    fieldsLayout->addRow(tr("Curs:"), ui_curs);
    layout->addLayout(fieldsLayout, 5, 0, 1, 1, Qt::AlignRight);
}

void CurrencyDlg::_data()
{
    ui_name->setText(m_currency->name());
    ui_code->setText(m_currency->code());
    ui_symbolLeft->setText(m_currency->symbolLeft());
    ui_symbolRight->setText(m_currency->symbolRight());
    ui_curs->setValue(m_currency->rate());
}

