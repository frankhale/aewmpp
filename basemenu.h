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

#ifndef _BASEMENU_H_
#define _BASEMENU_H_

#include "aewm.h"

// Not all have been implemented yet (for basemenu)
enum { 	
	SEND_TO_DESKTOP=0,
//	EXECUTE_COMMAND=1, 
//	MAXIMIZE=2,
//	ICONIFY=3,
// 	UNICONIFY=4,
//	CLOSE=5, 
//	LOWER=6,
//	SHADE=7
//
	SHOW=8,
	NONE=99
};

class BaseMenuItem 
{
public:	
	BaseMenuItem() {
		client		= NULL;
		icon		= None;
		name		= "";
		exec		= "";
		function	= 0;
		is_selected	= false;
		item_x		= 0;
		item_y		= 0;
		index		= 0;
		sub		= NULL; 
	}
	
	Client *client; 
	Window icon;   // this menu item may be pointing to an icon for use
		       // in an icon list or something.
	
	std::string name; // name showing on menu.
	std::string exec; // command to execute when clicked.
	
	int item_x, item_y;
	int function, index;
	bool is_selected;
	BaseMenu *sub; // submenu this item points to.
};

class BaseMenu
{
protected:	
	list<BaseMenuItem*> mi;
	
	Display *dpy;
	Window  root;
	int screen;

	Window item_window;
	int x, y, x_move, y_move;
	int width,height, total_item_height;
	bool is_visible;
	
	XColor xforeground_color, xbackground_color, xborder_color, xmenuselect_color;

	GC gc;
	XFontStruct *font;
	XCharStruct overall;
	
	int direction, ascent, descent, counter;
	bool bottom_edge, right_edge;
	int item_width, item_height;
	
	// Used to know which item to paint.
	BaseMenuItem *curr;
	bool enterOnce;	

public:		
	BaseMenu(Display * dpy);
	virtual ~BaseMenu();

	inline list<BaseMenuItem*> getMenuItemList() const { return mi; }
	inline int getItemCount() const { return mi.size(); }
	inline bool isVisible() const { return is_visible; }
					
	void setMenuPos(int x, int y);
	void show();
	void show(int x, int y);
	void showSub(BaseMenu *sub, int x, int y);
	void hide(BaseMenu *sub);
	void hideAllVisibleSubmenus();
	
	void updateMenu();

	virtual void insert(string n, BaseMenu *sub);
	virtual void insert(string n, std::string exec, int func);
	virtual void insert(string n, std::string exec, BaseMenu *sub, int func);
	virtual void insert(BaseMenuItem *item);

	int remove(BaseMenuItem *element);
	void removeAll();

	BaseMenuItem *findMenuItem(int x, int y);

	virtual void handleButtonPressEvent(XButtonEvent *e);
	virtual void handleButtonReleaseEvent(XButtonEvent *e);
	void handleEnterNotify(XCrossingEvent *e);
	void handleLeaveNotify(XCrossingEvent *e);
	void handleExposeEvent(XExposeEvent *e);
	void handleMotionNotifyEvent(XMotionEvent *e);

	// The menu item behavoir is defined with these
	// virtual functions in a derived class.
	virtual void handleButton1Press(BaseMenuItem *curr){} 
	virtual void handleButton2Press(BaseMenuItem *curr){} 
	virtual void handleButton3Press(BaseMenuItem *curr){}
	virtual void handleButton1Release(BaseMenuItem *curr){}
	virtual void handleButton2Release(BaseMenuItem *curr){}
	virtual void handleButton3Release(BaseMenuItem *curr){}

	void execute(std::string s);

	inline Window getMenuWindow() const { return item_window; }

	virtual void redraw();
	void hide();

private:
	void initializeMenu(Display *display);

	virtual void redraw(BaseMenuItem *m);
		
	void selectMenuItem(unsigned long col);
	void getMousePosition(int *x, int *y);
	void testMenuEdgeDetect(BaseMenu *sub);		

	// Not meant to be called directly by subclasses! Used internally.
	void hideSubmenus(); 
	
	void setForeground (unsigned long pixel);
	void draw3DLine(Window win, int x1, int y1, int x2, int y2);
};

#endif
