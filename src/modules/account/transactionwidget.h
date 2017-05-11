/***************************************************************************
 *   Copyright (C) 2014 by Peter Komar                                     *
 *   udldevel@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TRANSACTIONWIDGET_H
#define TRANSACTIONWIDGET_H

#include <QWidget>
#include <QMap>

#include "../../modulewidget.h"

class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QToolButton;
class Filter;
class QTreeWidgetItem;
class Account;
class TransactionsView;
class Transaction;
class AccountInfo;
class TransactionsCharts;

class TransactionWidget : public ModuleWidget
{
    Q_OBJECT
public:
    TransactionWidget(ModuleParams *params);
    ~TransactionWidget();
    
signals:
    
public slots:
    void slotFilterCheck();
    void slotCustomFilter();
    void slotDoFilter();
    void slotSetAccount(QTreeWidgetItem* );
    void slotSetProperties();

    void slotNew();
    void slotTransfer();
    void slotRevert();
    void slotView();
    void slotCharts();

protected:
    virtual void topPanel(QVBoxLayout *layout);
    virtual void mainPanel(QVBoxLayout *layout);
    virtual void bottomPanel(QVBoxLayout *layout);

private:
    QPushButton* createFiterButton(const QString& text, int property, const char* group, bool bChecked=false);

    Transaction* getSelectedTransaction();
    void stopAccountInfo();
    void loadAccountInfo();

    QMap<QString, QPushButton*> m_filterPanelGroups;
    Filter *m_filter;
    QStackedWidget *m_widget;
    TransactionsView *m_view;
    Account *m_account;
    AccountInfo *m_thread;
    TransactionsCharts *m_charts;
    QToolButton *viewModeBtn;
};

#endif // TRANSACTIONWIDGET_H
