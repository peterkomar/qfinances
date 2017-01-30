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


#ifndef FINANCES_H
#define FINANCES_H

#include <QtWidgets/QMainWindow>

class QStackedWidget;
class Modules;
class DataBase;
class LoginDialog;
class NavPanel;
class NavPanelItem;
class QTreeWidgetItem;
struct ModuleParams;


class Finances : public QMainWindow
{
  Q_OBJECT
public:
  Finances();
  ~Finances();

   bool login();

private slots:
  void slot_successLogin(LoginDialog *);

  void slotClickBottomPanelItem(NavPanelItem *);
  void slotClickNavPanelItem(QTreeWidgetItem *);

private:
  void _gui();
  ModuleParams *m_d;
  Modules *m_modules;
  bool m_bSuccessLogin;
};

#endif
