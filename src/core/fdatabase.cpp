/***************************************************************************
 *   Copyright (C) 2013 by Peter Komar                                     *
 *   marcusk@i.ua                                                          *
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
//
//
#include "fdatabase.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>

const char* dbExtansion = ".ab";

myFinancesDatabase::myFinancesDatabase()
    :m_account("")
{
  m_dbName = "";
  eMessage = "See console output for more details";
}

myFinancesDatabase::~myFinancesDatabase()
{}

bool myFinancesDatabase::create(const QString& account, const QString& password)
{
  qint64 date = QDateTime::currentMSecsSinceEpoch();


  QString fileName = getHomeDirPath() + "a" + QString::number(date) + dbExtansion;

  if( QFile::exists( fileName ) ) {
    eMessage = "Can't create account. "+fileName + " is exists.";
    return false;
  }

  if( !init(fileName) ) {
     return false;
  }

  m_dbName = fileName;

  if( !create_account() ) {
    m_dbName = "";
    return false;
  }

  QString newPassword = account + password;

  QByteArray smd5 = QCryptographicHash::hash( newPassword.toLatin1(), QCryptographicHash::Md5);

  if( !setSettingValue(Account, account) || !setSettingValue(Password, smd5.toHex()) ){
    m_dbName = "";
    return false;
  }

  return true;
}

bool myFinancesDatabase::login(const QString &account, const QString &password)
{
  QString fileName = getHomeDirPath() + account + dbExtansion;

  if( !init(fileName) ) {
    return false;
  }

  m_dbName = fileName;

  m_account = account;

  QString s = account + password;

  QByteArray hash = QCryptographicHash::hash( s.toLatin1(), QCryptographicHash::Md5);

  if( getSettingValue(Password) != hash.toHex() ) {
    eMessage = "Access denided. Password is incorrect.";
    m_dbName = "";
    return false;
  }

  return true;
}

void myFinancesDatabase::logout()
{
  closeConnection();
}

void myFinancesDatabase::closeConnection()
{
  QSqlDatabase::database(m_dbName).close();
  QSqlDatabase::removeDatabase( m_dbName );
}

QString myFinancesDatabase::getHomeDirPath() const
{
    QSettings settings("UDLApp", "myFinances");

    QString path = settings.value("account/path").toString();
    if( path == "" ) {
        path = QDir::homePath()+ "/";
    }

    return path;
}

bool myFinancesDatabase::setSettingValue( Setting type, const QString& value )
{
  QString result = getSettingValue(type);
  QSqlQuery query(QSqlDatabase::database(m_dbName));

  if( result == "NULL" ) {
    query.prepare("INSERT INTO settings (code, value) VALUES (:code, :value)");
  } else {
    query.prepare("UPDATE settings SET value = :value WHERE code = :code");
  }

  query.bindValue(":code", type);
  query.bindValue(":value", value);

  if ( !query.exec() ) {
    qDebug() << query.lastError();
    return false;
  }

  return true;
}

QString myFinancesDatabase::getSettingValue( Setting type )
{
  QSqlQuery query(QSqlDatabase::database(m_dbName));

  query.prepare(""
         "SELECT "
            " value "
         " FROM settings "
         " WHERE "
         "    code = :code"
  );

  query.bindValue(":code", QString::number((int)type));

  if(!query.exec()) {
    qDebug() << "Failed to execute "+query.lastQuery();
    return "";
  }

  if( query.first() ) {
    return query.value(0).toString();
  }

  return "NULL";
}

tranactions myFinancesDatabase::getTransactions(int billId, const QString& filter )
{
  QSqlQuery query(QSqlDatabase::database(m_dbName));

  QString queryString  = "SELECT * FROM transactions WHERE bill_id = " + QString::number((int)billId);

  if( filter != "" ) {
    queryString += " AND " + filter;
  }

  tranactions result;

  query.prepare( queryString );
  if(!query.exec()) {
    qDebug() << "Failed to execute "+query.lastQuery();
    return result;
  }

  int dateNo        = query.record().indexOf("date");
  int typeNo        = query.record().indexOf("type");
  int valueNo       = query.record().indexOf("value");
  int descriptionNo = query.record().indexOf("description");

  while (query.next()) {

    Transaction data;
    data.date        = query.value(dateNo).toInt();
    data.type        = (TransactionType)query.value(typeNo).toInt();
    data.value       = query.value(valueNo).toDouble();
    data.description = query.value(descriptionNo).toString();

    result.append(data);
  }

  return result;
}

bool myFinancesDatabase::addTransaction( Transaction data, int billId)
{
  QSqlQuery query(QSqlDatabase::database(m_dbName));

  query.prepare("INSERT INTO transactions "
                "(date, type, value, description, bill_id"
               "VALUES"
               "(:date, :type, :value, :description, :bill_id)");

  query.bindValue(":date",        data.date);
  query.bindValue(":type",        data.type);
  query.bindValue(":value",       data.value);
  query.bindValue(":description", data.description);
  query.bindValue(":bill_id",     billId);

  if ( !query.exec() ) {
    qDebug() << query.lastError();
    return false;
  }

  double value = (data.type == TRANSACTION_IN)? data.value : -data.value;

  query.prepare("UPDATE bill "
                "SET value = value + "+QString::number(value) +
               " WHERE "
               " bill_id = :bill_id");

  query.bindValue(":bill_id", billId);

  if ( !query.exec() ) {
    qDebug() << query.lastError();
    return false;
  }

  return true;
}

 currencies myFinancesDatabase::getCurrencies()
 {
     QSqlQuery query(QSqlDatabase::database(m_dbName));

     currencies result;
     query.prepare( "SELECT * FROM currencies" );
     if(!query.exec()) {
       qDebug() << "Failed to execute: "+query.lastQuery();
       return result;
     }

     int idNo     = query.record().indexOf("id");
     int nameNo   = query.record().indexOf("name");
     int valueNo  = query.record().indexOf("value");
     int symbolNo = query.record().indexOf("symbol");

     while (query.next()) {

       Currency data;
       data.id     = query.value(idNo).toInt();
       data.name   = query.value(nameNo).toString();
       data.value  = query.value(valueNo).toDouble();
       data.symbol = query.value(symbolNo).toString();

       result.append(data);
     }

     return result;
 }

 bool myFinancesDatabase::saveCurrency( Currency value )
 {
     QSqlQuery query(QSqlDatabase::database(m_dbName));

     int id = value.id;
     if( id == 0 ) {

         query.prepare("SELECT max(id) FROM currencies ");
         if(!query.exec()) {
           qDebug() << "Failed to execute: "+query.lastQuery();
           return false;
         }

         query.first();
         id = query.value(0).toInt() + 1;

         query.prepare("INSERT INTO currencies "
                       "(id, name, value, symbol"
                      "VALUES"
                      "(:id, :name, :value, :symbol)");
     } else {
         query.prepare("UPDATE currencies SET "
                       " name = :name, value = :value, symbol = :symbol"
                      " WHERE id = :id");
     }

     query.bindValue(":id",     id);
     query.bindValue(":name",   value.name);
     query.bindValue(":value",  value.value);
     query.bindValue(":symbol", value.symbol);

     if ( !query.exec() ) {
       qDebug() << query.lastError();
       return false;
     }

     return true;
 }

Currency myFinancesDatabase::getCurrencyById(int id)
{
    QSqlQuery query(QSqlDatabase::database(m_dbName));

    Currency data;
    data.id     = 0;
    data.name   = "";
    data.value  = 0;
    data.symbol = "";

    query.prepare( "SELECT * FROM currencies WHERE id = :id" );
    query.bindValue(":id", id);
    if(!query.exec()) {
      qDebug() << "Failed to execute: "+query.lastQuery();
      return data;
    }

    int idNo     = query.record().indexOf("id");
    int nameNo   = query.record().indexOf("name");
    int valueNo  = query.record().indexOf("value");
    int symbolNo = query.record().indexOf("symbol");

    if( query.first() ) {
        Currency data;
        data.id     = query.value(idNo).toInt();
        data.name   = query.value(nameNo).toString();
        data.value  = query.value(valueNo).toDouble();
        data.symbol = query.value(symbolNo).toString();
    }

    return data;
}

bool myFinancesDatabase::removeCurrency(int id)
{
     QSqlQuery query(QSqlDatabase::database(m_dbName));
     query.prepare( "DELETE FROM currencies WHERE id = :id" );
     query.bindValue(":id", id);
     if(!query.exec()) {
       qDebug() << "Failed to execute: "+query.lastQuery();
       return false;
     }

     return true;
}

Bills myFinancesDatabase::getBills()
{
    QSqlQuery query(QSqlDatabase::database(m_dbName));

    Bills result;
    query.prepare( "SELECT * FROM bill" );
    if(!query.exec()) {
      qDebug() << "Failed to execute: "+query.lastQuery();
      return result;
    }

    int idNo        = query.record().indexOf("bill_id");
    int nameNo      = query.record().indexOf("name");
    int currencyNo  = query.record().indexOf("currency_id");
    int createdAtNo = query.record().indexOf("created_at");
    int valueNo     = query.record().indexOf("value");
    int commentsNo  = query.record().indexOf("comments");
    int statusNo    = query.record().indexOf("status");

    while (query.next()) {

      Bill data;
      data.id         = query.value(idNo).toInt();
      data.name       = query.value(nameNo).toString();
      data.currency   = getCurrencyById(query.value(currencyNo).toInt());
      data.created_at = query.value(createdAtNo).toInt();
      data.value      = query.value(valueNo).toDouble();
      data.comments   = query.value(commentsNo).toString();
      data.status     = query.value(statusNo).toBool();

      result.append(data);
    }

    return result;
}

Bill myFinancesDatabase::getBill(int id)
{
    QSqlQuery query(QSqlDatabase::database(m_dbName));

    Bill data;
    query.prepare( "SELECT * FROM bill WHERE bill_id = :bill_id" );
    query.bindValue(":bill_id", id);
    if(!query.exec()) {
      qDebug() << "Failed to execute: "+query.lastQuery();
      return data;
    }

    if( query.first() ) {

        int idNo        = query.record().indexOf("bill_id");
        int nameNo      = query.record().indexOf("name");
        int currencyNo  = query.record().indexOf("currency_id");
        int createdAtNo = query.record().indexOf("created_at");
        int valueNo     = query.record().indexOf("value");
        int commentsNo  = query.record().indexOf("comments");
        int statusNo    = query.record().indexOf("status");

        data.id         = query.value(idNo).toInt();
        data.name       = query.value(nameNo).toString();
        data.currency   = getCurrencyById(query.value(currencyNo).toInt());
        data.created_at = query.value(createdAtNo).toInt();
        data.value      = query.value(valueNo).toDouble();
        data.comments   = query.value(commentsNo).toString();
        data.status     = query.value(statusNo).toBool();

    }

    return data;
}

bool myFinancesDatabase::saveBill(Bill bill)
{
    QSqlQuery query(QSqlDatabase::database(m_dbName));

    int billId = bill.id;
    if( billId == 0 ) {

        query.prepare("SELECT max(bill_id) FROM bill");
        if(!query.exec()) {
          qDebug() << "Failed to execute: "+query.lastQuery();
          return false;
        }

        query.first();

        billId = query.value(0).toInt() + 1;
        query.prepare("INSERT INTO bill "
                      "(bill_id, name, currency_id, created_at, value, comments, status"
                     " VALUES "
                     " (:bill_id, :name, :currency_id, :created_at, :value, :comments, :status)");

    } else {
        query.prepare("UPDATE bill "
                      "SET "
                      "name        = :name, "
                      "currency_id = :currency_id, "
                      "created_at  = :created_at, "
                      "value       = :value, "
                      "comments    = :comments, "
                      "status      = :status"
                     " WHERE "
                     " bill_id = :bill_id"
        );
    }

    query.bindValue(":bill_id",     billId);
    query.bindValue(":name",        bill.name);
    query.bindValue(":currency_id", bill.currency.id);
    query.bindValue(":created_at",  bill.created_at);
    query.bindValue(":value",       bill.value);
    query.bindValue(":comments",    bill.comments);
    query.bindValue(":status",      (int)bill.status);

    if(!query.exec()) {
      qDebug() << "Failed to execute: "+query.lastQuery();
      return false;
    }

    return true;
}

bool myFinancesDatabase::create_account() const
{
  QSqlQuery query(QSqlDatabase::database(m_dbName));

  QStringList queries;
  queries
  << "CREATE TABLE tranactions ("
              "date int,"
              "type int,"
              "value double,"
              "description mediumtext,"
              "bill_id int"
  ")"
  << "CREATE TABLE bill ("
              "bill_id int,"
              "name varchar,"
              "currency_id int,"
              "created_at int,"
              "value double,"
              "comments varchar,"
              "status int"
  ")"
  << "CREATE TABLE currencies ("
              "id int,"
              "name varchar,"
              "value double,"
              "symbol varchar"
  ")"
  << "CREATE TABLE settings ("
              "code int,"
              "value varchar"
  ")"
  << "CREATE TABLE categories ("
              "id int,"
              "name varchar,"
              "value double,"
              "message varchar"
  ")";

  QStringList::const_iterator constIterator;
  for (constIterator = queries.constBegin(); constIterator != queries.constEnd(); ++constIterator) {

      if (!query.exec(*constIterator)) {
        qDebug() << "Failed to execute: " << (*constIterator).toLocal8Bit().constData();
        return false;
      }
  }

  return true;
}

bool myFinancesDatabase::init(const QString &dbName)
{
    QSqlDatabase database;

        if( QSqlDatabase::database().databaseName() != dbName ) {
            database = QSqlDatabase::addDatabase("QSQLITE", dbName);
            database.setDatabaseName(dbName);
            database.setUserName("");
            database.setPassword("");
        } else {
            database = QSqlDatabase::database();
            if ( database.isOpen() ) {
                return true;
            }
        }

        //
        if (!database.open()) {
            eMessage =  QObject::tr("Unable to establish a database connection.")+"\n"+
                                  QObject::tr("myFinances needs SQLite support. Please read "
                                  "the Qt SQL driver documentation for information how "
                                  "to build it.");

            database = QSqlDatabase();
            QSqlDatabase::removeDatabase(dbName);

            return false;
        }

      return true;
}

