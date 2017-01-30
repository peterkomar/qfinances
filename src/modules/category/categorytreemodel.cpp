#include "categorytreemodel.h"
#include "categoryitem.h"
#include "category.h"
#include "project.h"

CategoryTreeModel::CategoryTreeModel(Models list, QObject *parent)
    : QAbstractItemModel(parent)
    , m_selectedItem(nullptr)
{
    m_rootItem = new CategoryItem(-1, "ROOT");
    loadData(list);
}

CategoryTreeModel::~CategoryTreeModel()
{
    delete m_rootItem;
    m_selectedItem = nullptr;
}

int CategoryTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool CategoryTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value);
    if (role == Qt::DisplayRole) {
        m_selectedItem = static_cast<CategoryItem*>(index.internalPointer());
    }
    return true;
}

QVariant CategoryTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
         CategoryItem *item = static_cast<CategoryItem*>(index.internalPointer());        
         return item->data(item == m_selectedItem);
    }

    if (role == Qt::UserRole) {
        CategoryItem *item = static_cast<CategoryItem*>(index.internalPointer());
        return QVariant(item->id());
    }

    return QVariant();
}

Qt::ItemFlags CategoryTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant CategoryTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

QModelIndex CategoryTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CategoryItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CategoryItem*>(parent.internalPointer());

    CategoryItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex CategoryTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    CategoryItem *childItem = static_cast<CategoryItem*>(index.internalPointer());
    CategoryItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CategoryTreeModel::rowCount(const QModelIndex &parent) const
{
    CategoryItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CategoryItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void CategoryTreeModel::loadData(Models data)
{
    CategoryItem *parent = m_rootItem;
    new CategoryItem(0, tr("Uncategorized"), parent);
    while(!data.isEmpty()) {
       Category *c = dynamic_cast<Category*>(data.takeFirst());
       if (c->parentId() > 0) {
           parent = m_rootItem->childById(c->parentId());
       } else {
           parent = m_rootItem;
       }

       if (parent != nullptr) {
           new CategoryItem(c->id(), c->name(), parent);
       }
       delete c;
    }
}
