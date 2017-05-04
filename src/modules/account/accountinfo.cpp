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
}

PropertiesInfo AccountInfo::getFormatedResults()
{
    PropertiesInfo properties;

    QMap<QString, QString> account;
    account.insert(tr("Balance"), m_account->displayBalance());
    properties.insert(tr("Account info"), account);

    QMap<QString, QString> stats;
    stats.insert(tr("Incomes"), formatValue(m_d.d_incomes));
    stats.insert(tr("Expenses"), formatValue(m_d.d_expenses));
    properties.insert(tr("Statistics"), stats);

    stats.clear();
    QMapIterator<QString, double> i(m_d.m_categoryExpenses);
    while (i.hasNext()) {
        i.next();
        stats.insert(i.key(), formatValue(i.value()));
    }
    properties.insert(tr("Expenses by categories"), stats);

    stats.clear();
    QMapIterator<QString, double> j(m_d.m_categoryIncomes);
    while (j.hasNext()) {
        j.next();
        stats.insert(j.key(), formatValue(j.value()));
    }
    properties.insert(tr("Incomes by categories"), stats);


    return properties;
}

double AccountInfo::getExpenses()
{
    return m_d.d_expenses;
}

double AccountInfo::getIncomes()
{
    return m_d.d_incomes;
}

CategoriesValues AccountInfo::getExpensesByCategories()
{
    return m_d.m_categoryExpenses;
}

CategoriesValues AccountInfo::getIncomesByCategories()
{
    return m_d.m_categoryIncomes;
}

void AccountInfo::run()
{
    collateDetails();
    emit resultReady();
}

void AccountInfo::collateDetails()
{
    m_d.d_incomes = getSumTransactions(Transaction::INCOMES);
    m_d.d_expenses = getSumTransactions(Transaction::EXPENSES);
    m_d.m_categoryExpenses = getTransactionsByCategories(Transaction::EXPENSES);
    m_d.m_categoryIncomes = getTransactionsByCategories(Transaction::INCOMES);
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

QMap<QString, double> AccountInfo::getTransactionsByCategories(int type)
{
    QMap<QString, double> categoriesStats;

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
        return categoriesStats;
    }

    while (q->next()) {
        QString categoryName = q->value(1).toString();
        if (categoryName.isEmpty()) {
            categoryName = QObject::tr("Uncategorised");
        }
        categoriesStats.insert(categoryName, q->value(0).toDouble());
    }

    return categoriesStats;
}
