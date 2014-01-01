# aewm++ - A small C++ window manager developed from aewm 0.9.6 around 2000 
# Copyright (C) 2014 Frank Hale <frankhale@gmail.com>
#
# aewm++ can be found here: http://code.google.com/p/aewmpp/
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Date: 1 January 2014
CC       = clang++
ADDITIONAL_CFLAGS   = -g -O2 -Wall

prefix   = /usr
INCLUDES = -I$/usr/X11R6
LDPATH   = -L/usr/X11R6/lib
LIBS     = -lXext -lX11

HEADERS  = aewm.h 		\
	   client.h 		\
	   windowmanager.h  	\
   	   basemenu.h  	\
	   windowmenu.h	\
	   genericmenu.h 	\
	   iconmenu.h  	

OBJS     = windowmanager.o 	\
	   client.o 		\
	   main.o 		\
	   basemenu.o   	\
	   genericmenu.o	\
	   iconmenu.o   	\
	   windowmenu.o 	

all: aewm++

aewm++: $(OBJS)
	$(CC) $(OBJS) $(LDPATH) $(LIBS) -o $@

$(OBJS): %.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) $(ADDITIONAL_CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(prefix)/bin
	install -s aewm++ $(DESTDIR)$(prefix)/bin
	
clean:
	rm -f aewm++ $(OBJS) core
