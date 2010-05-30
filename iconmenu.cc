// aewm++ - A small C++ window manager developed from aewm 0.9.6 around 2000 
// Copyright (C) 2000-2010 Frank Hale <frankhale@gmail.com>
//
// aewm++ can be found here: http://code.google.com/p/aewmpp/
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Date: 30 May 2010

#include "aewm.h"

IconMenu::IconMenu(Display * dpy) : GenericMenu(dpy)
{
	updateMenu();
	addToMenuList(this);
}

void IconMenu::handleButtonReleaseEvent(XButtonEvent *e)
{
	GenericMenu::handleButtonReleaseEvent(e);

	switch (e->button) 
	{
		case Button1:
			if (curr) 
			{
				switch(	curr->function )
				{
					case SHOW:
						curr->client->unhide();
						hideAllVisibleSubmenus();
					break;
				
				}										
			}	
		break;
	} 
}

void IconMenu::addThisClient(Client *c)
{
	if(c)
	{
		BaseMenuItem *item = new BaseMenuItem();
	
		item->client = c;
		
		if(c->getClientIconName() == NULL)
		{
			item->name = "untitled";
		}
		else
		{
			item->name = c->getClientIconName();
		}
		
		item->function=SHOW;
	
		insert(item);
	}
}

void IconMenu::updateClientName(Client *c)
{
	list<BaseMenuItem*> menuItemList = getMenuItemList();
	list<BaseMenuItem*>::iterator mit;
	
	for(mit = menuItemList.begin(); mit != menuItemList.end(); mit++)
	{
		if((*mit)->client == c) 
		{
			if(c->getClientIconName() == NULL)
			{
				(*mit)->name = "Untitled";
			}
			else
			{
				(*mit)->name = c->getClientIconName();
			}
			
			updateMenu();
		}
	}
}

void IconMenu::removeClientFromIconMenu(Client *c)
{
	list<BaseMenuItem*> menuItemList = getMenuItemList();
	list<BaseMenuItem*>::iterator mit;
	
	for(mit = menuItemList.begin(); mit != menuItemList.end(); mit++)
	{
		if((*mit)->client == c) 
		{
			remove(*mit);
			delete *mit;
			curr=NULL;
			break;
		}
	}
}
