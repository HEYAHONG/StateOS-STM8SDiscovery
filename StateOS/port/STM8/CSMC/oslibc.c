/******************************************************************************

    @file    StateOS: oslibc.c
    @author  Rajmund Szymanski
    @date    13.07.2017
    @brief   This file provides set of variables and functions for StateOS.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#if defined(__CSMC__)

#include <oskernel.h>

/* -------------------------------------------------------------------------- */

void *sbreak( int size )
{
	extern char  _startmem[];
	extern char  _endmem[];
	static char *_brk = _startmem;
	       char * brk = NULL;

	port_sys_lock();

	if (_brk + size < _endmem)
	{
		 brk  = _brk;
		_brk += size;
	}

	port_sys_unlock();

	return brk;
}

/* -------------------------------------------------------------------------- */

#endif // __CSMC__
