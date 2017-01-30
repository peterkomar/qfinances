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

#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QSqlQueryModel>

#include "project.h"

class DataBase
{
public:
  enum Setting {Account=15, Password=50, DefaultBill = 10};

  DataBase(const QString& dbName, const QString& password );
  ~DataBase();
  void login();
  void logout();

  QSqlQuery* query();
  void exec(QSqlQuery* query);
  QSqlDatabase* db();

  void setSettingValue( Setting code, const QString& value );
  QString getSettingValue( Setting code );

  void setAppSettingValue( const QString& code, const QVariant& value );
  QVariant getAppSettingValue( const QString& code );

  static QStringList listAccounts();
  int nextId(const QString& table);

private:  
  void setup();
  void initV1();
  void open();

  QByteArray _encrypt(const QString str);
  QString _lastQuery(QSqlQuery* query);

  static QString getHomeDirPath();

  QSqlDatabase *m_db;
  QSqlTableModel *m_settings;
};

#endif // DATABASE_H
