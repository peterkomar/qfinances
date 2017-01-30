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
#include "categoryitem.h"

CategoryItem::CategoryItem(int id, const QString& name, CategoryItem *parentItem)
{
    i_id = id;
    m_text = name;
    m_path = name;
    m_parentItem = parentItem;
    if (m_parentItem != 0) {
        m_parentItem->appendChild(this);
        if (parentItem->data().toString() != "ROOT") {
            m_path.prepend(parentItem->data().toString() + " > ");
        }
    }
}

CategoryItem::~CategoryItem()
{
    qDeleteAll(m_childItems);
}

void CategoryItem::appendChild(CategoryItem *item)
{
    m_childItems.append(item);
}

CategoryItem *CategoryItem::child(int row)
{
    return m_childItems.value(row);
}

CategoryItem *CategoryItem::childById(int categoryId)
{
    CategoryItem* category = nullptr;
    for (int i = 0; i < m_childItems.size(); ++i) {
        category = const_cast<CategoryItem*>(m_childItems.at(i));
        if (category->id() == categoryId) {
            break;
        } else {
           category = nullptr;
        }
    }
    return category;
}

int CategoryItem::childCount() const
{
    return m_childItems.count();
}

QVariant CategoryItem::data(bool path) const
{
    if (path) {
        return QVariant(m_path);
    }

    return QVariant(m_text);
}

CategoryItem *CategoryItem::parentItem()
{
    return m_parentItem;
}

int CategoryItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<CategoryItem*>(this));
    return 0;
}

