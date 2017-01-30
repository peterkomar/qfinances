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
#ifndef FNCACCOUNTWIDGET_H
#define FNCACCOUNTWIDGET_H

#include <QTreeWidget>
#include <QMap>
class NavigationItem;
class NavigationGroup : public QTreeWidgetItem
{
public:
    explicit NavigationGroup(int indexModule, QTreeWidget * parent, int type);
    void addItem(int indexItem, const QPixmap& icon, const QString& text);
    void removeItem(int indexItem);
    int indexModule() { return i_indexModule; }

private:
    int i_indexModule;
    QMap<int, NavigationItem*> m_items;
};

class NavigationItem : public QTreeWidgetItem
{
public:
    explicit NavigationItem(int indexItem, NavigationGroup * parent);
    int indexItem() { return i_indexItem; }

private:
    int i_indexItem;
};

class NavigationWidget : public QTreeWidget
{
    Q_OBJECT
public:
    enum NAvigationTypes {TYPE_GROUP = 2, TYPE_ITEM};
    explicit NavigationWidget(QWidget *parent = 0);
    ~NavigationWidget();

    NavigationGroup* addItemGroup(const QString& name, int indexModule);

signals:

public slots:
};

#endif // FNCACCOUNTWIDGET_H
