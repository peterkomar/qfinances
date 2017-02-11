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

#include "categorydlg.h"
#include "category.h"
#include "project.h"
#include "../account/filter.h"

#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QCheckBox>
#include <QTextEdit>
#include <QFrame>
#include <QDoubleValidator>

CategoryDlg::CategoryDlg(Category *category, QWidget *parent) :
  Dialog(parent)
{
    m_category = category;
}

CategoryDlg::~CategoryDlg()
{
    m_category = 0;
}

void CategoryDlg::slot_save()
{
    if( m_uiName->text().isEmpty() ) {
        QMessageBox::warning(this, tr("Wrong value"), tr("Category name empty"));
        m_uiName->setFocus();
        return;
    }

    m_category->setName(m_uiName->text());
    m_category->setDescription(m_uiComment->toPlainText());
    m_category->setTax(m_uiTax->text().toDouble());
    m_category->setValue(m_uiValue->text().toDouble());
    m_category->setType(m_uiType->currentData().toInt());
    m_category->setParentId(m_parentCategories->currentData().toInt());
    saveData(m_category);
}

void CategoryDlg::_gui(QGridLayout *layout)
{    
    //Init GUI
    m_uiName = new QLineEdit();
    m_uiName->setMinimumWidth(200);

    m_parentCategories = new QComboBox;

    m_uiType = new QComboBox();
    m_uiType->setMinimumWidth(200);

    m_uiTax = new QLineEdit();
    m_uiTax->setPlaceholderText(tr("Not required"));

    QDoubleValidator *v = new QDoubleValidator;
    v->setDecimals(2);

    m_uiTax->setValidator(v);

    m_uiValue = new QLineEdit();
    m_uiValue->setPlaceholderText(tr("Not required"));
    m_uiValue->setValidator(v);

    QFormLayout *fieldsLayout = getFormLayout();
    fieldsLayout->setFormAlignment(Qt::AlignCenter | Qt::AlignTop);
    fieldsLayout->addRow(tr("Parent:"), m_parentCategories);
    fieldsLayout->addRow(tr("Name:"), m_uiName);
    addSpacer(fieldsLayout);
    fieldsLayout->addRow(tr("Type:"), m_uiType);
    fieldsLayout->addRow(tr("Tax:"), m_uiTax);
    fieldsLayout->addRow(tr("Value:"), m_uiValue);

    QCheckBox *addComment = new QCheckBox(tr("Description:"));
    addComment->setChecked(false);

    m_uiComment = new QTextEdit;
    m_uiComment->setMinimumWidth(360);
    m_uiComment->setVisible(false);
    connect(addComment, SIGNAL(toggled(bool)), m_uiComment, SLOT(setVisible(bool)));

    layout->addLayout(fieldsLayout, 0, 0, Qt::AlignCenter | Qt::AlignTop);
    addLine(layout, 360);
    layout->addWidget(addComment, 2, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(m_uiComment, 3, 0, Qt::AlignCenter | Qt::AlignTop);
}

void CategoryDlg::_data()
{
    m_uiType->addItem(tr("None"));
    m_uiType->addItem(Filter::typeString(Transaction::INCOMES), Transaction::INCOMES);
    m_uiType->addItem(Filter::typeString(Transaction::EXPENSES), Transaction::EXPENSES);
    m_uiTax->setValidator(new QDoubleValidator(0.0, 1.0, 2, this));

    m_uiName->setText(m_category->name());
    m_uiComment->setText(m_category->description());
    m_uiTax->setText(QString::number(m_category->tax()));
    m_uiValue->setText(QString::number(m_category->value()));

    int index = m_uiType->findData(m_category->type());
    if (index != -1) {
        m_uiType->setCurrentIndex(m_category->type());
    }

    m_category->condition()->where = "parent_id = 0";
    m_category->condition()->order = "name ASC";

    Models list = m_category->items();
    m_parentCategories->addItem("", 0);
    int i = 0;
    while(!list.isEmpty()) {
        Category *c = dynamic_cast<Category*>(list.takeFirst());
        m_parentCategories->insertItem(i, c->name(), c->id());
        if (c->id() == m_category->parentId()) {
            m_parentCategories->setCurrentIndex(i);
        }
        delete c;
        ++i;
    }
}
