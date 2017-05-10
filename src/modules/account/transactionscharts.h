#ifndef TRANSACTIONSCHARTS_H
#define TRANSACTIONSCHARTS_H

#include <QWidget>
#include <QChartGlobal>

#include "accountinfo.h"

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class Account;

class TransactionsCharts : public QWidget
{
    Q_OBJECT
public:
    TransactionsCharts(QWidget *parent = 0);
    ~TransactionsCharts();

    void setGeneralData(qreal incomes, qreal expenses);
    void setCategoriesData(const QString& title, CategoriesValues data);
    void setAccount(Account* accont) { m_account = accont; }

signals:

public slots:

private:
    Account *m_account;
    QChartView *m_viewGeneral;
    QChartView *m_viewIncomesCategories;
    QChartView *m_viewExpensesCategories;

    QChart *createPieChart(const QString& title) const;
};

#endif // TRANSACTIONSCHARTS_H
