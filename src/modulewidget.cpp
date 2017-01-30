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

#include "modulewidget.h"

#include <QVBoxLayout>
#include <QToolButton>

ModuleWidget::ModuleWidget(ModuleParams *p) :
  QWidget(p->m_mainWidget->parentWidget())
  ,m_grid(0)
{
    m_db = p->m_db;
    m_properties = p->m_properties;
}

ModuleWidget::~ModuleWidget()
{
    if(  m_grid != 0 ) {
        delete m_grid;
    }
}

void ModuleWidget::gui()
{
    QVBoxLayout *pvbxLayout = new QVBoxLayout();

    topPanel(pvbxLayout);
    mainPanel(pvbxLayout);
    bottomPanel(pvbxLayout);

    pvbxLayout->setMargin(0);
    setLayout(pvbxLayout);
}

QToolButton* ModuleWidget::createToolButton(const QString& text, const QString& toolTip, const char* icon)
{
    QToolButton *button = new QToolButton();
    button->setText(text);
    button->setIcon(QIcon(icon));
    button->setIconSize(QSize(48,48));
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setToolTip(toolTip);
    button->setStatusTip(button->toolTip());
    button->setFocusPolicy(Qt::NoFocus);
    button->setAutoRaise(true);

    return button;
}

