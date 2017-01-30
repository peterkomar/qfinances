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

#ifndef CATEGORYITEM_H
#define CATEGORYITEM_H

#include <QList>
#include <QVariant>

class CategoryItem
{
public:
    CategoryItem(int id, const QString& name, CategoryItem *parentItem = 0);
    ~CategoryItem();

    int id() { return i_id; }    
    void appendChild(CategoryItem *item);
    CategoryItem *child(int row);
    CategoryItem *childById(int categoryId);
    int childCount() const;
    QVariant data(bool path = false) const;
    int row() const;
    CategoryItem *parentItem();

private:
    QList<CategoryItem*> m_childItems;
    QString m_text;
    QString m_path;
    int i_id;
    CategoryItem *m_parentItem;
};

#endif // CATEGORYITEM_H
