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

#ifndef FNCDIALOG_H
#define FNCDIALOG_H

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QFormLayout>

class Model;

class Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog(QWidget *parent = 0, bool readOnly = false);

    int dialog();

protected slots:
    virtual void slot_save() = 0;

protected:
    virtual void _gui(QGridLayout *layout) = 0;
    virtual void _data() = 0;
    virtual void _readOnly() {}

    void addLine(QGridLayout *dialogLayout, int width = 100);
    void addLine(QFormLayout *formLayout, int width = 100);
    void addSpacer(QGridLayout *dialogLayout);
    void addSpacer(QFormLayout *formLayout);
    QFormLayout* getFormLayout();

    void saveData(Model *model);

    bool isReadOnly;
};

#endif // FNCDIALOG_H
