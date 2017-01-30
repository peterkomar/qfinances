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
#ifndef MY_FINANCES_DATABASE_H
#define MY_FINANCES_DATABASE_H

#include <QList>

#include <QVariant>

enum TransactionType {TRANSACTION_IN=0, TRANSACTION_OUT, TRANSACTION_SYS};

struct Transaction {
  qint64          date;
  double          value;
  QString         description;
  TransactionType type;
};

typedef QList<Transaction> tranactions;

struct Currency {
  int     id;
  QString name;
  QString symbol;
  double  value;
};

typedef QList<Currency> currencies;

struct Bill {
    int      id;
    QString  name;
    Currency currency;
    int      created_at;
    double   value;
    QString  comments;
    bool     status;
};

typedef QList<Bill> Bills;

/**
class Data base finances implementation

  @author Peter Komar <marcusk@i.ua>
*/
class myFinancesDatabase
{ 
 public:
   enum Setting {Account=15, Password=50, DefaultBill = 10};
   myFinancesDatabase();
   ~myFinancesDatabase();

   bool create(const QString& account, const QString& password);
   bool login(const QString& account, const QString& password);
   void logout();

   currencies getCurrencies();
   Currency getCurrencyById(int id);
   bool saveCurrency( Currency value );
   bool removeCurrency(int id);

   Bills getBills();
   Bill getBill(int id);
   bool saveBill(Bill bill);

   tranactions getTransactions(int billId, const QString& filter);
   bool addTransaction( Transaction data, int billId );

   QString getLastError() const { return eMessage; }

private:

   bool init(QString const& dbName);

   bool create_account() const;

   QString getHomeDirPath() const;

   bool setSettingValue( Setting code, const QString& value );
   QString getSettingValue( Setting code );

   void closeConnection();

   QString m_dbName;

   QString m_account;

   QString eMessage;
};

#endif
