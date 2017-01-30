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

#include "navpanel.h"
#include "project.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>

/**************************NavPanelItem**********************************/
NavPanelItem::NavPanelItem(const QPixmap& icon, const QString& title, int widgetIndex, QWidget *parent)
  :QWidget(parent)
  ,b_isSelected(false)
  ,i_widgetIndex(widgetIndex)
{
    setMinimumWidth(250);

    QHBoxLayout *phlayout = new QHBoxLayout;

    labelIcon = new QLabel;
    labelIcon->setPixmap(icon);
    labelText = new QLabel(title);


    // Setup Text Label
    labelText->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    labelIcon->setFixedWidth(22);

    // Setup Font
    QFont font = labelText->font();
    font.setPixelSize(13);
    labelText->setFont(font);

    // Add Items to Layout
    phlayout->addWidget(labelIcon);
    phlayout->addWidget(labelText);

    // Setup Layout
    phlayout->setContentsMargins(1, 1, 1, 1);
    phlayout->insertSpacing(0, 10);
    setLayout(phlayout);
}

NavPanelItem::~NavPanelItem()
{}

void NavPanelItem::select()
{
    QFont font = this->font();
    font.setBold(true);
    setFont(font);
    setTextColor(Qt::white);

    b_isSelected = true;
}

void NavPanelItem::unselect()
{
    QFont font = this->font();
    font.setBold(false);
    setFont(font);
    setTextColor(Qt::black);

    b_isSelected = false;
}

void NavPanelItem::mouseReleaseEvent (QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    // Left Click, Select/Unselect Item
    if ( (event->button() == Qt::LeftButton) && !b_isSelected) {
        emit click(this);
    }
}

void NavPanelItem::setTextColor (const QColor& color)
{
    QPalette palette = labelText->palette();
    palette.setColor(QPalette::WindowText, color);
    labelText->setPalette(palette);
}

/**************************NavPanelItem***********************************/
/*                                                                      */
/**************************NavPanelItemGroup*****************************/
NavPanelItemGroup::NavPanelItemGroup (const QString& title, int widgetIndex, QWidget *parent)
  :QWidget(parent)
  ,i_widgetIndex(widgetIndex)
{

    QLabel *label = new QLabel(title);
    QFont font = label->font();
    font.setBold(true);
    font.setPixelSize(13);
    font.setCapitalization(QFont::AllUppercase);
    label->setFont(font);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, QColor(161,161,161));
    label->setPalette(palette);

    // Setup Layout
    pvLayout = new QVBoxLayout;
    pvLayout->setSpacing(1);
    pvLayout->setContentsMargins(0, 0, 0, 0);
    pvLayout->addWidget(label);
    setLayout(pvLayout);
}

NavPanelItemGroup::~NavPanelItemGroup()
{
    delete pvLayout;
    pvLayout = NULL;
}

NavPanelItem* NavPanelItemGroup::addItem(const QPixmap& icon, const QString& title, int widgetIndex)
{
    if( widgetIndex == 0 ) {
        widgetIndex = i_widgetIndex;
    }

    NavPanelItem* child = new NavPanelItem(icon, title, widgetIndex);
    pvLayout->addWidget(child);

    connect(child, SIGNAL(click(NavPanelItem*)), this, SLOT(slotItemClick(NavPanelItem*)));

    return child;
}

void NavPanelItemGroup::slotItemClick(NavPanelItem *item)
{
    emit clickItem(this, item);
}

/**************************NavPanelItemGroup**********************************/
/*                                                                           */
/**************************NavPanel*****************************************/

NavPanel::NavPanel(QWidget *parent, QLayout::SizeConstraint sizeConstraint) :
  QWidget(parent)
 ,selectedItem(NULL)
 ,selectedItemGroup(NULL)
{

    setMinimumWidth(250);

    d  = new QVBoxLayout;
    d->addStretch(1);
    d->setSizeConstraint(sizeConstraint);
    setLayout(d);
}

NavPanel::~NavPanel()
{
    delete d;
    d = NULL;
}

NavPanelItemGroup* NavPanel::addItemGroup(const QString& title, int widgetIndex)
{
    NavPanelItemGroup* item = new NavPanelItemGroup(title, widgetIndex, this);
    connect(item, SIGNAL(clickItem(NavPanelItemGroup*,NavPanelItem*)), this, SLOT(slotItemClick(NavPanelItemGroup*,NavPanelItem*)));
    d->insertWidget(d->count() - 1, item);

    QString s = title;
    m_listItems.insert(s.replace(" ", "_"), item);

    _sortItems();

    return item;
}

void NavPanel::unseletCurrentItem()
{
    if( 0 != selectedItem ) {
        selectedItem->unselect();
        update();

        selectedItemGroup = NULL;
        selectedItem = NULL;
    }
}

void NavPanel::slotItemClick(NavPanelItemGroup *group, NavPanelItem *item)
{
    if( selectedItem == item ) {
        return;
    }

    if( selectedItem ) {
        selectedItem->unselect();
    }

    selectedItem      = item;
    selectedItemGroup = group;

    selectedItem->select();

    emit clickItem(item);
}

void NavPanel::paintEvent (QPaintEvent *event)
{
    QWidget::paintEvent(event);

    // Recall Update() if painter area is not complete!
    if (event->rect().x() > 0 || event->rect().y() > 0) update();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Draw Selection
    if( !selectedItem) return;

    QPoint pos = selectedItemGroup->pos() + selectedItem->pos();
    int width = geometry().width();

    QColor colorSelection(169, 183, 206);

    int r = colorSelection.red();
    int g = colorSelection.green();
    int b = colorSelection.blue();

    QLinearGradient linearGrad(QPointF(0, pos.y()), QPointF(0, pos.y() + selectedItem->height()));
    linearGrad.setColorAt(0, colorSelection);
    linearGrad.setColorAt(1, QColor(r - 10, g - 10, b - 10));
    p.fillRect(0, pos.y(), width, selectedItem->height(), linearGrad);

    p.end();
}

void NavPanel::_sortItems()
{
    QMapIterator<QString, NavPanelItemGroup*> i(m_listItems);

    while (i.hasNext()) {
        i.next();
        d->removeWidget(i.value());
    }

    i.toFront();
    while (i.hasNext()) {
        i.next();
        d->insertWidget(d->count() - 1, i.value());
    }
}

/**************************NavPanel*****************************************/
