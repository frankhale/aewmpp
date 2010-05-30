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

BaseMenu::BaseMenu(Display * dpy)
{
	initializeMenu(dpy);
}

void BaseMenu::initializeMenu(Display *d)
{
	XSetWindowAttributes attrib;
	long create_mask;	
	
	dpy = d;
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	item_window = None;
	x=0;
	y=0;
	x_move=0;
	y_move=0;
	width=1;
	height=1;
	total_item_height=0;
	is_visible=false;
		
	direction=0;
	ascent=0;
	descent=0;
	counter=0;
	
	bottom_edge=false;
	right_edge=false;
	item_width=0;
	item_height=0;
	curr=NULL;
	enterOnce=false;
	
	font = XLoadQueryFont(dpy, DEF_FONT);

	// Colors
	XColor dummy;
	
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), DEF_BD, &xforeground_color, &dummy);
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), DEF_FC, &xbackground_color, &dummy);
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), DEF_BD, &xborder_color, &dummy);
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), DEF_BG, &xmenuselect_color, &dummy);
	
  	create_mask = CWBackPixmap       | 
		      CWBackPixel        | 
		      CWBorderPixel      |
	    	      CWOverrideRedirect |
		      CWEventMask; 
		      
  	attrib.background_pixmap = None;
  	attrib.background_pixel  = xbackground_color.pixel;
  	attrib.border_pixel 	 = xborder_color.pixel;
  	attrib.override_redirect = true;
  	attrib.event_mask = ButtonPressMask   |
	                    ButtonReleaseMask | 
			    PointerMotionMask |
    			    ExposureMask      |
			    EnterWindowMask   |
			    LeaveWindowMask;

	total_item_height=1;
       	item_width  = width;
	item_height = font->ascent + font->descent + 8;

	item_window = 
		XCreateWindow(
			      dpy, 
			      root, 
			      x, 
			      y,
		  	      width, 
			      height, 
			      1, 
			      DefaultDepth(dpy, screen),
		  	      InputOutput, 
			      DefaultVisual(dpy,screen), 
			      create_mask, 
			      &attrib
			      );
	XGCValues gv;
	gv.function = GXcopy;
    	gv.foreground = wm->getFGColor().pixel;
    	gv.font = font->fid;
    	gc = XCreateGC(dpy, item_window, GCFunction|GCForeground|GCFont, &gv);

	enterOnce = true;
}

BaseMenu::~BaseMenu() 
{			
	mi.clear();

	XDestroyWindow(dpy, item_window);
	
	XFreeFont(dpy, font);
	
	XFreeGC(dpy, gc);
}

// This updates all graphics contexts, gradient pixmaps and determines placement of all menu items.
void BaseMenu::updateMenu()
{
	int temp_width=0;
	int temp_ascent=0;
	int temp_descent=0;
	unsigned int count=0;
	unsigned int charcount=0;
	int text_width=0;

	total_item_height=1;

	list<BaseMenuItem*>::iterator it;

	for(it = mi.begin(); it != mi.end(); it++)
	{
		charcount = (*it)->name.length();
		
		XTextExtents(
			font, 
			(*it)->name.c_str(), 
			charcount, 
			&direction, 
			&temp_ascent, 
		        &temp_descent, 
			&overall
			);

		if(charcount > count)
		{
			count = charcount;
			
			temp_width = overall.width;
			temp_ascent = overall.ascent;
			temp_descent = overall.descent;

			width = temp_width + 35;
			item_width  = temp_ascent + temp_descent + 4;
			height = item_height * mi.size();
		
			text_width = temp_width;
		}
		
		(*it)->item_x = (width/2) - (text_width/2);
		(*it)->item_y = (item_height/2) + total_item_height + 2;
		
		total_item_height += item_height;
	}
	
	XSetWindowBackground (dpy, item_window, xbackground_color.pixel);
			
	testMenuEdgeDetect(this);
	
	if (y + height >= wm->getYRes()) 
		bottom_edge=True;
 	else 		
		bottom_edge=False;
	
	if(bottom_edge)
		XMoveWindow(dpy, item_window, x, wm->getYRes() - height - item_height - 4);
	
	XResizeWindow(dpy, item_window, width, height);
	
	redraw();
}

void BaseMenu::hide()
{
	XUnmapWindow(dpy, item_window);
	
	is_visible=false;
}

// Redraws the entire contents of the menu.
void BaseMenu::redraw()
{	
	list<BaseMenuItem*>::iterator it;
	
	for(it = mi.begin(); it != mi.end(); it++)
	{	  				
		if((*it)->name == "separator")
		{
			draw3DLine(item_window, 0, (*it)->item_y-5, width, (*it)->item_y-5);
		} else {
		
			setForeground(xforeground_color.pixel); // Set the gc so it has a black color
		
			XDrawString(dpy, item_window, gc, 
				(*it)->item_x, 
				(*it)->item_y + 2, 
				(*it)->name.c_str(), 
				(*it)->name.length());
						
			if((*it)->sub)
			{
				XDrawRectangle(dpy, item_window, gc, 
					width - 16, 
					(*it)->item_y - 7, 
					8,
					8);
			}
		}
	}
}

// Redraws a single menu item on the menu.
void BaseMenu::redraw(BaseMenuItem *i)
{
	if(i->name == "separator")
	{
		draw3DLine(item_window, 0, i->item_y-5, width, i->item_y-5);
	} else {		
		setForeground(xforeground_color.pixel);
		
		XDrawString(dpy, item_window, gc,  i->item_x, i->item_y + 2, i->name.c_str(), i->name.length());
		
		if(i->sub)
		{
			XDrawRectangle(dpy, item_window, gc, 
					width - 16, 
					i->item_y - 7, 
					8, 
					8);	
		} 
		
	}

}

void BaseMenu::insert(string n, BaseMenu *sub)
{
	BaseMenuItem *item = new BaseMenuItem();
	
	item->name = n;
	item->sub  = sub;

	// defaults
	item->is_selected=False;
	item->exec = ""; 
	item->function = 0;
	item->item_x = item->item_y = 0;

	mi.push_back(item);
}

void BaseMenu::insert(string n, std::string exec, int func)
{
	BaseMenuItem *item = new BaseMenuItem();
	
	const char* temp = exec.c_str();
	
	if(temp[0] == '~')
	{
		std::string exec_temp = getenv("HOME");
		exec_temp += temp;
		exec = temp;
	}
	
	item->exec = exec;
	item->name = n;
	item->function = func;

	// defaults
	item->is_selected=False;
	item->sub  = None;
	item->item_x = item->item_y = 0;

	mi.push_back(item);
}


void BaseMenu::insert(string n, std::string exec, BaseMenu *sub, int func)
{
	BaseMenuItem *item = new BaseMenuItem();

	const char* temp = exec.c_str();
	
	if(temp[0] == '~')
	{
		std::string exec_temp = getenv("HOME");
		exec_temp += temp;
		exec = temp;
	}
	
	item->exec = exec;
	item->name = n;

	item->is_selected=False;
	item->sub  = sub;
	item->function = func;
	item->item_x = item->item_y = 0;
	
	mi.push_back(item);
}

void BaseMenu::insert(BaseMenuItem *item)
{
	if(item) mi.push_back(item);
}

int BaseMenu::remove(BaseMenuItem* element)
{
	mi.remove(element);

	updateMenu();

	return mi.size();
}

void BaseMenu::removeAll()
{
	mi.clear();
	
	updateMenu();
}

void BaseMenu::execute(std::string s)
{
	if(! (s == ""))
	{
		pid_t pid = fork();

    		switch (pid) 
		{
        		case 0:
            			execlp("/bin/sh", "sh", "-c", s.c_str(), NULL);
	    			cerr << "exec failed, cleaning up child" << endl;
            			exit(1);
        		case -1:
	    			cerr << "can't fork" << endl;
    		}
	}
}

void BaseMenu::show()
{	
	int mouse_x=0;
	int mouse_y=0;
	
	// Gets the coordinates of the mouse
	getMousePosition(&mouse_x, &mouse_y);
		
	// Check to make sure menu will be displayed on the screen.
 	if (mouse_x + width > wm->getXRes()) 
	{
 		x = wm->getXRes() - width - 1;
		right_edge=true;
 	
	} else {
	
		x = mouse_x;
		right_edge=false;

	}
	
	if (mouse_y + height > wm->getYRes()) 
	{
		y = wm->getYRes() - height;
		bottom_edge=true;

 	} else {
		
		y = mouse_y;
		bottom_edge=false;

	}
	
	// Move the menu to the position of the mouse pointer
	XMoveWindow(dpy, item_window, x, y);
	
	// Show the menu windows
	XMapRaised(dpy, item_window);
	
	is_visible=true;
}

void BaseMenu::getMousePosition(int *x, int *y)
{
	Window dw1=None;
	Window dw2=None;
	int t1=0;
	int t2=0;
	unsigned int t3=0;

	XQueryPointer(dpy, root, &dw1, &dw2, x, y, &t1, &t2, &t3);
}

void BaseMenu::show(int nx, int ny)
{	
	x = nx;
	y = ny;

	XMoveWindow(dpy, item_window, x, y);
	
	// Show the menu window
	XMapRaised(dpy, item_window);

	is_visible=true;
}

void BaseMenu::showSub(BaseMenu *sub, int nx, int ny)
{
	sub->x = nx;
	sub->y = ny;
	
	XMoveWindow(dpy, sub->item_window, sub->x, sub->y);
		
	// Show the menu window
	XMapRaised(dpy,  sub->item_window);

	sub->is_visible=true;

}

void BaseMenu::hide(BaseMenu *sub)
{
	if (sub->is_visible)
	{
		// hide the menu windows
		XUnmapWindow(dpy, sub->item_window);

		sub->is_visible=false;
	}
}

BaseMenuItem *BaseMenu::findMenuItem(int x, int y)
{
	int item_xpos2=0;
	int item_ypos2=0;
	
	if(mi.size()) 
	{
		list<BaseMenuItem*>::iterator it;
	
		for(it = mi.begin(); it != mi.end(); it++)
		{
			item_xpos2 = width-1;
			item_ypos2 = (*it)->item_y + item_height - 15;
	  
			if ( (x >= (*it)->item_x - 14) && 
			     (x < item_xpos2) &&
			     (y >= (*it)->item_y - 14) &&  
			     (y < item_ypos2)
			   ) 
			    {
			    	  return (*it);
			    } 
		}	
	}

	return NULL;
}

void BaseMenu::handleButtonPressEvent(XButtonEvent *e)
{
	switch (e->button) 
	{
		case Button1:
			if (curr) handleButton1Press(curr);
		break;

		case Button2:
			if(curr) handleButton2Press(curr);
		break;
			
		case Button3:
			if (curr) handleButton3Press(curr);
		break;
	} 
}

void BaseMenu::handleButtonReleaseEvent(XButtonEvent *e)
{
	switch (e->button) 
	{
		case Button1:
			if (curr) handleButton1Release(curr);
		break;

		case Button2:
			if (curr) handleButton2Release(curr);
		break;
			
		case Button3:
			if (curr) handleButton3Release(curr);
		break;
	} 
}

void BaseMenu::setMenuPos(int nx, int ny)
{
	x = nx; 
	y = ny;
	
	XMoveWindow(dpy, item_window, x, y);
}

void BaseMenu::selectMenuItem(unsigned long col)
{
	setForeground( col );
		
	if (! (curr->name == "separator"))
	{
		XClearArea(dpy, item_window, 0, 
			curr->item_y - 15, 
			item_width - item_height - 2, 
			item_height + 3,
			true);
		
		XFillRectangle(dpy, item_window, gc, 
			0, 
			curr->item_y - 15, 
			item_width - item_height - 2, 
			item_height + 3
		);
	}
}

void BaseMenu::hideSubmenus()
{
	list<BaseMenuItem*> temp_mi;
	list<BaseMenuItem*>::iterator it;
	
	for(it = mi.begin(); it != mi.end(); it++)
	{
		if ((*it)->sub && (*it)->sub->is_visible)
		{
			(*it)->sub->hide((*it)->sub);
			
			temp_mi = (*it)->sub->mi;
			
			// Oh yeah isn't this good programming practice?
			LOOP: 
				
			//LinkedListIterator<BaseMenuItem> n(temp_mi);
			list<BaseMenuItem*>::iterator n_it;
			for(n_it = temp_mi.begin(); n_it != temp_mi.end(); n_it++)
			{
				if((*n_it)->sub && (*n_it)->sub->is_visible)
				{
					(*n_it)->sub->hide((*n_it)->sub);
					
					temp_mi = (*n_it)->sub->mi;
					
					// I love goto's	
					goto LOOP;						
				} 					
			}
		} 
	}	

}

void BaseMenu::handleEnterNotify(XCrossingEvent *e)
{
	if(curr)
	{					
		hideSubmenus(); // Hide the submenu's which are visible but not needed anymore.
		
		selectMenuItem( xmenuselect_color.pixel );

		if(curr->sub) 
		{
			testMenuEdgeDetect(curr->sub);
			
			if(curr->sub->right_edge) 
			{
				if(curr->sub->bottom_edge)
				{
					showSub(curr->sub, x - curr->sub->width - 1, 
					wm->getYRes() - curr->sub->height);
				}					
				else
					showSub(curr->sub,
						x - curr->sub->width - 1, 
						y + curr->item_y - item_height + 5
						);
					
				}
			else {
				if(curr->sub->bottom_edge)
				{
					showSub(curr->sub,
							x + width + 1, 
							wm->getXRes() - curr->sub->height + item_height
						);
	
				}
				else {
					showSub(curr->sub,
						x + width + 1, 
						y + curr->item_y - item_height + 5
						);
				}
			}
		} 
		redraw(curr);
	}
}

void BaseMenu::handleLeaveNotify(XCrossingEvent *e)
{
	if(curr)
	{	
		selectMenuItem( xbackground_color.pixel );
		redraw(curr);
		curr = NULL;			
	}
}

void BaseMenu::hideAllVisibleSubmenus()
{
	hide(this);
	hideSubmenus();
}

void BaseMenu::handleExposeEvent(XExposeEvent *e)
{
	if(e->count == 0) redraw();
}

void BaseMenu::handleMotionNotifyEvent(XMotionEvent *e)
{
	XEvent temp;	
	
	if(e->window == item_window)
	{
		BaseMenuItem *i = findMenuItem(e->x, e->y);

		if (i && enterOnce) 
		{
			curr = i;

			handleEnterNotify(&temp.xcrossing);				

			enterOnce=false;
		} else {

			BaseMenuItem *i = findMenuItem(e->x, e->y);
			
			if (i != curr)
			{
				handleLeaveNotify(&temp.xcrossing);
			
				curr =  i;
				
				enterOnce=true;
			} 
		}
	}
}

void BaseMenu::testMenuEdgeDetect(BaseMenu *sub)
{
	int mouse_x=0;
	int mouse_y=0;
	
	bool rightEdge=False;
	bool bottomEdge=False;
	
	// Gets the coordinates of the mouse
	getMousePosition(&mouse_x, &mouse_y);

	rightEdge = ( ( x + sub->width + 150 ) < wm->getXRes() ) ? true : false;
	
	bottomEdge = ( mouse_y + sub->height >= wm->getYRes() ) ? true : false;
	
	if (mouse_y + sub->height >= wm->getYRes() )
	{
		//cout << " Menu is at bottom edge " << endl;
		bottomEdge = true;
	} else {
		//cout << " mouse_y + sub->menu.height = " << mouse_y + sub->menu.height
  		//     << " wm->getYRes() = " << wm->getYRes() << endl;
		bottomEdge = false;
	}
	
	// Check to make sure menu will be displayed on the screen.
	if (rightEdge) sub->right_edge=false;
 	else sub->right_edge=true;
	
	if (bottomEdge) sub->bottom_edge=true;
	else sub->bottom_edge=false;
}

void BaseMenu::setForeground (unsigned long pixel)
{
	XSetForeground (dpy, gc, pixel);
}

void BaseMenu::draw3DLine(Window win, int x1, int y1, int x2, int y2)
{
  	//setForeground(3);
  	setForeground(xforeground_color.pixel);
	XDrawLine(dpy, win, gc, x1, y1, x2, y2);
 	//setForeground(4);
	setForeground(xbackground_color.pixel);
  	XDrawLine(dpy, win, gc, x1, y1-1, x2, y2-1);
}
