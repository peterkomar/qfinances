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

#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QStackedWidget>
#include <QDebug>
#include "navpanel.h"
#include "database.h"
#include "navigationwidget.h"
#include "properties.h"

struct ModuleParams {
  NavigationWidget *m_navPanel;//left navigation panel
  QStackedWidget *m_mainWidget;//Main widget which contains modules tables
  DataBase *m_db;//handler to work with database
  NavPanelItemGroup *m_cfg;//Management section
  Properties *m_properties;//Properties panel
};

class Module : public QObject
{
    Q_OBJECT
public:
    Module(ModuleParams *params);
    virtual ~Module();
    virtual void exec();

protected:
  ModuleParams *m_p;

  virtual void _install() = 0;

  void _installData(const QString& name);
};

#endif // MODULE_H
