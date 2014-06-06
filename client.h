// aewm++ - A small C++ window manager developed from aewm 0.9.6 around 2000 
// Frank Hale <frankhale@gmail.com>
//
// aewm++ can be found here: https://github.com/frankhale/aewmpp
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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "aewm.h"

class Client
{
private: /* Member Variables */

    	Display 	*dpy;
	Window 		root;
	XSizeHints	*size;
  Colormap	cmap;
	int 		screen;
	char		*name;
	Window		window, frame, title, trans;	
	WindowMenu 	*window_menu; 
	int 		border_width;
	int  		x, y, width, height;	
	int  		old_x, old_y, old_width, old_height;   
  bool has_focus, 
	     has_title, 
	     has_border, 
	     is_being_dragged, 
	     is_being_resized, 
	     do_drawoutline_once, 
	     wire_move,
	     is_shaded,
	     is_iconified,
	     is_maximized,
	     is_visible,
	     has_been_shaped;

  int belongs_to_desktop, ignore_unmap;
    	Time last_button1_time;
    	 	
	// For window title placement
	XCharStruct overall;
	int direction,
	    ascent,
	    descent,
	    text_width,
	    text_justify,
	    justify_style;
	
	// Used in client move
	int pointer_x, pointer_y, old_cx, old_cy;
	bool button_pressed;

private: /* Member Functions */

	void initialize(Display *d);
	
	void redraw();
	void drawOutline();
	int  getIncsize(int *, int *, int);
	void initPosition();
	void reparent();
	int  theight();
	void sendConfig();
	void gravitate(int);

	void setShape();

public: /* Member Functions */

	Client(Display *d, Window new_client);
	~Client();
	
	void getXClientName();
		
	void makeNewClient(Window);
	void removeClient();
	
	char* getClientName() const { return name; }
	char* getClientIconName() const { return name; } // for now just return application name
	
	Window getFrameWindow() const	{ return frame; }
	Window getAppWindow() 	const { return window; }
	Window getTitleWindow()	const { return title;	}
	Window getTransientWindow() const { return trans; }

	bool isTransient() { if(trans) return true; else return false; }
	
	bool hasWindowDecorations() 	const { return has_title; }
	bool hasFocus()			const { return has_focus; }
	
	int belongsToWhichDesktop() 	const { return belongs_to_desktop;}
	bool isIconified() 		const { return is_iconified; 	}
	bool isVisible()		const { return is_visible;	}

	void setFocus(bool focus); // (decieving name) Only paints the titlebar in the focus color

	void hide();
	void unhide();
	void iconify();
	void shade();
	void maximize();

	void setDesktop(int desk);

	void handleButtonEvent(XButtonEvent *);
	void handleConfigureRequest(XConfigureRequestEvent *);
	void handleMapRequest(XMapRequestEvent *);
	void handleUnmapEvent(XUnmapEvent *);
	void handleDestroyEvent(XDestroyWindowEvent *);
	void handleClientMessage(XClientMessageEvent *);
	void handlePropertyChange(XPropertyEvent *);
	void handleEnterEvent(XCrossingEvent *);
	void handleColormapChange(XColormapEvent *);
	void handleExposeEvent(XExposeEvent *);
	void handleFocusInEvent(XFocusChangeEvent *);
	void handleMotionNotifyEvent(XMotionEvent *);
	void handleShapeChange(XShapeEvent *);
};

#endif
