#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <QThread>
#include <QMap>

class Account;
class Filter;

typedef QMap <QString, QMap<QString, QString>> PropertiesInfo;

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
    void getDetails();

    double getSumTransactions(int type);
    QMap<QString, QString> getTransactionsByCategories(int type);

    QString formatValue(double value);

    Account* m_account;
    Filter* m_filter;

    PropertiesInfo m_properties;
};

#endif // ACCOUNTINFO_H
