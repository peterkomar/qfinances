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
#include "account.h"
#include "../currency/currency.h"

//TEST
#include <QTime>

TransactionsCharts::TransactionsCharts(QWidget *parent)
    : QWidget(parent)
    ,m_account(nullptr)
    ,m_viewGeneral(nullptr)
    ,m_viewIncomesCategories(nullptr)
    ,m_viewExpensesCategories(nullptr)
{
    QGridLayout *baseLayout = new QGridLayout();

    m_viewGeneral = new QChartView();
    m_viewGeneral->setRenderHint(QPainter::Antialiasing, true);
    baseLayout->addWidget(m_viewGeneral, 0, 0, 1, 3);

    m_viewIncomesCategories = new QChartView();
    m_viewIncomesCategories->setRenderHint(QPainter::Antialiasing, true);
    baseLayout->addWidget(m_viewIncomesCategories, 1, 0);

    m_viewExpensesCategories = new QChartView();
    m_viewExpensesCategories->setRenderHint(QPainter::Antialiasing, true);
    baseLayout->addWidget(m_viewExpensesCategories, 1, 1);

    setLayout(baseLayout);
}

TransactionsCharts::~TransactionsCharts()
{
    m_account = nullptr;
    delete m_viewExpensesCategories;
    delete m_viewGeneral;
    delete m_viewIncomesCategories;    
}

QChart* TransactionsCharts::createPieChart(const QString& title) const
{
    QChart *chart = new QChart();
    chart->setTitle(title);
    chart->setTheme(QChart::ChartThemeLight);
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    return chart;
}

void TransactionsCharts::setGeneralData(qreal incomes, qreal expenses)
{
    QChart *oldChart = m_viewGeneral->chart();
    QChart *newChart = createPieChart(tr("General"));

    QPieSeries *allSeries = new QPieSeries(this->parentWidget());
    allSeries->setName("All operations");

    QPieSlice *incomesSlice = new QPieSlice();
    incomesSlice->setLabel(tr("Incomes (%1)")
                           .arg(m_account->currency()->displayPrice(incomes)));
    incomesSlice->setValue(incomes);
    incomesSlice->setColor(QColor("darkGreen"));
    incomesSlice->setExploded(true);

    QPieSlice *expensesSlice = new QPieSlice();
    expensesSlice->setLabel(tr("Expenses (%1)")
                            .arg(m_account->currency()->displayPrice(expenses)));
    expensesSlice->setValue(expenses);
    expensesSlice->setColor(QColor("darkRed"));


    *allSeries << incomesSlice;
    *allSeries <<  expensesSlice;
    newChart->addSeries(allSeries);
    m_viewGeneral->setChart(newChart);
    m_viewGeneral->resetCachedContent();
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
}

void TransactionsCharts::setCategoriesData(const QString& title, CategoriesValues data)
{
    QChartView *view = m_viewExpensesCategories;
    QString name = tr("Expenses");
    if (title == tr("Incomes Categories")) {
        view = m_viewIncomesCategories;        
        name = tr("Incomes");
    }

    QChart *oldChart = view->chart();
    QChart *newChart = createPieChart(title);

    QPieSeries *allSeries = new QPieSeries(this->parentWidget());
    allSeries->setName(name);

    QMapIterator<QString, double> i(data);
    while (i.hasNext()) {
        i.next();
        *allSeries << new QPieSlice(QString("%1 (%2)")
                                    .arg(i.key())
                                    .arg(m_account->currency()->displayPrice(i.value())), i.value());
    }

    newChart->addSeries(allSeries);
    view->setChart(newChart);
    view->resetCachedContent();
    if (oldChart) {
        delete oldChart;
        oldChart = nullptr;
    }
    view = nullptr;
}

