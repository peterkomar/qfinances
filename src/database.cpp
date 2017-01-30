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

#include "database.h"
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDateTime>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QSqlTableModel>

const char* APP_KEY = "UDLApp";
const char* APP_NAME = "myFinances";

DataBase::DataBase(const QString& dbName, const QString& password)
{
    m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", dbName));
    m_db->setUserName("");
    m_db->setPassword(password);
    open();
}

DataBase::~DataBase()
{
    this->logout();
}

void DataBase::open()
{
    QString db_path   = DataBase::getHomeDirPath() + "a" +m_db->connectionName() + ".ab";
    bool dbExists = QFile::exists(db_path);
    m_db->setDatabaseName(db_path);
    if (!m_db->open()) {
        QSqlDatabase::removeDatabase(m_db->connectionName());
        throw QObject::tr("Unable to establish a database connection.");
    }

    setup();
    //If it creating a new accont init config
    if (!dbExists) {
        setSettingValue(Account, m_db->connectionName());
        setSettingValue(Password, _encrypt(m_db->password()));
    }
}

void DataBase::setup()
{
    QStringList tables = m_db->tables();
    if (!tables.contains("settings")) {
        initV1();
    }

    m_settings = new QSqlTableModel(Q_NULLPTR, *m_db);
    m_settings->setTable("settings");
    m_settings->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

void DataBase::initV1()
{
    QSqlQuery *q = query();

    q->prepare(
          "CREATE TABLE settings ("
          "code int,"
          "value varchar"
          ")"
    );
    exec(q);
}

QString DataBase::getHomeDirPath()
{
    QString accounts_dir_name = "myfinances2";

    QDir home = QDir::home();
    if( !home.exists(accounts_dir_name) ) {
        home.mkdir(accounts_dir_name);
    }

    home.cd(accounts_dir_name);

    return home.absolutePath() + "/";
}

QSqlDatabase* DataBase::db()
{
    return m_db;
}

QSqlQuery* DataBase::query()
{
    return new QSqlQuery(*m_db);
}

void DataBase::exec(QSqlQuery* query)
{
  if( !query->exec() ) {
#ifdef MFN_DEBUG
      qDebug() << "Error execution query: " << query->lastQuery() << " Error: " << query->lastError().text();
#endif
      delete query;
      query =  0;
      throw -1;
  }

#ifdef MFN_DEBUG
  qDebug() << _lastQuery(query);
#endif

  if( !query->isSelect() ) {
      delete query;
      query = 0;
  }
}

QString DataBase::_lastQuery(QSqlQuery* query)
{
    QString str = query->lastQuery();
     QMapIterator<QString, QVariant> it(query->boundValues());
     while (it.hasNext()) {
         it.next();
         str.replace(it.key(),it.value().toString());
     }
     return str;
}

QStringList DataBase::listAccounts()
{
  QDir dir(DataBase::getHomeDirPath());
  dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Name);

  QStringList filters;
  filters << "a*.ab";
  dir.setNameFilters(filters);

  QStringList result;
  QFileInfoList list = dir.entryInfoList();
  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);
    QString file = fileInfo.fileName();
    file.remove(0,1);
    file.remove(QRegularExpression(".ab$"));
    result << file;
  }

  return result;
}

QByteArray DataBase::_encrypt(const QString str)
{
  QDateTime date = QDateTime::fromString("1983-06-09 18:45:00");

  QString s = str + date.toMSecsSinceEpoch();
  QByteArray smd5 = QCryptographicHash::hash(s.toLatin1(), QCryptographicHash::Md5);

  return smd5.toHex();
}

void DataBase::login()
{
    if (getSettingValue(Password) != _encrypt(m_db->password())) {
        throw QObject::tr("Access denided. Password is incorrect.");
    }
}

void DataBase::logout()
{
    delete m_settings;
    m_settings = 0;
    QString name = m_db->connectionName();
    m_db->close();
    delete m_db;

    QSqlDatabase::removeDatabase(name);
}

QString DataBase::getSettingValue( Setting code )
{
    m_settings->setFilter(QString("code = %1").arg((int)code));
    m_settings->select();

    if (m_settings->rowCount()  == 1) {
        return m_settings->record(0).value("value").toString();
    }

    return QString("NULL");
}

void DataBase::setSettingValue( Setting code, const QString& value )
{    
    m_settings->setFilter(QString("code = %1").arg((int)code));
    m_settings->select();

    if (!m_settings->rowCount()) {
        QSqlRecord record = m_settings->record();
        record.setValue("code", QVariant(code));
        record.setValue("value", QVariant(value));
        m_settings->insertRecord(-1, record);
    } else {
        QSqlRecord record = m_settings->record(0);
        record.setValue("value", value);
        m_settings->setRecord(0, record);
    }
    m_settings->submitAll();
}

int DataBase::nextId(const QString &table)
{    
    QSqlQuery *q = query();
    q->prepare("SELECT max(id) FROM " + table);
    exec(q);

    int index = 0;
    if( q->first() ) {
      index = q->value(0).toInt();
    }
    delete q;

    return index + 1;
}

void DataBase::setAppSettingValue( const QString& code, const QVariant& value )
{
  QSettings settings(APP_KEY, APP_NAME);
  settings.setValue(code, value);
}

QVariant DataBase::getAppSettingValue( const QString& code )
{
  QSettings settings(APP_KEY, APP_NAME);
  return settings.value(code);
}
