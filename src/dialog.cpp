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

#include "dialog.h"
#include "model.h"

#include <QPushButton>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent, bool readOnly)
    :QDialog(parent)
    ,isReadOnly(readOnly)
{
    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowContextHelpButtonHint);
}

int Dialog::dialog()
{
    QGridLayout *dialogLayout = new QGridLayout;
    dialogLayout->setContentsMargins(0, 10, 0, 10);

    this->_gui(dialogLayout);
    this->_data();

    addSpacer(dialogLayout);

    QPushButton *cancelBtn = new QPushButton(tr("Cancel"));

    QHBoxLayout *lButtons = new QHBoxLayout;
    lButtons->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    lButtons->addWidget(cancelBtn);

    if (isReadOnly) {
        this->_readOnly();
        cancelBtn->setText(tr("Close"));
    } else {
        QPushButton *saveBtn = new QPushButton(tr("OK"));
        saveBtn->setDefault(true);
        //slot
        connect(saveBtn, SIGNAL(clicked()), this, SLOT(slot_save()));
        lButtons->addWidget(saveBtn);
    }

    lButtons->addSpacing(20);



    //Slots
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));

    addSpacer(dialogLayout);
    dialogLayout->addLayout(lButtons, dialogLayout->rowCount(), 0, Qt::AlignRight | Qt::AlignTop);
    dialogLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(dialogLayout);

    exec();

    return result();
}

void Dialog::saveData(Model *model)
{
    if(model->save()) {
        accept();
    } else {
        QString message = model->lastErrorMessage();
        if(message.isEmpty()) {
            message = tr("Can't save data. Check application output for more details");
        }

        QMessageBox::critical(this, tr("Error"), message);
    }
}

QFormLayout* Dialog::getFormLayout()
{
    QFormLayout *fieldsLayout = new QFormLayout;

    fieldsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    fieldsLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    fieldsLayout->setFormAlignment(Qt::AlignRight | Qt::AlignTop);
    fieldsLayout->setLabelAlignment(Qt::AlignRight);
    fieldsLayout->setContentsMargins(0,10,10,10);

    return fieldsLayout;
}

void Dialog::addLine(QGridLayout *dialogLayout, int width)
{
    QFrame *line = new QFrame(this);
    line->setFrameShadow(QFrame::Plain);
    line->setFrameShape(QFrame::HLine);
    line->setMinimumWidth(width);
    line->setMinimumHeight(20);

    dialogLayout->addWidget(line);
}

void Dialog::addLine(QFormLayout *formLayout, int width)
{
    QFrame *line = new QFrame(this);
    line->setFrameShadow(QFrame::Plain);
    line->setFrameShape(QFrame::HLine);
    line->setMinimumWidth(width);
    line->setMinimumHeight(20);

    formLayout->addWidget(line);
}

void Dialog::addSpacer(QGridLayout *dialogLayout)
{
    QFrame *line = new QFrame(this);
    line->setFrameShadow(QFrame::Plain);
    line->setFrameShape(QFrame::NoFrame);

    dialogLayout->addWidget(line);
}

void Dialog::addSpacer(QFormLayout *formLayout)
{
    QFrame *line = new QFrame(this);
    line->setFrameShadow(QFrame::Plain);
    line->setFrameShape(QFrame::NoFrame);

    formLayout->addWidget(line);
}


