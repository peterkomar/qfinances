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

#ifndef MODULEWIDGET_H
#define MODULEWIDGET_H

#include <QWidget>
#include "grid.h"
#include "module.h"

class QToolButton;
class QVBoxLayout;

class ModuleWidget : public QWidget
{
    Q_OBJECT
public:
    ModuleWidget(ModuleParams *params);
    ~ModuleWidget();

    enum{ BTN_NEW, BTN_EDIT };

    void gui();
    
signals:
    
public slots:

protected:
    QToolButton* createToolButton(const QString& text, const QString& toolTip, const char* icon);

    virtual void topPanel(QVBoxLayout *layout) = 0;
    virtual void mainPanel(QVBoxLayout *layout) = 0;
    virtual void bottomPanel(QVBoxLayout *layout) = 0;

    Grid *m_grid;

    DataBase *m_db;
    Properties *m_properties;
};

#endif // MODULEWIDGET_H
