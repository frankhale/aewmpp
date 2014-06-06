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


#ifndef _WINDOWMANAGER_H_
#define _WINDOWMANAGER_H_

#include "aewm.h"

class WindowManager
{
private: /* member variables */

	list<Client*> client_list;
	list<Window> client_window_list;

	WindowMenu *window_menu;
	IconMenu   *icon_menu;
	
	Client* focused_client;
	XFontStruct *font;
	GC invert_gc, string_gc, border_gc, unfocused_gc, focused_title_gc;
	XColor fg, bg, bd, fc, focused_border, unfocused_border;
	
	Cursor move_curs, arrow_curs;

	Display *dpy;	
	Window 	root;   
	Window	_button_proxy_win;  

	int screen, current_desktop, xres, yres, shape, shape_event, max_desktops, focus_model;

	string 	command_line;
		
	char 	*opt_display,	
		*opt_fc, 
		*opt_fg,
		*opt_fm, 
		*opt_bg, 
		*opt_bd, 
		*opt_tj,
		*opt_wm,
		*opt_wp,
		*opt_es,
		*opt_new1;
		
	int	opt_bw, opt_text_justify;
	bool 	wire_move, rand_window_placement, edge_snap;	

	static KeySym alt_keys[];
	
 	Atom	atom_wm_state,
	        atom_wm_change_state,
		atom_wm_protos,
		atom_wm_delete, 
		atom_wm_cmapwins,
		atom_wm_takefocus;

	/* Member Functions */
	
	void setupSignalHandlers();
	void setupDisplay();	
	void cleanup();
	void doEventLoop();
	void scanWins();
	
	void handleKeyPressEvent(XEvent *ev);
	void handleButtonPressEvent(XEvent *ev);
	void handleButtonReleaseEvent(XEvent *ev);
	void handleConfigureRequestEvent(XEvent *ev);
	void handleMotionNotifyEvent(XEvent *ev);
	void handleMapRequestEvent(XEvent *ev);
	void handleUnmapNotifyEvent(XEvent *ev);
	void handleDestroyNotifyEvent(XEvent *ev);
	void handleEnterNotifyEvent(XEvent *ev);
	void handleLeaveNotifyEvent(XEvent *ev);
	void handleFocusInEvent(XEvent *ev);
	void handleFocusOutEvent(XEvent *ev);
	void handleClientMessageEvent(XEvent *ev);
	void handleColormapNotifyEvent(XEvent *ev);
	void handlePropertyNotifyEvent(XEvent *ev);
	void handleExposeEvent(XEvent *ev);
	void handleDefaultEvent(XEvent *ev);

	static void sigHandler(int signal);
	
public: /* Member Functions */

	WindowManager(int argc, char** argv);

	void parseCommandLine(int argc, char** argv);
	void quitNicely();
	void restart();

	Client* getFocusedClient() { return focused_client; }
	
	void addClient(Client *c);
	void removeClient(Client* c);
	Client* findClient(Window w);

	void focusPreviousWindowInStackingOrder();
	void unfocusAnyStrayClients();
	void findTransientsToMapOrUnmap(Window win, bool hide); 
	
	inline WindowMenu* getWindowMenu() 	const { return window_menu; }
	inline IconMenu*   getIconMenu() 	const { return icon_menu; }
	
	void updateIconMenu();
	void addClientToIconMenu(Client *c);
	void updateClientNameOnIconMenu(Client *c);
	void removeClientFromIconMenu(Client *c);
	
	inline XFontStruct* getFont() 	const { return font; 		}
	inline GC getInvertGC() 	const { return invert_gc; 	}
	inline GC getStringGC() 	const { return string_gc; 	}
	inline GC getBorderGC() 	const { return border_gc; 	}
	inline GC getUnfocusedGC() 	const { return unfocused_gc; 	}
	inline GC getFocusedTitleGC() 	const { return focused_title_gc;}
	inline Cursor getMoveCursor() 	const { return move_curs;	}
	inline Cursor getArrowCursor()  const { return arrow_curs; 	}
	inline XColor getFGColor() 	const { return fg; 		}
	inline XColor getFCColor() 	const { return fc; 		}
	inline XColor getBGColor() 	const { return bg; 		}
	inline XColor getBDColor() 	const { return bd; 		}
	
	inline XColor getFocusedBorderColor() const { return focused_border; }
	inline XColor getUnFocusedBorderColor() const { return unfocused_border; }
		
	inline int getShape() 		const { return shape; }

	long getWMState(Window window);
	void setWMState(Window window, int state);
	void sendWMDelete(Window window);
	int sendXMessage(Window w, Atom a, long mask, long x);

	void getMousePosition(int *x, int *y);
	void goToDesktop(int d);
	inline int getCurrentDesktop() const { return current_desktop; }
	void setCurrentDesktop(int desk);

	// Returns a number corresponding to the current focus model.
	inline int getFocusModel() const { return focus_model; }

	// Accepts a number corresponding to a new focus model.
	inline void setFocusModel(int new_fm)
	{
		switch (new_fm)
		{
			case FOCUS_FOLLOW:
			case FOCUS_SLOPPY:
			case FOCUS_CLICK:
				focus_model = new_fm;			
			break;
						
			default:
				focus_model=FOCUS_CLICK;
			break;
		}
	}

	inline int getTextJustify()	const { return opt_text_justify; }
	inline bool getWireMove() 	const { return wire_move; }
	inline bool getEdgeSnap() 	const { return edge_snap; }
	inline bool getRandPlacement() 	const { return rand_window_placement; }
	inline int getMaxDesktops() 	const { return max_desktops; }
	inline int getXRes() const { return xres; }
	inline int getYRes() const { return yres; }
	inline int getOptBW() 	const { return opt_bw; 		}
	inline Atom getWMChangeStateAtom() const { return atom_wm_change_state; }
	inline Atom getWMProtosAtom() const { return atom_wm_protos; }
	inline Atom getWMTakeFocusAtom() const { return atom_wm_takefocus; }
	
	void grabKeys(Window w);
	void ungrabKeys(Window w);
};

extern WindowManager *wm;

#endif
