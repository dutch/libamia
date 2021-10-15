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

#include <amia.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

static int
secondchild(void)
{
  umask(0);

  if (chdir("/") == -1)
    return -errno;

  /* TODO: Write the PID here. */

  /* TODO: Drop privileges here. */

  return 0;
}

static int
firstchild(void)
{
  if (setsid() == -1)
    return -errno;

  if (!fork())
    return secondchild();

  exit(EXIT_SUCCESS);
}

int
daemonize(char *s, size_t slen)
{
  int sig;
  sigset_t sigs;
  struct rlimit rlim;
  struct sigaction sa;

  if (getrlimit(RLIMIT_NOFILE, &rlim) == -1) {
    memcpy(s, "getrlimit", 10 <= slen ? 10 : slen);
    return -errno;
  }

  while (rlim.rlim_max --> 3)
    close(rlim.rlim_max);

  if (sigfillset(&sigs) == -1) {
    memcpy(s, "sigfillset", 11 <= slen ? 11 : slen);
    return -errno;
  }

  for (sig = 1; sig <= SIGRTMAX; ++sig) {
    if (sig == SIGKILL || sig == SIGSTOP)
      continue;

    if (!sigismember(&sigs, sig))
      continue;

    sa.sa_handler = SIG_DFL;

    if (sigemptyset(&sa.sa_mask) == -1) {
      memcpy(s, "sigemptyset", 12 <= slen ? 12 : slen);
      return -errno;
    }

    if (sigaction(sig, &sa, NULL) == -1) {
      memcpy(s, "sigaction", 10 <= slen ? 10 : slen);
      return -errno;
    }
  }

  if (sigprocmask(SIG_UNBLOCK, &sigs, NULL) == -1) {
    memcpy(s, "sigprocmask", 12 <= slen ? 12 : slen);
    return -errno;
  }

  /* TODO: Sanitize the environment block here. */

  switch (fork()) {
  case -1:
    memcpy(s, "fork", 5 <= slen ? 5 : slen);
    return -errno;

  case 0:
    return firstchild();
  }

  sleep(1);
  exit(EXIT_SUCCESS);
}
