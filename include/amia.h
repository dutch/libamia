#ifndef LIBAMIA_AMIA_HEADER
#define LIBAMIA_AMIA_HEADER

/* Copyright (C) 2021 Chris Lamberson
   This file is part of libamia.

   libamia is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   libamia is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with libamia.  If not, see
   <https://www.gnu.org/licenses/>. */

#include <sys/types.h>

#define AMIA_EOK      0
#define AMIA_EUNKNOWN 1

int daemonize(const char *path);

#endif
