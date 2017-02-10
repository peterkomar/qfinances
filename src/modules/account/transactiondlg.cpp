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
#include "transactiondlg.h"
#include "project.h"
#include "filter.h"
#include "../category/category.h"
#include "../category/categorytreemodel.h"
#include "account.h"
#include "treecombobox.h"

#include <QRadioButton>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QLocale>

#include "account.h"

TransactionDlg::TransactionDlg(Transaction *transaction, QWidget *parent, bool readOnly)
    :Dialog(parent, readOnly)
{    
    m_transaction = transaction;
    m_prevDescription = "";
}

TransactionDlg::~TransactionDlg()
{
    m_transaction = nullptr;
}

void TransactionDlg::slot_save()
{
    Transaction::Type type = Transaction::EXPENSES;
    if(m_uiIncomes->isChecked()) {
        type = Transaction::INCOMES;
    }
    double amount = QLocale::system().toDouble(m_uiAmount->text());
    if (!amount) {
        QMessageBox::warning(this, tr("Missed param"), tr("Please set ammount of transaction"));
        return;
    }

    m_transaction->setDate(m_uiDate->dateTime());
    m_transaction->setValue(amount);
    m_transaction->setCategoryId(m_uiCategory->currentData().toInt());
    m_transaction->setDescription(m_uiComment->toPlainText());
    m_transaction->setType(type);
    accept();
}

void TransactionDlg::_gui(QGridLayout *layout)
{
    m_uiExpences = new QRadioButton(tr("Expense"));
    m_uiIncomes = new QRadioButton(tr("Income"));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(m_uiExpences);
    hlayout->addWidget(m_uiIncomes);

    m_uiDate = new QDateTimeEdit();
    m_uiDate->setMinimumWidth(150);
    m_uiDate->setCalendarPopup(true);

    m_uiCategory = new TreeComboBox();
    m_uiCategory->setMinimumWidth(150);
    connect(m_uiCategory, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_categoryChanged()));

    m_uiAmount = new QLineEdit();
    QDoubleValidator *v = new QDoubleValidator;
    v->setDecimals(2);
    m_uiAmount->setValidator(v);
    m_uiAmount->setPlaceholderText(m_transaction->displayValue().remove("+"));

    m_uiComment = new QTextEdit();
    m_uiComment->setMinimumWidth(300);


    QGridLayout *gl = new QGridLayout;
    gl->setContentsMargins(20, 1, 5, 1);
    gl->addWidget(new QLabel(tr("Type:")), 0, 0, Qt::AlignRight);
    gl->addLayout(hlayout, 0, 1, Qt::AlignRight);
    gl->addWidget(new QLabel(tr("Category:")), 1, 0, Qt::AlignRight);
    gl->addWidget(m_uiCategory, 1, 1, Qt::AlignRight);
    layout->addLayout(gl, 0, 0, Qt::AlignRight | Qt::AlignTop);
    addLine(layout, 300);

    gl = new QGridLayout;
    gl->setContentsMargins(20, 1, 5, 1);
    gl->addWidget(new QLabel(tr("Date:")), 0, 0, Qt::AlignRight);
    gl->addWidget(m_uiDate, 0, 1, Qt::AlignRight);
    gl->addWidget(new QLabel(tr("Amount:")), 1, 0, Qt::AlignRight);
    gl->addWidget(m_uiAmount, 1, 1, Qt::AlignRight);
    layout->addLayout(gl, 2, 0, Qt::AlignRight | Qt::AlignTop);
    addLine(layout, 300);

    layout->addWidget(m_uiComment, 4, 0, Qt::AlignCenter | Qt::AlignTop);
}

void TransactionDlg::_data()
{
    if (!isReadOnly) {
        m_uiExpences->setChecked(true);
    }

    QString title = isReadOnly? tr("View Transaction") : tr("New Transaction");
    setWindowTitle(title);

    m_uiComment->setPlaceholderText(tr("Comments"));

    m_uiDate->setDisplayFormat("yyyy-MM-dd hh:mm");
    m_uiDate->setDateTime(QDateTime::currentDateTime());

    //load categories
    Category *category = new Category(m_transaction->m_db);
    category->condition()->order = "parent_id ASC";
    category->condition()->where = "status = 1";
    m_uiCategory->setModel(new CategoryTreeModel(category->items()));
    delete category;
}

void TransactionDlg::_readOnly()
{
    m_uiAmount->setText(m_transaction->displayValue());
    m_uiCategory->selectItem(m_transaction->categoryId());
    m_uiComment->setText(m_transaction->description());
    m_uiDate->setDateTime(m_transaction->date());
    if (m_transaction->type() == Transaction::INCOMES) {
        m_uiIncomes->setChecked(true);
    } else {
        m_uiExpences->setChecked(true);
    }
}

void TransactionDlg::slot_categoryChanged()
{
    int id_category = m_uiCategory->currentData().toInt();
    if (!id_category) {
        return;
    }

    Category *category = new Category(m_transaction->m_db, id_category);
    category->load();

    double value = category->value();
    if (value > 0 && m_uiAmount->text().isEmpty()) {
        m_uiAmount->setText(QString::number(value));
    }

    QString comment = m_uiComment->toPlainText();
    if (comment.isEmpty() || comment == m_prevDescription) {
        m_uiComment->setText(category->description());
    }
    m_prevDescription = category->description();

    if (category->type() == Transaction::INCOMES) {
        m_uiIncomes->setChecked(true);
    } else {
        m_uiExpences->setChecked(true);
    }

    delete category;
}
