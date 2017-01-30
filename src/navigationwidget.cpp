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
#include "navigationwidget.h"
#include "project.h"

/////////////////////////////////////////////////////////////////////////
NavigationItem::NavigationItem(int indexItem, NavigationGroup *parent)
    :QTreeWidgetItem(parent, NavigationWidget::TYPE_ITEM)
    ,i_indexItem(indexItem)
{}

/////////////////////////////////////////////////////////////////////////
NavigationGroup::NavigationGroup(int indexModule, QTreeWidget *parent, int type)
    :QTreeWidgetItem(parent, type)
    ,i_indexModule(indexModule)
{

}

void NavigationGroup::addItem(int indexItem, const QPixmap &icon, const QString &text)
{
    NavigationItem *item = new NavigationItem(indexItem, this);
    QFont font;
    font.setPixelSize(13);

    item->setFont(0, font);
    item->setText(0, text);
    item->setIcon(0, QIcon(icon));
    m_items.insert(indexItem, item);
}

void NavigationGroup::removeItem(int indexItem)
{
    NavigationItem* item = m_items.take(indexItem);
    if (item) {
        delete item;
    }
}


///////////////////////////////////////////////////////////////////////

NavigationWidget::NavigationWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    QPalette palette;
    QBrush brush(parent->palette().background().color());
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Base, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
    QBrush brush1(QColor(232, 231, 230, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
    setPalette(palette);

    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(22, 22));
    setAutoExpandDelay(4);
    setIndentation(12);
    setRootIsDecorated(true);
    setUniformRowHeights(false);
    setHeaderHidden(true);
    setFocusPolicy(Qt::NoFocus);
}

NavigationWidget::~NavigationWidget()
{

}

NavigationGroup* NavigationWidget::addItemGroup(const QString &name, int indexModule)
{
    NavigationGroup *group = new NavigationGroup(indexModule, this, TYPE_GROUP);
    QFont font;
    font.setPixelSize(13);
    font.setBold(true);
    font.setCapitalization(QFont::AllUppercase);
    font.setStyleStrategy(QFont::PreferAntialias);

    group->setFont(0, font);
    group->setText(0, name);
    group->setTextColor(0, QColor(161,161,161));
    group->setExpanded(true);

    return group;
}
