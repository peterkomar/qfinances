#include "accountinfo.h"
#include "account.h"
#include "project.h"
#include "../../properties.h"
#include "filter.h"
#include "../../database.h"
#include "../currency/currency.h"
#include "../category/category.h"

AccountInfo::AccountInfo(Account *account, Filter *filter, QObject *parent)
    : QThread(parent)
    ,m_account(account)
    ,m_filter(filter)
{

}

AccountInfo::~AccountInfo()
{
    m_account = nullptr;
    m_filter = nullptr;
    m_properties.clear();
}

PropertiesInfo AccountInfo::getResults()
{
    return m_properties;
}

void AccountInfo::run()
{
    getDetails();
}

void AccountInfo::getDetails()
{
    QMap<QString, QString> account;
    account.insert(tr("Balance"), m_account->displayBalance());

    m_properties.insert(tr("Account info"), account);

    QMap<QString, QString> stats;
    stats.insert(tr("Incomes"), formatValue(getSumTransactions(Transaction::INCOMES)));
    stats.insert(tr("Expenses"), formatValue(getSumTransactions(Transaction::EXPENSES)));
    m_properties.insert(tr("Statistics"), stats);

    m_properties.insert(tr("Expenses by categories"),
                        getTransactionsByCategories(Transaction::EXPENSES));

    m_properties.insert(tr("Incomes by categories"),
                        getTransactionsByCategories(Transaction::INCOMES));

    emit resultReady();
}

QString AccountInfo::formatValue(double value)
{
    return m_account->currency()->displayPrice(value);
}

double AccountInfo::getSumTransactions(int type)
{
    Transaction *tr = new Transaction(m_account->m_db, m_account);

    QString query = ""
            "SELECT sum(tr.value) "
            "FROM "+tr->table()+" AS tr "
            "WHERE "+m_filter->toString("tr") +
            " AND tr.type = :type AND tr.status = 1";
    QSqlQuery *q = m_account->m_db->query();
    q->prepare(query);
    q->bindValue(":type", type);

    try {
        m_account->m_db->exec(q);
    } catch (int ) {
        return 0.0;
    }

    if (!q->next()) {
        return 0.0;
    }

    return q->value(0).toDouble();
}

QMap<QString, QString> AccountInfo::getTransactionsByCategories(int type)
{
    QMap<QString, QString> stats;

    Transaction *tr = new Transaction(m_account->m_db, m_account);
    Category *cat = new Category(m_account->m_db);

    QString query = ""
            "SELECT sum(tr.value), cat.name "
            "FROM "+tr->table()+" AS tr "
            "LEFT JOIN " + cat->table() + " AS cat ON cat.id = tr.category_id "
            "WHERE "+m_filter->toString("tr") +
            " AND tr.type = :type AND tr.status = 1 "
            "GROUP By tr.category_id";
    QSqlQuery *q = m_account->m_db->query();
    q->prepare(query);
    q->bindValue(":type", type);

    try {
        m_account->m_db->exec(q);
    } catch (int ) {
        return stats;
    }

    while (q->next()) {
        QString categoryName = q->value(1).toString();
        if (categoryName.isEmpty()) {
            categoryName = QObject::tr("Uncategorised");
        }
        stats.insert(categoryName, formatValue(q->value(0).toDouble()));
    }

    return stats;
}
