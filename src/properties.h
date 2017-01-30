#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QTreeWidget>

class Properties : public QTreeWidget
{
    Q_OBJECT
public:
    explicit Properties(QWidget *parent = 0);
    QTreeWidgetItem * addGroup(const QString& name);
    void addProperty(const QString& name, const QString& value, QTreeWidgetItem *group = nullptr);

signals:

public slots:

protected:
    enum Type {Header=1, Item};

    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;    
};

#endif // PROPERTIES_H
