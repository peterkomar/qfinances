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

#include "topheader.h"

#include <QtGui>

topHeader::topHeader(QWidget *p)
        :QWidget(p)
{
    setFixedHeight(50);
}

void topHeader::paintEvent ( QPaintEvent * /*event*/ )
{
  QPainter p(this);
  p.fillRect(rect(), QBrush(qApp->palette().window() ));

  QLinearGradient headerBackgroundGrd(0,0, 0,100);
  headerBackgroundGrd.setColorAt(0, QColor(217,217,217));
  headerBackgroundGrd.setColorAt(1, QColor(164,164,164));
  p.fillRect(QRect(0,0,width(),200), QBrush(headerBackgroundGrd));


  QFont font("Times", 14, QFont::Bold, true);
  p.setFont(font);
  p.setPen(QColor(255,255,255));
  p.drawText(rect(), Qt::AlignBottom | Qt::AlignRight, tr("myFinances v1.0"));;
}
