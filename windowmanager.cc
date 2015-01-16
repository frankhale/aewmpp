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
// Date: 16 January 2015


#include "aewm.h"

WindowManager* wm;

#define AEWM_KEY_ALT_COUNT 4
KeySym WindowManager::alt_keys[]=
{
  XK_Delete,
  XK_End,
  XK_Page_Up,
  XK_Page_Down
};

WindowManager::WindowManager(int argc, char** argv)
{
	wm = this; 
  
  current_desktop=0;
  
	focused_client=NULL;
  
	parseCommandLine(argc, argv);
  
  if(max_desktops <= 0) max_desktops=MAX_DESKTOPS;
  
	setupSignalHandlers();
	setupDisplay();
	scanWins();
	
  // If there are any iconified clients, add them to the icon menu.
	updateIconMenu(); 
  
	doEventLoop();
}

void WindowManager::parseCommandLine(int argc, char** argv)
{
	// Make the default options equal something
	opt_fm = (char*) DEF_FM;
  opt_fg = (char*) DEF_FG;
  opt_fc = (char*) DEF_FC;
  opt_bg = (char*) DEF_BG;
  opt_bd = (char*) DEF_BD;
  opt_tj = (char*) TEXT_JUSTIFY;
	opt_wm = (char*) WIRE_MOVE;
	opt_es = (char*) EDGE_SNAP;
  opt_new1 = (char*) DEF_NEW1;
  opt_bw = DEF_BW;
  opt_wp = (char*) DEF_WP;
  opt_display=NULL;
	max_desktops=MAX_DESKTOPS;
  
  #define OPT_STR(name, variable)                                      \
  if (strcmp(argv[i], name) == 0 && i+1<argc) {                    \
    variable = argv[++i];                                        \
    continue;                                                    \
  }
  #define OPT_INT(name, variable)                                      \
  if (strcmp(argv[i], name) == 0 && i+1<argc) {                    \
    variable = atoi(argv[++i]);                                  \
    continue;                                                    \
  }
  
	// Create a command line string, i.e. the command line used to 
	// run this iteration of the window manager. If the user restarts
	// the window manager while running we will restart it with the same
	// options used to start it the first time.
	for (int i = 0; i < argc; i++) command_line = command_line + argv[i] + " ";
  
	// Get the args and test for different options
	for (int i = 1; i < argc; i++) 
  {
    OPT_STR("-fg", opt_fg)
    OPT_STR("-bg", opt_bg)
		OPT_STR("-fc", opt_fc)
		OPT_STR("-fm", opt_fm)
    OPT_STR("-bd", opt_bd)
    OPT_STR("-new1", opt_new1)
		OPT_STR("-display", opt_display)
		OPT_STR("-tj", opt_tj)
		OPT_STR("-wm", opt_wm)
		OPT_STR("-es", opt_es)
		OPT_STR("-wp", opt_wp)
		
		OPT_INT("-bw", opt_bw)
		OPT_INT("-md", max_desktops)
    
    if (strcmp(argv[i], "-version") == 0) 
    {
			cout << "Version: " << VERSION << endl;
			cout << "Date: " << DATE << endl;
      exit(0);
    }
    
		if(strcmp(argv[i], "-usage")==0) 
    {
      cerr << "usage: " << WINDOW_MANAGER_NAME << " [options]" << endl;
      cerr << "   options are: -display <display>, -fg|-bg|-bd <color>, " << endl;
      cerr << "   -bw <width>, -md <max desktops>, -tj <left|center|right>, -wm <true|false>," << endl;
			cerr << "    -new1|-new2 <cmd>, -fm (follow|sloppy|click), -wp (mouse|random), -usage, -help" << endl;
			exit(0);
		}
    
    if(strcmp(argv[i], "-help")==0) 
    {
      cerr << "help: " << WINDOW_MANAGER_NAME << endl << endl;
      cerr << "-display specifies a display to start the window manager on, The default is display :0." << endl;
      cerr << "-fg, -bg and -bd are colors you wish the foreground, background and border to be for window titlebars." << endl;
      cerr << "-bw is the border width of the window." << endl;
      cerr << "-md is the number of maximum virtual desktops, the default is 4." << endl;
      cerr << "-tj is the text justify variable, its default is center, but you can specify left or right also." << endl;
      cerr << "-new1 and -new2 are commands you wish the first and second mouse buttons to execute when pressed on the root window." << endl;
      cerr << "-fm is the focus model you want to use, the default is click to focus." << endl;
      cerr << "-wp is the window placement model you want to use, the default is place by mouse." << endl;
      cerr << "-es is for edge snapping, pass either true or false here." << endl;
			cerr << "-usage prints a reduced version of this information." << endl;
      cerr << "-help prints this message." << endl << endl;
      
      exit(0);
    }
	}
  
	// Set the focus model based on user defined option
	if (strcmp(opt_fm, "follow")==0) setFocusModel(FOCUS_FOLLOW);
	else if (strcmp(opt_fm, "sloppy")==0) setFocusModel(FOCUS_SLOPPY);
	else if (strcmp(opt_fm, "click")==0) setFocusModel(FOCUS_CLICK);
	else setFocusModel(FOCUS_SLOPPY);	
  
	// Set up the window title justification per user defined option
	if(strcmp(opt_tj, "left")==0) opt_text_justify = LEFT_JUSTIFY;
	else if(strcmp(opt_tj, "center")==0) opt_text_justify = CENTER_JUSTIFY;
	else if(strcmp(opt_tj, "right")==0) opt_text_justify = RIGHT_JUSTIFY;
	else opt_text_justify = LEFT_JUSTIFY;
  
	// Set wire move based on user defined option
	if(strcmp(opt_wm, "true")==0) wire_move=true;
	else if(strcmp(opt_wm, "false")==0) wire_move=false;
	else wire_move=false;
  
	// Set edge snapping based on user defined option
	if(strcmp(opt_es, "true")==0) edge_snap=true;
	else if(strcmp(opt_es, "false")==0) edge_snap=false;
	else edge_snap=false;
  
	// Set window placement model
	if (strcmp(opt_wp,"random")==0) rand_window_placement = true;
	else if (strcmp(opt_wp,"mouse")==0) rand_window_placement = false;
	else rand_window_placement = false;
}

void WindowManager::setupSignalHandlers()
{
 	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);
	signal(SIGHUP, sigHandler);
	signal(SIGCHLD, sigHandler);
}

void WindowManager::setCurrentDesktop(int desk)
{
	if ( (desk < max_desktops) && (desk > 0) ) current_desktop = desk;
	
	updateIconMenu();
}

void WindowManager::addClientToIconMenu(Client *c)
{
	icon_menu->hide();
	
	if(c->belongsToWhichDesktop() == current_desktop)
		icon_menu->addThisClient(c);
	
	icon_menu->updateMenu();
}

void WindowManager::removeClientFromIconMenu(Client *c)
{
	icon_menu->hide();
	icon_menu->removeClientFromIconMenu(c);
	icon_menu->updateMenu();
}

void WindowManager::updateClientNameOnIconMenu(Client *c)
{
	icon_menu->updateClientName(c);
}

void WindowManager::updateIconMenu()
{
	icon_menu->hide();
	icon_menu->removeAll();
	
	list<Client*>::iterator it;
	for(it = client_list.begin(); it != client_list.end(); it++)
	{
		if((*it)->belongsToWhichDesktop() == current_desktop)
		{
			if((!(*it)->isTransient()) && ((*it)->hasWindowDecorations())) icon_menu->addThisClient(*it);
		}
	}
	
	icon_menu->updateMenu();
}

void WindowManager::goToDesktop(int d)
{
	unsigned int nwins, i;
	Window dummyw1, dummyw2, *wins;
	Client* c;
  
	setCurrentDesktop(d);
  
	XSetInputFocus(dpy, _button_proxy_win, RevertToNone, CurrentTime);
  
	// Preserve stacking order
	XQueryTree(dpy, root, &dummyw1, &dummyw2, &wins, &nwins);
	for (i = 0; i < nwins; i++) 
	{
		c = findClient(wins[i]);
    
		if(c)
		{
			if(c->belongsToWhichDesktop() == current_desktop)
			{
				if(! (c->isIconified())) 
          c->unhide();
			}
			else {
				if(! (c->isIconified())) 
          c->hide();												
			}
		}
	}
	XFree(wins);
}

void WindowManager::scanWins(void)
{
	unsigned int nwins, i;
  Window dummyw1, dummyw2, *wins;
  XWindowAttributes attr;
	Client *c=NULL;
  
	XQueryTree(dpy, root, &dummyw1, &dummyw2, &wins, &nwins);
	for(i = 0; i < nwins; i++) 
	{
    XGetWindowAttributes(dpy, wins[i], &attr);
    if (!attr.override_redirect && attr.map_state == IsViewable)
		{
      client_window_list.push_back(wins[i]);
			c = new Client(dpy, wins[i]);
		}
  }
  XFree(wins);
  
	XMapWindow(dpy, _button_proxy_win);
	grabKeys(_button_proxy_win);
	XSetInputFocus(dpy, _button_proxy_win, RevertToNone, CurrentTime);
}

void WindowManager::setupDisplay()
{
  XColor dummyc;
  XGCValues gv;
  XSetWindowAttributes sattr;
  int dummy;
  
	if (opt_display)
    setenv("DISPLAY", opt_display, 1);
	else
    opt_display = getenv("DISPLAY");
  
  dpy = XOpenDisplay(opt_display);
  
  if (!dpy) {
		cerr << "can't open display! check your DISPLAY variable." << endl;
    exit(1);
	}
  
  screen = DefaultScreen(dpy);
  root = RootWindow(dpy, screen);
  
	xres = DisplayWidth(dpy, screen);
	yres = DisplayHeight(dpy, screen);
	
	XSetErrorHandler(handleXError);
  
	// ICCCM atoms
	atom_wm_state 		= XInternAtom(dpy, "WM_STATE", False);
	atom_wm_change_state 	= XInternAtom(dpy, "WM_CHANGE_STATE", False);
	atom_wm_protos 		= XInternAtom(dpy, "WM_PROTOCOLS", False);
	atom_wm_delete 		= XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	atom_wm_cmapwins 	= XInternAtom(dpy, "WM_COLORMAP_WINDOWS", False);
	atom_wm_takefocus 	= XInternAtom(dpy, "WM_TAKE_FOCUS", False);
  
	XSetWindowAttributes pattr;
	pattr.override_redirect=True;
	_button_proxy_win=XCreateSimpleWindow(dpy, root, -80, -80, 24, 24,0,0,0);
	XChangeWindowAttributes(dpy, _button_proxy_win, CWOverrideRedirect, &pattr);
	
  XAllocNamedColor(dpy, DefaultColormap(dpy, screen), opt_fg, &fg, &dummyc);
  XAllocNamedColor(dpy, DefaultColormap(dpy, screen), opt_bg, &bg, &dummyc);
  XAllocNamedColor(dpy, DefaultColormap(dpy, screen), opt_bd, &bd, &dummyc);
  XAllocNamedColor(dpy, DefaultColormap(dpy, screen), opt_fc, &fc, &dummyc);
  
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), FOCUSED_BORDER_COLOR, &focused_border, &dummyc);
	XAllocNamedColor(dpy, DefaultColormap(dpy, screen), UNFOCUSED_BORDER_COLOR, &unfocused_border, &dummyc);
  
  font = XLoadQueryFont(dpy, DEF_FONT);
  if (!font) { cerr << "The default font cannot be found, exiting..." << endl; exit(1); }
  
  shape = XShapeQueryExtension(dpy, &shape_event, &dummy);
  
  move_curs = XCreateFontCursor(dpy, XC_fleur);
  arrow_curs = XCreateFontCursor(dpy, XC_left_ptr);
  
	XDefineCursor(dpy, root, arrow_curs);
  
  gv.function = GXcopy;
  gv.foreground = fg.pixel;
  gv.font = font->fid;
  string_gc = XCreateGC(dpy, root, GCFunction|GCForeground|GCFont, &gv);
  
	gv.foreground = unfocused_border.pixel;
	gv.font = font->fid;
	unfocused_gc = XCreateGC(dpy, root, GCForeground|GCFont, &gv);
  
	gv.foreground = fg.pixel;
	gv.font = font->fid;
	focused_title_gc = XCreateGC(dpy, root, GCForeground|GCFont, &gv);
  
  gv.foreground = bd.pixel;
  gv.line_width = opt_bw;
  border_gc = XCreateGC(dpy, root, GCFunction|GCForeground|GCLineWidth, &gv);
  
	gv.foreground = fg.pixel;
  gv.function = GXinvert; 
  gv.subwindow_mode = IncludeInferiors;
  invert_gc = XCreateGC(dpy, root, GCForeground|GCFunction|GCSubwindowMode|GCLineWidth|GCFont, &gv);
	
	sattr.event_mask = SubstructureRedirectMask	|
  SubstructureNotifyMask  	|
  ColormapChangeMask		|
  ButtonPressMask		|
  ButtonReleaseMask		|
  FocusChangeMask		|
  EnterWindowMask		|
  LeaveWindowMask		|
  PropertyChangeMask   	|
  ButtonMotionMask		;
  
  XChangeWindowAttributes(dpy, root, CWEventMask, &sattr);
  
	grabKeys(root);
	
	window_menu = new WindowMenu(dpy);
	icon_menu = new IconMenu(dpy);  
}

void WindowManager::doEventLoop()
{
 	XEvent ev;
  
  for (;;) 
	{
		XNextEvent(dpy, &ev);
    
    switch (ev.type) 
		{		
    case KeyPress:
      handleKeyPressEvent(&ev);
			break;
			
    case ButtonPress:
      handleButtonPressEvent(&ev);
			break;
      
    case ButtonRelease:
      handleButtonReleaseEvent(&ev);
			break;
      
    case ConfigureRequest:
      handleConfigureRequestEvent(&ev);
			break;
      
    case MotionNotify:
      handleMotionNotifyEvent(&ev);
			break;
      
    case MapRequest:
      handleMapRequestEvent(&ev);
      break;
      
    case UnmapNotify:
      handleUnmapNotifyEvent(&ev);
      break;
	    
    case DestroyNotify:
      handleDestroyNotifyEvent(&ev);
			break;
      
    case EnterNotify:
      handleEnterNotifyEvent(&ev);
			break;
      
    case LeaveNotify:
      handleLeaveNotifyEvent(&ev);
			break;
      
    case FocusIn:
      handleFocusInEvent(&ev);
      break;
	    
    case FocusOut:
      handleFocusOutEvent(&ev);
      break;
      
    case ClientMessage:
      handleClientMessageEvent(&ev);
			break;
      
    case ColormapNotify:
      handleColormapNotifyEvent(&ev);
			break;
      
    case PropertyNotify:
      handlePropertyNotifyEvent(&ev);
			break;
      
    case Expose:
      handleExposeEvent(&ev);
			break;
      
    default:
      handleDefaultEvent(&ev);
      break;
    }
  }
}

void WindowManager::grabKeys(Window w)
{
	int max_desktop_keys=max_desktops;
	if (max_desktops>9)
	{
		max_desktop_keys=9;
	}
  for(int i=0;i<AEWM_KEY_ALT_COUNT;i++)
    XGrabKey(dpy, XkbKeycodeToKeysym(dpy, alt_keys[i], 0, 1), (Mod1Mask|ControlMask), w,True,GrabModeAsync,GrabModeAsync);

  for(int i=0;i<AEWM_KEY_ALT_COUNT+max_desktop_keys;i++)
		XGrabKey(dpy, XkbKeycodeToKeysym(dpy, alt_keys[i], 0, 1), (Mod1Mask),
    w,True,GrabModeAsync,GrabModeAsync);
}

void WindowManager::ungrabKeys(Window w)
{
	int max_desktop_keys=max_desktops;
	if (max_desktops>9)
	{
		max_desktop_keys=9;
	}
  for(int i=0;i<AEWM_KEY_ALT_COUNT;i++)
  XUngrabKey(dpy,XKeysymToKeycode(dpy,alt_keys[i]),
    (Mod1Mask|ControlMask),w);
  for(int i=0;i<AEWM_KEY_ALT_COUNT+max_desktop_keys;i++)
		XUngrabKey(dpy,XKeysymToKeycode(dpy,alt_keys[i]), (Mod1Mask),w);
}

void WindowManager::handleKeyPressEvent(XEvent *ev) 
{
	KeySym ks;
  
	ks=XkbKeycodeToKeysym(dpy,ev->xkey.keycode,0, 1);
	if (ks==NoSymbol) return;
  
	switch(ks) 
	{
  case XK_Delete:
    cerr << WINDOW_MANAGER_NAME << " is restarting..." << endl;
    restart();
		break;
    
  case XK_End:
    cerr << WINDOW_MANAGER_NAME << " is quitting." << endl;
    quitNicely();
		break;
    
  case XK_Page_Up: 
    if( current_desktop < max_desktops - 1 )
    {
      current_desktop++;
      goToDesktop(current_desktop);
    }
		break;
		
  case XK_Page_Down: 
    if( current_desktop > 0 )
    {
      current_desktop--;
      goToDesktop(current_desktop);
    }			
		break;
	}
	//if (ks >= XK_1 && ks <= XK_1+max_desktops && ks - XK_1 <= 9)  /* no two digit keys */
 	//{
  //        if( current_desktop != ks - XK_1 )
	//	{
	//		current_desktop = ks - XK_1;
	//		goToDesktop(current_desktop);
	//	}
	//}
}

void WindowManager::handleButtonPressEvent(XEvent *ev)
{
	if (ev->xbutton.window == root) 
	{
		switch (ev->xbutton.button) 
		{
    case Button1: 
      if(icon_menu->isVisible())
        icon_menu->hide();
			break;
      
    case Button2: 
      if(icon_menu->getItemCount())
      {
        if(icon_menu->isVisible())
          icon_menu->hide();
        else 
          icon_menu->show();
      }
			break;
      
    case Button3: 
      forkExec(opt_new1); 
      
      if(icon_menu->isVisible())
        icon_menu->hide();
			break;
    }
	}
	else
	{
		Client* c = findClient(ev->xbutton.window);
		
		if(c && c->hasWindowDecorations())
		{
			if( (ev->xbutton.button == Button1) && 
        (ev->xbutton.type==ButtonPress) && 
      (ev->xbutton.state==Mod1Mask) &&
      (c->getFrameWindow() == ev->xbutton.window)
      )
      if(XGrabPointer(dpy, c->getFrameWindow(), False, 
        PointerMotionMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, 
        None, wm->getMoveCursor(), CurrentTime) != GrabSuccess) return; 
		} 
		
		switch (focus_model) 
		{
    case FOCUS_FOLLOW:
    case FOCUS_SLOPPY:
      if(c)
      {
        c->handleButtonEvent(&ev->xbutton);
        focused_client = c;
      }
			break;
      
    case FOCUS_CLICK:
      // if this is the first time the client window's clicked, focus it
      if(c && c != focused_client)
      {
        XSetInputFocus(dpy, c->getAppWindow(), RevertToNone, CurrentTime);
        focused_client = c;
      }
      
      // otherwise, handle the button click as usual
      if(c && c == focused_client)
        c->handleButtonEvent(&ev->xbutton);
			break;
		}
    
		BaseMenu* mu = window_menu->findMenu(ev->xbutton.window);
    
		if(!mu)
			mu = icon_menu->findMenu(ev->xbutton.window);
    
		if(mu)
			mu->handleButtonPressEvent(&ev->xbutton);
	}
  
	//if(ev->xbutton.window==root)
	//	XSendEvent(dpy, _button_proxy_win, False, SubstructureNotifyMask, ev);
  
}

void WindowManager::handleButtonReleaseEvent(XEvent *ev)
{
	Client* c = findClient(ev->xbutton.window);
  
	if(c) {
		XUngrabPointer(dpy, CurrentTime);
		
		c->handleButtonEvent(&ev->xbutton); 
	}
	else 
	{
		BaseMenu* mu = window_menu->findMenu(ev->xbutton.window);
    
		if(!mu)
			mu = icon_menu->findMenu(ev->xbutton.window);
    
		if(mu) 
		{
			mu->hide();
			mu->handleButtonReleaseEvent(&ev->xbutton);
		}
	}
  
	//if(ev->xbutton.window==root)
	//	XSendEvent(dpy, _button_proxy_win, False, SubstructureNotifyMask, ev);
}

void WindowManager::handleConfigureRequestEvent(XEvent *ev)
{
	Client* c = findClient(ev->xconfigurerequest.window);
  
	if(c)
		c->handleConfigureRequest(&ev->xconfigurerequest); 
	else 
	{
		// Since this window isn't yet a client lets delegate
		// the configure request back to the window so it can
		// use it.
    
		XWindowChanges wc;	
    
    wc.x = ev->xconfigurerequest.x;
		wc.y = ev->xconfigurerequest.y;
		wc.width = ev->xconfigurerequest.width;
		wc.height = ev->xconfigurerequest.height;
    wc.sibling = ev->xconfigurerequest.above;
    wc.stack_mode = ev->xconfigurerequest.detail;
		XConfigureWindow(dpy, ev->xconfigurerequest.window, ev->xconfigurerequest.value_mask, &wc);
	}
}

void WindowManager::handleMotionNotifyEvent(XEvent *ev)
{
	Client* c = findClient(ev->xmotion.window);
  
	if(c)
		c->handleMotionNotifyEvent(&ev->xmotion); 
	else
	{
		BaseMenu* mu = window_menu->findMenu(ev->xmotion.window);
		
		if(!mu)
			mu = icon_menu->findMenu(ev->xmotion.window);
    
		if(mu)
			mu->handleMotionNotifyEvent(&ev->xmotion);
	}
}

void WindowManager::handleMapRequestEvent(XEvent *ev)
{
	Client* c = findClient(ev->xmaprequest.window);
  
	if(c) 
		c->handleMapRequest(&ev->xmaprequest);
	else {
		client_window_list.push_back(ev->xmaprequest.window);
		c = new Client(dpy, ev->xmaprequest.window);
    
		updateIconMenu();
	}
}

void WindowManager::handleUnmapNotifyEvent(XEvent *ev)
{
	Client* c = findClient(ev->xunmap.window);
	
	if(c) 
	{
		c->handleUnmapEvent(&ev->xunmap); 
		// if unmapping it, note that it's no longer focused
		focused_client = NULL;
	}
}

void WindowManager::handleDestroyNotifyEvent(XEvent *ev)
{
	Client* c = findClient(ev->xdestroywindow.window);
  
	if(c)
	{
		c->handleDestroyEvent(&ev->xdestroywindow);
		// if destroying it, note that it's no longer focused
		focused_client = NULL;
	}     
	
	XSendEvent(dpy, _button_proxy_win, False, SubstructureNotifyMask, ev);
}

void WindowManager::handleEnterNotifyEvent(XEvent *ev)
{
	BaseMenu* mu = window_menu->findMenu(ev->xcrossing.window);
  
	if(!mu)
		mu = icon_menu->findMenu(ev->xcrossing.window);			
  
	if(mu) 
		mu->handleEnterNotify(&ev->xcrossing);	
	else
	{
		Client* c = findClient(ev->xcrossing.window);
		
		switch (focus_model) 
		{
    case FOCUS_FOLLOW:
      if(c)
      {
        c->handleEnterEvent(&ev->xcrossing);
        focused_client = c;
      }
      else
        XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
			break;
      
    case FOCUS_SLOPPY:
      // if the pointer's not on a client now, don't change focus
      if(c)
      {
        c->handleEnterEvent(&ev->xcrossing);
        focused_client = c;
      }
			break;
		}
	}
}

void WindowManager::handleLeaveNotifyEvent(XEvent *ev)
{
	BaseMenu* mu = window_menu->findMenu(ev->xcrossing.window);
  
	if(!mu)
		mu = icon_menu->findMenu(ev->xcrossing.window);			
  
	if(mu) mu->handleLeaveNotify(&ev->xcrossing);	
}

void WindowManager::handleFocusInEvent(XEvent *ev)
{
	if((ev->xfocus.mode==NotifyGrab) || (ev->xfocus.mode==NotifyUngrab)) return;
	
	list<Window>::iterator iter;
	
	for(iter=client_window_list.begin(); iter != client_window_list.end(); iter++)
	{
		if(ev->xfocus.window == (*iter))
		{
			Client *c = findClient( (*iter) );
			
			if(c) 
			{
				unfocusAnyStrayClients();
				c->handleFocusInEvent(&ev->xfocus);
				focused_client = c;
				grabKeys( (*iter) );
			}
		}
		else 
		{
			if(ev->xfocus.window==root && focus_model==FOCUS_FOLLOW)
				unfocusAnyStrayClients();
		}
	}
}

void WindowManager::handleFocusOutEvent(XEvent *ev)
{	
	list<Window>::iterator iter;
	for(iter=client_window_list.begin(); iter != client_window_list.end(); iter++)
	{
		if(ev->xfocus.window == (*iter))
		{
			Client *c = findClient( (*iter) );
			
			if(c) 
			{
				focused_client = NULL;
				ungrabKeys( (*iter) );
				return;
			}
		}
	}
  
	if(focus_model == FOCUS_CLICK) 
		focusPreviousWindowInStackingOrder();
  else if(focus_model == FOCUS_SLOPPY && client_list.size())
  {
    unsigned int nwins;
    Window dummyw1, dummyw2, *wins;
    Client *c=NULL;
    
    XQueryTree(dpy, root, &dummyw1, &dummyw2, &wins, &nwins);
    for (unsigned int i = 0; i < nwins; i++)
    {
      c = findClient(wins[i]);
      
      if(c)
      {
        if(c->belongsToWhichDesktop()==current_desktop) 
				{
					focusPreviousWindowInStackingOrder();
          return;
				}
      }
    }
    XSetInputFocus(dpy, PointerRoot, RevertToNone, CurrentTime);
  }
  
}

void WindowManager::handleClientMessageEvent(XEvent *ev)
{
	Client* c = findClient(ev->xclient.window);
  
	if(c)
		c->handleClientMessage(&ev->xclient);
}

void WindowManager::handleColormapNotifyEvent(XEvent *ev)
{
	Client* c = findClient(ev->xcolormap.window);
  
	if(c)
		c->handleColormapChange(&ev->xcolormap); 
}

void WindowManager::handlePropertyNotifyEvent(XEvent *ev)
{
	Client* c = findClient(ev->xproperty.window);
  
	if(c)
		c->handlePropertyChange(&ev->xproperty); 
}

void WindowManager::handleExposeEvent(XEvent *ev)
{
	BaseMenu* mu = window_menu->findMenu(ev->xexpose.window);
  
	if(!mu)
		mu = icon_menu->findMenu(ev->xexpose.window);					
  
	if(mu) 
		mu->handleExposeEvent(&ev->xexpose); 
	else
	{
		Client* c = findClient(ev->xexpose.window);
		
		if(c)
			c->handleExposeEvent(&ev->xexpose);
	}
}

void WindowManager::handleDefaultEvent(XEvent *ev)
{
	Client* c = findClient(ev->xany.window);
  
	if(c)
	{	
		if (shape && ev->type == shape_event)
			// ev was &ev
    c->handleShapeChange((XShapeEvent *)ev);
	} 
}

void WindowManager::unfocusAnyStrayClients()
{
	// To prevent two windows titlebars from being painted with the focus color we
	// will prevent that from happening by setting all windows to false.
	
	list<Client*>::iterator iter;
	for(iter=client_list.begin(); iter != client_list.end(); iter++)
		(*iter)->setFocus(false);
}

void WindowManager::focusPreviousWindowInStackingOrder()
{
	unsigned int nwins, i;
	Window dummyw1, dummyw2, *wins;
	Client* c=NULL;
  
	XSetInputFocus(dpy, _button_proxy_win, RevertToNone, CurrentTime);
  
	XQueryTree(dpy, root, &dummyw1, &dummyw2, &wins, &nwins);
  
	if(client_list.size())
	{
		list<Client*> client_list_for_current_desktop;
    
		for (i = 0; i < nwins; i++)
		{
			c = findClient(wins[i]);
			
			if(c)
			{
				if((c->belongsToWhichDesktop()==current_desktop 
          && c->hasWindowDecorations() && (c->isIconified() == false) ))
				client_list_for_current_desktop.push_back(c);
			}
		}
		
		if(client_list_for_current_desktop.size())
		{
			list<Client*>::iterator iter = client_list_for_current_desktop.end();
			
			iter--;
			
			if( (*iter) )
			{
				XSetInputFocus(dpy, (*iter)->getAppWindow(), RevertToNone, CurrentTime);
        
				client_list_for_current_desktop.clear();
				
				XFree(wins);
        
				return;
			}
		} 
	} 
	
	XFree(wins);
}

void WindowManager::getMousePosition(int *x, int *y)
{
	Window mouse_root, mouse_win;
	int win_x, win_y;
	unsigned int mask;
  
	XQueryPointer(dpy, root, &mouse_root, &mouse_win, x, y, &win_x, &win_y, &mask);
}

void WindowManager::addClient(Client *c)
{
	client_list.push_back(c);
}

void WindowManager::removeClient(Client* c)
{
	removeClientFromIconMenu(c);
	client_window_list.remove(c->getAppWindow());
	client_list.remove(c);	
}

Client* WindowManager::findClient(Window w)
{
	if(client_list.size())
	{
		list<Client*>::iterator iter = client_list.begin();
    
		for(; iter != client_list.end(); iter++)
		{
			if (w == (*iter)->getTitleWindow()  ||
        w == (*iter)->getFrameWindow()  ||
      w == (*iter)->getAppWindow())
      return (*iter);
		}
	}
	return NULL;
}

void WindowManager::findTransientsToMapOrUnmap(Window win, bool hide)
{
	list<Client*>::iterator iter;
	
	if(client_list.size()) 
	{
		for(iter=client_list.begin(); iter!= client_list.end(); iter++)
		{
			if((*iter)->getTransientWindow() == win)
			{
				if(hide)
				{
					if(! (*iter)->isIconified())
						(*iter)->iconify(); 
				}
				else 
				{
					if((*iter)->isIconified())
						(*iter)->unhide();
				}
			}
		}
	}	
}

void WindowManager::restart()
{
	cleanup();
	
	execl("/bin/sh", "sh", "-c", command_line.c_str(), (char *)NULL);
}

void WindowManager::quitNicely()
{
	cleanup();
	exit(0);
}

void WindowManager::cleanup()
{
  cerr << WINDOW_MANAGER_NAME << " is cleaning up.... " << endl;
	
	unsigned int nwins, i;
  Window dummyw1, dummyw2, *wins;
  Client* c;
  
	XDestroyWindow(dpy, _button_proxy_win);
  
	ungrabKeys(root);
  
  // Preserve stacking order when removing the clients
  // from the list.
  XQueryTree(dpy, root, &dummyw1, &dummyw2, &wins, &nwins);
  for (i = 0; i < nwins; i++) 
  {
		c = findClient(wins[i]);
		
		if(c)
		{
			XMapWindow(dpy, c->getAppWindow());
			
			delete c;
		}
  }
  XFree(wins);
  
  delete window_menu;
	delete icon_menu;
  
  XFreeFont(dpy, font);
  
	XFreeCursor(dpy, move_curs);
	XFreeCursor(dpy, arrow_curs);
	
	XFreeGC(dpy, invert_gc);
  XFreeGC(dpy, border_gc);
  XFreeGC(dpy, string_gc);
	XFreeGC(dpy, unfocused_gc);
	XFreeGC(dpy, focused_title_gc);	
  
	XInstallColormap(dpy, DefaultColormap(dpy, screen));
  XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
  XCloseDisplay(dpy);
}

/* If we can't find a wm->wm_state we're going to have to assume
* Withdrawn. This is not exactly optimal, since we can't really
* distinguish between the case where no WM has run yet and when the
* state was explicitly removed (Clients are allowed to either set the
* atom to Withdrawn or just remove it... yuck.) */
long WindowManager::getWMState(Window window)
{
  Atom real_type; int real_format;
  unsigned long items_read, items_left;
  unsigned char *data, state = WithdrawnState;
  
  if (XGetWindowProperty(dpy, window, atom_wm_state, 0L, 2L, False,
    wm->atom_wm_state, &real_type, &real_format, &items_read, &items_left,
    &data) == Success && items_read) {
  state = *data;
  XFree(data);
    }
    
    return state;
}

/* Attempt to follow the ICCCM by explicity specifying 32 bits for
* this property. Does this goof up on 64 bit systems? */
void WindowManager::setWMState(Window window, int state)
{
  CARD32 data[2];
  
  data[0] = state;
  data[1] = None; // Icon? We don't need no steenking icon. 
  
  XChangeProperty(dpy, window, atom_wm_state, atom_wm_state,
    32, PropModeReplace, (unsigned char *)data, 2);
}

// The name of this function is a bit misleading: if the client
// doesn't listen to WM_DELETE then we just terminate it with extreme
// prejudice. 
void WindowManager::sendWMDelete(Window window)
{
  int i, n, found = 0;
  Atom *protocols;
  
  if (XGetWMProtocols(dpy, window, &protocols, &n)) {
    for (i=0; i<n; i++) if (protocols[i] == atom_wm_delete) found++;
    XFree(protocols);
  }
  if (found) 
		sendXMessage(window, atom_wm_protos, NoEventMask, atom_wm_delete);
  else XKillClient(dpy, window);
}

// Currently, only sendWMDelete uses this one...
int WindowManager::sendXMessage(Window w, Atom a, long mask, long x)
{
  XEvent e;
  
  e.type = ClientMessage;
  e.xclient.window = w;
  e.xclient.message_type = a;
  e.xclient.format = 32;
  e.xclient.data.l[0] = x;
  e.xclient.data.l[1] = CurrentTime;
  
  return XSendEvent(dpy, w, False, mask, &e);
}

void WindowManager::sigHandler(int signal)
{
  switch (signal) {
  case SIGINT:
  case SIGTERM:
		wm->quitNicely(); 
    break;
    
  case SIGHUP:
		wm->restart(); 
    break;
    
	case SIGCHLD:
    wait(NULL); 
    break;
  }
}
