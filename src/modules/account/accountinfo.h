#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <QThread>
#include <QMap>

class Account;
class Filter;

typedef QMap <QString, QMap<QString, QString>> PropertiesInfo;
typedef QMap<QString, double> CategoriesValues;

class AccountInfo : public QThread
{
    Q_OBJECT
public:
    AccountInfo(Account *account, Filter *filter, QObject *parent = 0);
    ~AccountInfo();

    void run() Q_DECL_OVERRIDE;

    PropertiesInfo getResults();

signals:
      void resultReady();

private:
    void collateDetails();

    double getSumTransactions(int type);
    QMap<QString, double> getTransactionsByCategories(int type);

    QString formatValue(double value);

    Account* m_account;
    Filter* m_filter;

    struct {
        double d_expenses;
        double d_incomes;

        CategoriesValues m_categoryExpenses;
        CategoriesValues m_categoryIncomes;
    } m_d;
};

#endif // ACCOUNTINFO_H
