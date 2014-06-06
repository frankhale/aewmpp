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


 #include "aewm.h"

// Dunno where I ripped this from. Kudos to the author whoever he is!
void forkExec(char *cmd)
{
    if(! (strlen(cmd)>0)) return;
    
    pid_t pid = fork();

    switch (pid) {
        case 0:
            execlp("/bin/sh", "sh", "-c", cmd, NULL);
	    cerr << "exec failed, cleaning up child" << endl;
            exit(1);
        case -1:
	    cerr << "can't fork" << endl;
    }
}

int handleXError(Display *dpy, XErrorEvent *e)
{
    if (e->error_code == BadAccess && e->resourceid == RootWindow(dpy, DefaultScreen(dpy)) ) {
        cerr << "root window unavailable (maybe another wm is running?)" << endl;
        exit(1);
    }
   
    return 0;
}

int main(int argc, char **argv)
{
	WindowManager aewm(argc, argv);
	return 0;
}
