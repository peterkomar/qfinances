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
#include "filterdlg.h"
#include "filter.h"
#include "treecombobox.h"
#include "database.h"
#include "project.h"
#include "../category/category.h"
#include "../category/categorytreemodel.h"

#include <QCheckBox>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStringList>
#include <QDoubleValidator>
#include <QLabel>

FilterDlg::FilterDlg(Filter *filter, DataBase *db)
{
    m_filter = filter;
    m_db = db;
    setupUI();
    data();
    setWindowTitle(tr("Custom Filter"));
}

FilterDlg::~FilterDlg()
{
    m_db = 0;
    m_filter = 0;
}

void FilterDlg::slotFilter()
{
    m_filter->reset();
    if( ui_dateFilter->isChecked() ) {
        m_filter->setDateFrom(ui_dateTimeFrom->dateTime());
        m_filter->setDateTo(ui_dateTimeTo->dateTime());
    }

    if( ui_categoryFilter->isChecked() && ui_categoryCombo->currentData().toInt() >= 0) {
        m_filter->setCategoryId(ui_categoryCombo->currentData().toInt());
    }

    if( ui_valueFilter->isChecked() ) {
        m_filter->setVal1Condition(ui_value1->text(), ui_value1Mod->currentText());
        if (ui_value2Mod->isEnabled() && ui_value2Mod->currentText() != "" ) {
            m_filter->setVal2Condition(ui_value2->text(), ui_value2Mod->currentText());
         }
     }

     if( ui_commetFilter->isChecked() ) {
         m_filter->setComment(ui_comment->text());
     }

    emit filter();
    accept();
}

void FilterDlg::slotValue1Mod(const QString &text)
{
    QStringList operators;
    if (text == "<") {
        operators << "" << ">";
    } else if (text == ">") {
        operators << "" << "<";
    } else {
        ui_value2->setVisible(false);
        ui_value2Mod->setVisible(false);
    }

    if (!operators.isEmpty()) {
        ui_value2->setVisible(true);
        ui_value2Mod->setVisible(true);
        ui_value2Mod->clear();
        ui_value2Mod->insertItems(0, operators);
    }
}

void FilterDlg::slotValue2Mod(const QString& text)
{
    if (text.isEmpty()) {
        ui_value2->setEnabled(false);
    } else {
        ui_value2->setEnabled(true);
    }
}

void FilterDlg::setupUI()
{
    QGridLayout *dialogLayout = new QGridLayout;

    setupDateFilter(dialogLayout);
    setupValueFilter(dialogLayout);
    setupCategoryFilter(dialogLayout);
    setupCommentFilter(dialogLayout);

    QPushButton *filterBtn = new QPushButton(tr("Filter"));
    filterBtn->setDefault(true);
    QPushButton *cancelBtn = new QPushButton(tr("Cancel"));

    QHBoxLayout *lButtons = new QHBoxLayout;
    lButtons->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    lButtons->addWidget(cancelBtn);
    lButtons->addWidget(filterBtn);

    //Slots
    connect(filterBtn, &QPushButton::clicked, this, &FilterDlg::slotFilter);
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));

    dialogLayout->addLayout(lButtons, dialogLayout->rowCount(), 0, 1, 2, Qt::AlignRight | Qt::AlignTop);
    dialogLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(dialogLayout);
}

void FilterDlg::setupDateFilter(QGridLayout *layout)
{
    int row = layout->rowCount();
    ui_dateFilter = new QCheckBox(tr("Date"));
    ui_dateFilter->setChecked(false);
    layout->addWidget(ui_dateFilter, row, 0, 1, 1, Qt::AlignLeft);

    QWidget *widget = new QWidget(this);
    widget->setDisabled(true);

    QGridLayout *gridLayout = new QGridLayout(widget);

    ui_dateTimeFrom = new QDateTimeEdit;
    ui_dateTimeFrom->setCalendarPopup(true);
    ui_dateTimeTo = new QDateTimeEdit;
    ui_dateTimeTo->setCalendarPopup(true);

    gridLayout->addWidget(new QLabel(tr("From:")), 0, 0, 1, 1);
    gridLayout->addWidget(ui_dateTimeFrom, 0, 1, 1, 1);

    gridLayout->addWidget(new QLabel(tr("To:")), 0, 2, 1, 1);
    gridLayout->addWidget(ui_dateTimeTo, 0, 3, 1, 1);

    layout->addWidget(widget, row, 1, 1, 1, Qt::AlignLeft);

    connect(ui_dateFilter, &QCheckBox::toggled, widget, &QWidget::setEnabled);
}

void FilterDlg::setupValueFilter(QGridLayout *layout)
{
    int row = layout->rowCount();

    ui_valueFilter = new QCheckBox(tr("Value"));
    ui_valueFilter->setChecked(false);
    layout->addWidget(ui_valueFilter, row, 0, 1, 1);

    QWidget *widget = new QWidget(this);
    widget->setDisabled(true);

    QGridLayout *gridLayout = new QGridLayout(widget);

    ui_value1 = new QLineEdit;
    ui_value2 = new QLineEdit;
    ui_value1Mod = new QComboBox;
    ui_value1Mod->setEditable(false);
    ui_value2Mod = new QComboBox;
    ui_value2Mod->setEditable(false);

    QDoubleValidator *validator = new QDoubleValidator;
    validator->setDecimals(2);
    ui_value1->setValidator(validator);
    ui_value2->setValidator(validator);

    gridLayout->addWidget(ui_value1Mod, 0, 0, 1, 1);
    gridLayout->addWidget(ui_value1, 0, 1, 1, 1);
    gridLayout->addWidget(ui_value2Mod, 0, 2, 1, 1);
    gridLayout->addWidget(ui_value2, 0, 3, 1, 1);

    layout->addWidget(widget, row, 1, 1, 1);

    connect(ui_valueFilter, &QCheckBox::toggled, widget, &QWidget::setEnabled);
    connect(ui_value1Mod,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(slotValue1Mod(QString)));
    connect(ui_value2Mod,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(slotValue2Mod(QString)));
}

void FilterDlg::setupCategoryFilter(QGridLayout *layout)
{
    int row = layout->rowCount();

    ui_categoryFilter = new QCheckBox(tr("Category"));
    ui_categoryFilter->setChecked(false);
    layout->addWidget(ui_categoryFilter, row, 0, 1, 1);

    QWidget *widget = new QWidget(this);
    widget->setEnabled(false);

    QGridLayout *gridLayout = new QGridLayout(widget);

    ui_categoryCombo = new TreeComboBox(widget);
    gridLayout->addWidget(ui_categoryCombo, 0, 0, 1, 1);

    layout->addWidget(widget, row, 1, 1, 1);

    connect(ui_categoryFilter, &QCheckBox::toggled, widget, &QWidget::setEnabled);
}

void FilterDlg::setupCommentFilter(QGridLayout *layout)
{
    int row = layout->rowCount();

    ui_commetFilter = new QCheckBox(tr("Comment"));
    ui_commetFilter->setChecked(false);
    layout->addWidget(ui_commetFilter, row, 0, 1, 1);

    QWidget *widget = new QWidget(this);
    widget->setEnabled(false);

    QGridLayout *gridLayout = new QGridLayout(widget);

    ui_comment = new QLineEdit(widget);
    gridLayout->addWidget(ui_comment, 0, 0, 1, 1);

    layout->addWidget(widget, row, 1, 1, 1);

    connect(ui_commetFilter, &QCheckBox::toggled, ui_comment, &QWidget::setEnabled);
}

void FilterDlg::data()
{
    ui_value1Mod->insertItems(0, QStringList()
                           << "="
                           << ">"
                           << "<");

    Category *category = new Category(m_db);
    category->condition()->order = "parent_id ASC";
    ui_categoryCombo->setModel(new CategoryTreeModel(category->items()));
    delete category;

    //load data from filter
    //Set datetime
    if( m_filter->dateFrom().isValid() && m_filter->dateTo().isValid() ) {
        ui_dateFilter->setChecked(true);
        ui_dateTimeFrom->setDateTime(m_filter->dateFrom());
        ui_dateTimeTo->setDateTime(m_filter->dateTo());
    } else {
        ui_dateFilter->setChecked(false);
    }

    //Set value params
    if(!m_filter->sign1().isEmpty() && !m_filter->sign1().isEmpty()) {
        ui_value1->setText(m_filter->val1());
        ui_value1Mod->setCurrentText(m_filter->sign1());
        ui_valueFilter->setChecked(true);

        if(!m_filter->sign2().isEmpty() && !m_filter->val2().isEmpty()) {
            ui_value2->setText(m_filter->val2());
            ui_value2Mod->setCurrentText(m_filter->sign2());
            ui_value2Mod->setEnabled(true);
            ui_value2->setEnabled(true);
        } else if(m_filter->sign2() == "=") {
            ui_value2Mod->setEnabled(false);
            ui_value2->setEnabled(false);
        }
    }

    //Set category
    if(m_filter->categoryId() >= 0) {
        ui_categoryCombo->selectItem(m_filter->categoryId());
        ui_categoryFilter->setChecked(true);
    }

    //Set comment
    if(!m_filter->comment().isEmpty()) {
        ui_comment->setText(m_filter->comment());
        ui_commetFilter->setChecked(true);
    }

}
