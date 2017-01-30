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

#include "categorywidget.h"
#include "categorydlg.h"
#include "category.h"
#include "project.h"
#include "../account/filter.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMessageBox>
#include <QTreeWidget>
#include <QHeaderView>

CategoryWidget::CategoryWidget(ModuleParams *params) :
    ModuleWidget(params)
{    
}

void CategoryWidget::topPanel(QVBoxLayout *layout)
{
    Q_UNUSED(layout);
}

void CategoryWidget::mainPanel(QVBoxLayout *layout)
{
    m_treeGrid = new QTreeWidget();
    m_treeGrid->setColumnCount(4);
    m_treeGrid->setAlternatingRowColors(true);
    m_treeGrid->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeGrid->header()->resizeSection(0, 200);
    m_treeGrid->header()->setDefaultAlignment(Qt::AlignCenter);

    QStringList labels;
    labels << tr("Name") << tr("Type") << tr("Tax") << tr("Description");

    m_treeGrid->setHeaderLabels(labels);
    layout->addWidget(m_treeGrid);
    _loadData();

    connect(m_treeGrid, SIGNAL(itemSelectionChanged()), this, SLOT(slotEnableEditing()));
}

void CategoryWidget::bottomPanel(QVBoxLayout *layout)
{
    QHBoxLayout *phbxLayout = new QHBoxLayout();
    phbxLayout->setAlignment(Qt::AlignLeft);
    phbxLayout->setMargin(2);

    QToolButton *newBtn = createToolButton(tr("New"), tr("Add new category"), ":pictures/new.png");
    newBtn->setProperty("type_btn", BTN_NEW);
    connect(newBtn, SIGNAL(clicked()), this, SLOT(slotEdit()));
    phbxLayout->addWidget(newBtn);

    editBtn = createToolButton(tr("Edit"), tr("Edit category"), ":pictures/edit.png");
    editBtn->setDisabled(true);
    editBtn->setProperty("type_btn", BTN_EDIT);
    connect(editBtn, SIGNAL(clicked()), this, SLOT(slotEdit()));
    phbxLayout->addWidget(editBtn);

    phbxLayout->addStretch(0);

    remBtn = createToolButton(tr("Remove"), tr("Remove category"), ":pictures/remove.png");
    remBtn->setDisabled(true);
    connect(remBtn, SIGNAL(clicked()), this, SLOT(slotDelete()));
    phbxLayout->addWidget(remBtn);

    layout->addLayout(phbxLayout);
}


void CategoryWidget::slotEdit()
{
    int id_category = 0;
    if( QToolButton *btn = qobject_cast<QToolButton *>(sender())) {      
        if(btn->property("type_btn") == BTN_EDIT) {
            QTreeWidgetItem *item = m_treeGrid->currentItem();
            id_category = item->data(0, Qt::UserRole).toInt();
        }
    }

    Category *c = new Category(m_db, id_category);
    c->load();
    if( !id_category ) {
        c->setType(0);
        c->setTax(0.0);
    }

    CategoryDlg * dlg = new CategoryDlg(c, this);

    if( id_category ) {
        dlg->setWindowTitle(QString("%1 %2").arg(tr("Edit category")).arg(c->name()));
    } else {
        dlg->setWindowTitle(tr("Create new category"));
    }

    if( QDialog::Accepted == dlg->dialog() ) {
        _loadData();
    }

    delete dlg;
    delete c;
}

void CategoryWidget::slotEnableEditing()
{
    QTreeWidgetItem *item = m_treeGrid->currentItem();
    if (item) {
        editBtn->setEnabled(true);
        remBtn->setEnabled(true);
    } else {
        editBtn->setEnabled(false);
        remBtn->setEnabled(false);
    }
}

void CategoryWidget::slotDelete()
{
    QTreeWidgetItem *item = m_treeGrid->currentItem();
    if (!item) {
        return;
    }

    int id_category = item->data(0, Qt::UserRole).toInt();

    Category *c = new Category(m_db, id_category);
    c->load();

    QString message = tr("Are you shure want  to remove: %1 %2?")
            .arg(c->name())
            .arg(c->hasChildren()? "with children" : "");

    int btn = QMessageBox::question(this, tr("Confirm remove category"), message);
    if( btn == QMessageBox::Yes ) {
        c->remove();
    }
    delete c;
    delete item;
    _loadData();

    editBtn->setDisabled(true);
    remBtn->setDisabled(true);
}

void CategoryWidget::_loadData()
{
    m_treeGrid->clear();
    Category *category = new Category(m_db);
    category->condition()->order = "parent_id ASC, name ASC";
    category->condition()->where = "status = 1";
    Models list = category->items();
    delete category;

    QMap<int, QTreeWidgetItem*> itemsMapping;
    QTreeWidgetItem *item = 0;
    while(!list.isEmpty()) {
       Category *c = dynamic_cast<Category*>(list.takeFirst());

       if (itemsMapping.contains(c->parentId())) {
           item = new QTreeWidgetItem(itemsMapping.value(c->parentId()));
       } else {
           item = new QTreeWidgetItem(m_treeGrid);
           itemsMapping.insert(c->id(), item);
       }

       item->setText(0, c->name());
       item->setData(0, Qt::UserRole, c->id());
       item->setText(1, Filter::typeString(c->type()));
       item->setText(2, QString::number(c->tax()));
       item->setText(3, c->description());
       delete c;
    }
    itemsMapping.clear();
}
