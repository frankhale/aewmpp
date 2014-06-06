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

#ifndef _AEWM_H_
#define _AEWM_H_

#define WINDOW_MANAGER_NAME "aewm++"
#define VERSION "xxx"
#define DATE "29 December 2008"

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <iostream> 
#include <list>

#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>
#include <X11/extensions/shape.h>

using namespace std;

#define DEF_FONT "Fixed"
#define DEF_FG	"#ffffff"
#define DEF_BG	"#999999" 
#define DEF_FC	"#dddddd"
#define DEF_BD	"#000000"

#define FOCUSED_BORDER_COLOR "#000000"
#define UNFOCUSED_BORDER_COLOR "#888888"
#define FOCUSED_WINDOW_TITLE_COLOR "#FFFFFF"

#define DEF_NEW1	"xterm -ls -sb -bg black -fg white"
#define DEF_BW		1
#define SPACE		3
#define MINSIZE		15
#define EDGE_SNAP	"true"
#define SNAP		5
#define TEXT_JUSTIFY	"right"
#define WIRE_MOVE	"false"
#define MAX_DESKTOPS	4
#define DEF_FM		"click"
#define DEF_WP          "mouse"

#define TRANSIENT_WINDOW_HEIGHT 8

enum { LEFT_JUSTIFY, CENTER_JUSTIFY, RIGHT_JUSTIFY };
enum { FOCUS_FOLLOW, FOCUS_SLOPPY, FOCUS_CLICK };
enum { APPLY_GRAVITY=1, REMOVE_GRAVITY=-1 };
enum { PIXELS=0, INCREMENTS=1 };

// Border width accessor to handle hints/no hints
#define BW (has_border ? wm->getOptBW() : 0)

// defined in main.cc
void forkExec(char *);
int handleXError(Display *, XErrorEvent *);

class Client;
class BaseMenu;

#include "basemenu.h"
#include "genericmenu.h"
#include "windowmenu.h"
#include "iconmenu.h"
#include "client.h"
#include "windowmanager.h"

#endif // _AEWM_H_ 
