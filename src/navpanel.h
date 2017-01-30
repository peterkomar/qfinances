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

#ifndef NAVPANEL_H
#define NAVPANEL_H

#include <QWidget>
#include <QMap>
#include <QLayout>

class QVBoxLayout;
class QLabel;

class NavPanelItem : public QWidget {
  Q_OBJECT
public:
  NavPanelItem (const QPixmap& icon, const QString& title, int widgetIndex, QWidget *parent = 0);
  ~NavPanelItem();
  void select();
  void unselect();
  void setTextColor(const QColor& color);

  int widgetIndex() const { return i_widgetIndex; }

signals:
  void click(NavPanelItem *item);

protected:
  void mouseReleaseEvent (QMouseEvent *event);

private:
  bool b_isSelected;
  int i_widgetIndex;

  QLabel *labelIcon;
  QLabel *labelText;
};

class NavPanelItemGroup : public QWidget {
  Q_OBJECT
public:
  NavPanelItemGroup (const QString& title, int widgetIndex = 0, QWidget *parent = 0);
  ~NavPanelItemGroup();

  NavPanelItem* addItem(const QPixmap& icon, const QString& title, int widgetIndex = 0);

signals:
  void clickItem(NavPanelItemGroup* groupItem, NavPanelItem* item);

public slots:
  void slotItemClick(NavPanelItem* item);

private:
  QVBoxLayout *pvLayout;
  int i_widgetIndex;
};

class NavPanel : public QWidget
{
  Q_OBJECT
public:
  NavPanel(QWidget *parent = 0, QLayout::SizeConstraint sizeConstraint = QLayout::SetDefaultConstraint);
  ~NavPanel();
  NavPanelItemGroup* addItemGroup(const QString& title, int widgetIndex = 0);
  void unseletCurrentItem();

signals:
  void clickItem(NavPanelItem* item);

public slots:
  void slotItemClick(NavPanelItemGroup* groupItem, NavPanelItem* item);

protected:
  void paintEvent (QPaintEvent *event);

private:
  void _sortItems();

  QVBoxLayout *d;
  NavPanelItem* selectedItem;
  NavPanelItemGroup* selectedItemGroup;
  QMap<QString, NavPanelItemGroup*>  m_listItems;
};

#endif // NAVPANEL_H
