/***************************************************************************
 *   Copyright (C) 2016 by Peter Komar                                     *
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
#ifndef FILTERDLG_H
#define FILTERDLG_H

#include <QDialog>

class Filter;
class TreeComboBox;
class QCheckBox;
class QDateTimeEdit;
class QComboBox;
class QLineEdit;
class QGridLayout;
class DataBase;

class FilterDlg : public QDialog
{
    Q_OBJECT
public:
    FilterDlg(Filter *filter, DataBase *db);
    ~FilterDlg();

public slots:
    void slotFilter();
    void slotValue1Mod(const QString& text);
    void slotValue2Mod(const QString& text);

signals:
    void filter();

private:
    Filter *m_filter;
    DataBase *m_db;

    QCheckBox *ui_dateFilter;
    QDateTimeEdit *ui_dateTimeFrom;
    QDateTimeEdit *ui_dateTimeTo;

    QCheckBox *ui_valueFilter;
    QComboBox *ui_value1Mod;
    QLineEdit *ui_value1;
    QComboBox *ui_value2Mod;
    QLineEdit *ui_value2;

    QCheckBox *ui_categoryFilter;
    TreeComboBox *ui_categoryCombo;

    QCheckBox *ui_commetFilter;
    QLineEdit *ui_comment;

    void setupUI();
    void setupDateFilter(QGridLayout *);
    void setupValueFilter(QGridLayout *);
    void setupCategoryFilter(QGridLayout *);
    void setupCommentFilter(QGridLayout *);

    void data();
};

#endif // FILTERDLG_H
