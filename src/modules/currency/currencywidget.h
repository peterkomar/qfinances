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

#ifndef CURRENCYCONFIG_H
#define CURRENCYCONFIG_H

#include "../../modulewidget.h"

class QToolButton;

class CurrencyWidget : public ModuleWidget
{
    Q_OBJECT
public:
    CurrencyWidget(ModuleParams *params);
    
signals:
    
public slots:
    void slotEditCurrency();
    void slotEnableEditing();
    void slotDeleteCurrency();

protected:
    virtual void topPanel(QVBoxLayout *layout);
    virtual void mainPanel(QVBoxLayout *layout);
    virtual void bottomPanel(QVBoxLayout *layout);

private:
    void _loadData();

    QToolButton *editBtn;
    QToolButton *remBtn;
};

#endif // CURRENCYCONFIG_H
