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

#ifndef CATEGORYDLG_H
#define CATEGORYDLG_H

#include "dialog.h"

class Category;

class QComboBox;
class QLineEdit;
class QTextEdit;

class CategoryDlg : public Dialog
{
  Q_OBJECT
public:
  explicit CategoryDlg(Category *category, QWidget *parent = 0);
  ~CategoryDlg();

protected slots:
  void slot_save();

private:
  Category *m_category;

  QComboBox *m_uiType;
  QComboBox *m_parentCategories;
  QLineEdit *m_uiName;
  QLineEdit *m_uiTax;
  QLineEdit *m_uiValue;
  QTextEdit *m_uiComment;

  void _gui(QGridLayout *layout);
  void _data();
};

#endif // CATEGORYDLG_H
