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

#include "modules.h"

//include account plugin
#include "modules/account/mainaccount.h"
#include "modules/currency/maincurrency.h"
#include "modules/category/maincategory.h"

Modules::Modules(ModuleParams *params)
{
  m_p = params;
}

Modules::~Modules()
{
    m_lModules.clear();
}

void Modules::loadModules()
{
    //Register system modules
    _registerModule("category", new MainCategory(m_p));
    _registerModule("currency", new MainCurrency(m_p));
    _registerModule("account", new MainAccount(m_p));

    //Add Your modules here
    // for example:
    // _registerModule("module_name", new myModule(m_p));
}

void Modules::_registerModule(const QString& name, Module *module)
{
    module->exec();
    m_lModules.insert(name, module);
}
