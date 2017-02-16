#ifndef TRANSACTIONSCHARTS_H
#define TRANSACTIONSCHARTS_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class TransactionsCharts : public QWidget
{
    Q_OBJECT
public:
    TransactionsCharts(QWidget *parent = 0);

signals:

public slots:

private:
    QChartView *m_viewGeneral;
    QChartView *m_viewIncomesCategories;
    QChartView *m_viewExpensesCategories;
    QChartView *m_viewTransactions;
};

#endif // TRANSACTIONSCHARTS_H
