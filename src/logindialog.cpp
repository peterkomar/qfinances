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

#include "logindialog.h"
#include "database.h"

#include <QCloseEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QFormLayout>
#include <QGridLayout>
#include <QRegularExpression>

LoginDialog::LoginDialog(QWidget *parent) :
  QDialog(parent)
{
  _gui();
  setModal(true);
  _tabOrder();

  QStringList list = DataBase::listAccounts();
  if( list.isEmpty() ) {
      _setModeCreate();
  } else {
      _setModeCreate(false);
  }

  list << tr("< add new >");
  m_selectAccount->addItems(list);

  m_linePassword->setFocus(Qt::MouseFocusReason);
}

QString LoginDialog::login() const
{
  return m_selectAccount->currentText().trimmed();
}

QString LoginDialog::password() const
{
  return m_linePassword->text();
}

void LoginDialog::setFocusPassword(bool b)
{
    if( b ) {
        m_linePassword->setFocus(Qt::MouseFocusReason);
        m_linePassword->selectAll();
    } else {
        m_linePassword->clearFocus();
    }
}

void LoginDialog::slot_accept()
{
  if( bModeCreate && m_linePassword->text() != m_linePassword2->text()) {
    QMessageBox::critical(this, tr("Passwords Error"), tr("Passwords are not equal"));
    return;
  }

  if( m_selectAccount->currentText().trimmed() == "" ) {
    QMessageBox::critical(this, tr("Name Error"), tr("Account name is empty"));
    return;
  }

  QRegularExpression re("^[a-zA-Z0-9]+$");
  QRegularExpressionMatch match = re.match(m_selectAccount->currentText().trimmed());
  if (!match.hasMatch()) {
    QMessageBox::critical(this, tr("Name Error"), tr("Account name contains incorrect symbols"));
    return;
  }

  emit success(this);
}

void LoginDialog::slot_new(const QString & text)
{
  _setModeCreate(text == tr("< add new >"));
}

void LoginDialog::closeEvent(QCloseEvent *event)
{
  reject();
  event->accept();
}

void LoginDialog::_gui()
{
  m_selectAccount = new QComboBox(this);
  m_selectAccount->setFixedWidth(200);
  m_selectAccount->setEditable(false);
  connect(m_selectAccount, SIGNAL(activated(const QString& )), this, SLOT(slot_new(const QString& )));

  m_linePassword = new QLineEdit();
  m_linePassword->setFixedWidth(200);
  m_linePassword->setEchoMode(QLineEdit::Password);

  m_labelPassword2 = new QLabel(tr("Password again:"));

  m_linePassword2 = new QLineEdit();
  m_linePassword2->setFixedWidth(200);
  m_linePassword2->setEchoMode(QLineEdit::Password);

  m_loginBtn = new QPushButton(tr("Login"));
  m_loginBtn->setDefault(true);
  connect(m_loginBtn, SIGNAL(pressed()), this, SLOT(slot_accept()));

  QPushButton *exitBtn = new QPushButton(tr("Exit"));
  connect(exitBtn, SIGNAL(pressed()), this, SLOT(reject()));

  QLabel *label = new QLabel;
  label->setPixmap(QPixmap(":img/png/view-bank-account-savings.png"));

  //Setup layouts

  QFormLayout *fieldsLayout = new QFormLayout;
  fieldsLayout->addRow(new QLabel(tr("Login:")), m_selectAccount);
  fieldsLayout->addRow(new QLabel(tr("Password:")), m_linePassword);
  fieldsLayout->addRow(m_labelPassword2, m_linePassword2);
  fieldsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  fieldsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
  fieldsLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
  fieldsLayout->setLabelAlignment(Qt::AlignLeft);

  QVBoxLayout *pButtons = new QVBoxLayout;
  pButtons->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  pButtons->addWidget(m_loginBtn);
  pButtons->addWidget(exitBtn);

  QGridLayout *formLayout = new QGridLayout;
  formLayout->addWidget(label,0,0, Qt::AlignLeft | Qt::AlignTop);
  formLayout->addLayout(fieldsLayout, 0,1, Qt::AlignLeft | Qt::AlignTop);
  formLayout->addLayout(pButtons, 0,2, Qt::AlignLeft | Qt::AlignTop);
  formLayout->setSizeConstraint(QLayout::SetFixedSize);
  setLayout(formLayout);

}

void LoginDialog::_tabOrder()
{
  QWidget::setTabOrder(m_selectAccount, m_linePassword);
  QWidget::setTabOrder(m_linePassword, m_linePassword2);
  QWidget::setTabOrder(m_linePassword2, m_loginBtn);
}

void LoginDialog::_setModeCreate(bool mode)
{
  bModeCreate = mode;

  if(bModeCreate) {
      m_selectAccount->setEditable(true);
      m_labelPassword2->setVisible(true);
      m_linePassword2->setVisible(true);
      m_loginBtn->setText(tr("Create"));
  } else {
      m_selectAccount->setEditable(false);
      m_labelPassword2->setVisible(false);
      m_linePassword2->setVisible(false);
      m_loginBtn->setText(tr("Login"));
  }
}
