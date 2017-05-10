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

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

class TransactionsCharts : public QWidget
{
    Q_OBJECT
public:
    TransactionsCharts(QWidget *parent = 0);
    ~TransactionsCharts();

    void setGeneralData(qreal incomes, qreal expenses);
    void setCategoriesData(const QString& title, CategoriesValues data);

signals:

public slots:

private:
    QChartView *m_viewGeneral;
    QChartView *m_viewIncomesCategories;
    QChartView *m_viewExpensesCategories;
    QChartView *m_viewTransactions;

    //TESTmethod
    DataTable generateRandomData() const;

    QChart *createPieChart(const QString& title) const;
    QChart* createTransactionsChart() const;
};

#endif // TRANSACTIONSCHARTS_H
