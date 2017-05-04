#include "transactionscharts.h"

#include <QGridLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

#include "../../project.h"

//TEST
#include <QTime>

TransactionsCharts::TransactionsCharts(QWidget *parent)
    : QWidget(parent)
    ,m_viewGeneral(nullptr)
    ,m_viewIncomesCategories(nullptr)
    ,m_viewExpensesCategories(nullptr)
    ,m_viewTransactions(nullptr)
{
    QGridLayout *baseLayout = new QGridLayout();

    m_viewGeneral = new QChartView();
    m_viewGeneral->setRenderHint(QPainter::Antialiasing, true);
    baseLayout->addWidget(m_viewGeneral, 0, 0);

    m_viewIncomesCategories = new QChartView(createIncomesCategoriesChart());
    m_viewIncomesCategories->setRenderHint(QPainter::Antialiasing, true);
    baseLayout->addWidget(m_viewIncomesCategories, 0, 1);

    m_viewExpensesCategories = new QChartView(createExpensesCategoriesChart());
    m_viewExpensesCategories->setRenderHint(QPainter::Antialiasing, true);
    baseLayout->addWidget(m_viewExpensesCategories, 0, 2);

    m_viewTransactions = new QChartView(createTransactionsChart());
    m_viewTransactions->setRenderHint(QPainter::Antialiasing, true);
    baseLayout->addWidget(m_viewTransactions, 1, 0, 1, 3);

    setLayout(baseLayout);
}

TransactionsCharts::~TransactionsCharts()
{
    delete m_viewExpensesCategories;
    delete m_viewGeneral;
    delete m_viewIncomesCategories;
    delete m_viewTransactions;
}

DataTable TransactionsCharts::generateRandomData() const
{
    int listCount = 3;
    int m_valueMax = 0;
    int valueMax = m_valueMax = 10;
    int valueCount = 7;

    DataTable dataTable;

    // set seed for random stuff
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    // generate random data
    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + (qreal)(qrand() % valueMax) / (qreal) valueCount;
            QPointF value((j + (qreal) rand() / (qreal) RAND_MAX) * ((qreal) m_valueMax / (qreal) valueCount),
                          yValue);
            QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }

    return dataTable;
}

QChart* TransactionsCharts::createGeneralChart() const
{
    QChart *chart = new QChart();
    chart->setTitle(tr("General"));
    chart->setTheme(QChart::ChartThemeLight);
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    return chart;
}

void TransactionsCharts::setGeneralData(qreal incomes, qreal expenses)
{
    QChart *oldChart = m_viewGeneral->chart();
    QChart *newChart = createGeneralChart();

    QPieSeries *allSeries = new QPieSeries(this->parentWidget());
    allSeries->setName("All operations");

    *allSeries << new QPieSlice(tr("Incomes"), incomes);
    *allSeries <<  new QPieSlice(tr("Expenses"), expenses);
    newChart->addSeries(allSeries);
    m_viewGeneral->setChart(newChart);
    m_viewGeneral->resetCachedContent();
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
}

QChart* TransactionsCharts::createIncomesCategoriesChart() const
{
    QChart *chart = new QChart();
      chart->setTitle("Bar chart");

      DataTable m_dataTable = generateRandomData();

      QStackedBarSeries *series = new QStackedBarSeries(chart);
      for (int i(0); i < m_dataTable.count(); i++) {
          QBarSet *set = new QBarSet("Bar set " + QString::number(i));
          foreach (Data data, m_dataTable[i])
              *set << data.first.y();
          series->append(set);
      }
      chart->addSeries(series);
      chart->createDefaultAxes();

      return chart;
}

QChart* TransactionsCharts::createExpensesCategoriesChart() const
{
    QChart *chart = new QChart();
      chart->setTitle("Bar chart");

      DataTable m_dataTable = generateRandomData();

      QStackedBarSeries *series = new QStackedBarSeries(chart);
      for (int i(0); i < m_dataTable.count(); i++) {
          QBarSet *set = new QBarSet("Bar set " + QString::number(i));
          foreach (Data data, m_dataTable[i])
              *set << data.first.y();
          series->append(set);
      }
      chart->addSeries(series);
      chart->createDefaultAxes();

      return chart;
}

QChart* TransactionsCharts::createTransactionsChart() const
{
    QChart *chart = new QChart();
      chart->setTitle("Bar chart");

      DataTable m_dataTable = generateRandomData();

      QStackedBarSeries *series = new QStackedBarSeries(chart);
      for (int i(0); i < m_dataTable.count(); i++) {
          QBarSet *set = new QBarSet("Bar set " + QString::number(i));
          foreach (Data data, m_dataTable[i])
              *set << data.first.y();
          series->append(set);
      }
      chart->addSeries(series);
      chart->createDefaultAxes();

      return chart;
}
