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


#include <QtWidgets/QMessageBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLayout>
#include <QAction>
#include <QMenuBar>
#include <QDateTime>

#include "project.h"
#include "finances.h"
#include "topheader.h"
#include "navpanel.h"
#include "modules.h"
#include "database.h"
#include "logindialog.h"
#include "modulewidget.h"

//////////////////////////////////////////////
Finances::Finances()
  :m_modules(NULL)
  ,m_bSuccessLogin(false)
{  
  setWindowTitle("QFinances");

  m_d = new ModuleParams;
  m_d->m_db = 0;
  m_d->m_mainWidget  = 0;
  m_d->m_navPanel = 0;
  m_d->m_cfg = 0;
  m_d->m_properties = 0;
  _gui();
}

Finances::~Finances()
{
    if( m_modules != NULL ) {
        delete m_modules;
    }

    if( m_d->m_db ) {
        delete m_d->m_db;
        m_d->m_db = 0;
    }

    delete m_d;
}


void Finances::_gui()
{
  //Init transaction list
  m_d->m_mainWidget = new QStackedWidget(this);
  m_d->m_mainWidget->addWidget(new QWidget);//add empty widget by default

  m_d->m_navPanel = new NavigationWidget(this);
  connect(m_d->m_navPanel, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slotClickNavPanelItem(QTreeWidgetItem*)));

  //setup accounts list
  QDockWidget *accountsDock = new QDockWidget(this);
  accountsDock->setObjectName("Left");
  accountsDock->setTitleBarWidget(new QWidget);
  accountsDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  accountsDock->setFloating(false);
  accountsDock->setWidget(m_d->m_navPanel);
  addDockWidget(Qt::LeftDockWidgetArea, accountsDock);


  m_d->m_properties = new Properties(this);
  QDockWidget *propertiesDock = new QDockWidget(this);
  propertiesDock->setObjectName("Left");
  propertiesDock->setTitleBarWidget(new QWidget);
  propertiesDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  propertiesDock->setFloating(false);
  propertiesDock->setWidget(m_d->m_properties);
  addDockWidget(Qt::LeftDockWidgetArea, propertiesDock);

  NavPanel *management = new NavPanel(this, QLayout::SetFixedSize);
  connect(management, SIGNAL(clickItem(NavPanelItem*)), this, SLOT(slotClickBottomPanelItem(NavPanelItem*)));
  m_d->m_cfg = management->addItemGroup(tr("Management"));

  QDockWidget *dockCfg = new QDockWidget(this);
  dockCfg->setObjectName("Left_Bottom");
  dockCfg->setTitleBarWidget(new QWidget);
  dockCfg->setFeatures(QDockWidget::NoDockWidgetFeatures);
  dockCfg->setFloating(false);
  dockCfg->setWidget(management);
  addDockWidget(Qt::LeftDockWidgetArea, dockCfg);

  setCentralWidget(m_d->m_mainWidget);

  QAction *a = new QAction(tr("About"), this);
  QMenu *m = menuBar()->addMenu(tr("Help"));
  m->addAction(a);
  connect(a, SIGNAL(triggered()), this, SLOT(slotAbout()));

  //Remove spaces
  setStyleSheet(
      "QMainWindow::separator {"
          "background: rgb(200, 200, 200);"
          "width: 1px;"
          "height: 1px;"
      "}");
}

bool Finances::login()
{
  LoginDialog *dlg = new LoginDialog(this);
  connect(dlg, &LoginDialog::success, this, &Finances::slot_successLogin);
  dlg->exec();
  delete dlg;

  return m_bSuccessLogin;
}

void Finances::slot_successLogin(LoginDialog *dialog)
{
    try {
        m_d->m_db = new DataBase(dialog->login(), dialog->password());
        m_d->m_db->login();
    } catch (QString error) {
        QMessageBox::critical(this, tr("Login Failed"), error);
        delete m_d->m_db;
        m_d->m_db = 0;
        dialog->setFocusPassword(true);
    } catch(int code) {
      QMessageBox::critical(this, tr("App Init Fail"), tr("Can't initialize app. Check app output for more details"));
      delete m_d->m_db;
      m_d->m_db = 0;      
  }


  m_bSuccessLogin = true;
  dialog->accept();

  show();
  m_modules = new Modules(m_d);
  m_modules->loadModules();
}

void Finances::slotClickBottomPanelItem(NavPanelItem *item)
{
    m_d->m_mainWidget->setCurrentIndex(item->widgetIndex());

    if(m_d->m_navPanel->currentItem()) {
       m_d->m_navPanel->currentItem()->setSelected(false);
    }
}

void Finances::slotClickNavPanelItem(QTreeWidgetItem *item)
{
    if( 0 == item->parent() ) {
        return;
    }

    NavigationGroup* group = static_cast<NavigationGroup*>(item->parent());
    m_d->m_mainWidget->setCurrentIndex(group->indexModule());
    ((NavPanel*)m_d->m_cfg->parent())->unseletCurrentItem();
}

void Finances::slotAbout()
{
    QMessageBox::about(this, "About", "<b>QFinances</b> - track manager home finances.<br/>"
                       "<br/> "
                       "Author <a href=\"http://peter_komar.byethost17.com/\">Peter Komar</a>"
                       "<br/><br/><b>License:</b> GPL v2"
                       "<br/> 2007 - " + QDateTime::currentDateTime().toString("yyyy") +
                       "<br/><br/><b>Build </b>: 2.0." + QString::number(QDateTime::currentMSecsSinceEpoch()));
}

