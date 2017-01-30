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
#include "treecombobox.h"
#include "project.h"

#include <QHeaderView>
#include <QTreeView>
#include <QStandardItemModel>
#include <QLineEdit>

TreeComboBox::TreeComboBox(QWidget *parent)
: QComboBox(parent), m_treeView(nullptr)
{
    m_treeView = new QTreeView(this);
    m_treeView->setFrameShape(QFrame::NoFrame);
    m_treeView->setEditTriggers(QTreeView::NoEditTriggers);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSelectionBehavior(QTreeView::SelectRows);
    m_treeView->setRootIsDecorated(false);
    m_treeView->setWordWrap(true);
    m_treeView->setAllColumnsShowFocus(true);
    m_treeView->header()->setVisible(false);
    setView(m_treeView);

     connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(slotIndexChanged()));
}

void TreeComboBox::showPopup() {
  setRootModelIndex(QModelIndex());

  for(int i=1;i<model()->columnCount();++i)
    m_treeView->hideColumn(i);

  m_treeView->expandAll();
  m_treeView->setItemsExpandable(false);
  QComboBox::showPopup();
}

void TreeComboBox::selectIndex(const QModelIndex& index) {
  setRootModelIndex(index.parent());
  setCurrentIndex(index.row());
}

void TreeComboBox::selectItem(int id)
{
    QModelIndex index = findItemIndex(id, QModelIndex());
    if (index.isValid()) {
        setRootModelIndex(index.parent());
        setCurrentIndex(index.row());
    }
}

QModelIndex TreeComboBox::findItemIndex(int id, const QModelIndex& index) const
{
    if (index.data(Qt::UserRole) == id) {
        return index;
    } else {
        QStandardItemModel *m_pModel = (QStandardItemModel *)model();
        int childCount = m_pModel->rowCount(index);
        for (int j = 0; j < childCount; ++j) {
            QModelIndex childIndex = m_pModel->index(j, 0, index);
            QModelIndex foundIndex = findItemIndex(id, childIndex);
            if (foundIndex.isValid()) {
                return foundIndex;
            }
        }
    }
    return QModelIndex();
}

void TreeComboBox::slotIndexChanged()
{
    int id = currentData().toInt();
    QModelIndex index = findItemIndex(id, QModelIndex());
    if (index.isValid()) {
       QStandardItemModel *m_pModel = (QStandardItemModel *)model();
       m_pModel->setData(index, QVariant(), Qt::DisplayRole);
    }
}

