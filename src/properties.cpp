#include "properties.h"

#include <QPainter>
#include <QApplication>
#include <QHeaderView>

Properties::Properties(QWidget *parent)
    : QTreeWidget(parent)
{
    setSelectionMode(QAbstractItemView::NoSelection);
    setAllColumnsShowFocus(true);
    setUniformRowHeights(true);
    setWordWrap(false);
    setAlternatingRowColors(true);

    header()->setSectionsMovable(false);
    header()->setSectionResizeMode(QHeaderView::Interactive);

    QStringList headers;
    headers << tr("name") << tr("value");
    setHeaderLabels(headers);  
}

QTreeWidgetItem * Properties::addGroup(const QString& name)
{
    QTreeWidgetItem *group = new QTreeWidgetItem(this, Properties::Header);
    group->setText(0, name);
    group->setFirstColumnSpanned(1);    
    return group;
}

void Properties::addProperty(const QString& name, const QString& value, QTreeWidgetItem *group)
{
    QTreeWidgetItem *property;
    if (group == nullptr) {
        property = new QTreeWidgetItem(this, Properties::Item);
    } else {
        property = new QTreeWidgetItem(group, Properties::Item);
    }

    property->setText(0, name);
    property->setText(1, value);
}

void Properties::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    QTreeWidgetItem *item = itemFromIndex(index);
    if (item->type() == Properties::Header) {
        const QColor c = option.palette.color(QPalette::Dark);
        painter->fillRect(option.rect, c);
        opt.palette.setColor(QPalette::AlternateBase, c);

        QFont font = option.font;
        font.setBold(true);
        opt.font = font;
        opt.showDecorationSelected = false;
    }

    QTreeWidget::drawRow(painter, opt, index);
    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
    if(item->type() != Properties::Header) {
        int cs=0;//draw vertical lines
        for(int i = 0; i<columnCount(); i++) {
            cs += columnWidth(i);
            painter->drawLine(opt.rect.x()+cs,opt.rect.top(), opt.rect.x()+cs, opt.rect.bottom());
        }
    }
    painter->restore();
}
