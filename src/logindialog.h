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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;

class LoginDialog : public QDialog
{
  Q_OBJECT
public:
  LoginDialog(QWidget *parent = 0);
  QString login() const;
  QString password() const;
  void setFocusPassword(bool );

signals:
  void success(LoginDialog *);
  
public slots:
  void slot_accept();
  void slot_new(const QString& text);

protected:
    void closeEvent(QCloseEvent *event);

private:
    void _gui();
    void _tabOrder();
    void _setModeCreate(bool mode = true);

    bool bModeCreate;
    QLineEdit *m_linePassword;
    QLabel   *m_labelPassword2;
    QLineEdit *m_linePassword2;
    QComboBox *m_selectAccount;
    QPushButton *m_loginBtn;
};

#endif // LOGINDIALOG_H
