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

#define _POSIX_C_SOURCE 200809L

#include <amia.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>

static int
secondchild(int outdes, int errdes, const char *path)
{
  int nullfd, fd, pidfd;
  pid_t pid;
  struct flock l;

  if ((nullfd = open("/dev/null", O_RDWR)) == -1) {
    write(errdes, " ", 1);
    exit(EXIT_FAILURE);
  }

  for (fd = 0; fd <= 2; ++fd) {
    if (dup2(nullfd, fd) == -1) {
      write(errdes, " ", 1);
      exit(EXIT_FAILURE);
    }
  }

  close(nullfd);

  umask(0);

  if (chdir("/") == -1) {
    write(errdes, " ", 1);
    exit(EXIT_FAILURE);
  }

  if ((pidfd = open(path, O_RDWR | O_EXCL | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR)) == -1) {
    write(errdes, " ", 1);
    exit(EXIT_FAILURE);
  }

  pid = getpid();

  memset(&l, 0, sizeof(struct flock));
  l.l_type = F_WRLCK;
  l.l_whence = SEEK_SET;
  l.l_pid = pid;

  if (fcntl(pidfd, F_SETLK, &l) == -1) {
    write(errdes, " ", 1);
    exit(EXIT_FAILURE);
  }

  dprintf(pidfd, "%ld\n", pid);

  l.l_type = F_UNLCK;

  if (fcntl(pidfd, F_SETLK, &l) == -1) {
    write(errdes, " ", 1);
    exit(EXIT_FAILURE);
  }

  /* TODO: Drop privileges here. */

  write(outdes, " ", 1);

  return AMIA_EOK;
}

static int
firstchild(int outdes, int errdes, const char *path)
{
  if (setsid() == -1) {
    write(errdes, " ", 1);
    exit(EXIT_FAILURE);
  }

  switch (fork()) {
  case -1:
    write(errdes, " ", 1);
    exit(EXIT_FAILURE);

  case 0:
    return secondchild(outdes, errdes, path);
  }

  exit(EXIT_SUCCESS);
}

int
daemonize(const char *path)
{
  char ch;
  int sig, outpipe[2], errpipe[2];
  sigset_t sigs;
  struct rlimit rlim;
  struct sigaction sa;
  fd_set readfds;

  if (getrlimit(RLIMIT_NOFILE, &rlim) == -1)
    return AMIA_EUNKNOWN;

  while (rlim.rlim_max --> 3)
    close(rlim.rlim_max);

  if (sigfillset(&sigs) == -1)
    return AMIA_EUNKNOWN;

  for (sig = 1; sig <= SIGRTMAX; ++sig) {
    if (sig == SIGKILL || sig == SIGSTOP)
      continue;

    if (!sigismember(&sigs, sig))
      continue;

    sa.sa_handler = SIG_DFL;

    if (sigemptyset(&sa.sa_mask) == -1)
      return AMIA_EUNKNOWN;

    if (sigaction(sig, &sa, NULL) == -1)
      return AMIA_EUNKNOWN;
  }

  if (sigprocmask(SIG_UNBLOCK, &sigs, NULL) == -1)
    return AMIA_EUNKNOWN;

  /* TODO: Sanitize the environment block here. */

  pipe(outpipe);
  pipe(errpipe);

  switch (fork()) {
  case -1:
    return AMIA_EUNKNOWN;

  case 0:
    close(outpipe[0]);
    close(errpipe[0]);
    return firstchild(outpipe[1], errpipe[1], path);
  }

  for (;;) {
    FD_ZERO(&readfds);
    FD_SET(outpipe[0], &readfds);
    FD_SET(errpipe[0], &readfds);

    if (select(errpipe[0] + 1, &readfds, NULL, NULL, NULL) == -1) {
      if (errno == EINTR)
        continue;

      return AMIA_EUNKNOWN;
    }

    if (FD_ISSET(errpipe[0], &readfds))
      return AMIA_EUNKNOWN;

    if (FD_ISSET(outpipe[0], &readfds))
      exit(EXIT_SUCCESS);
  }
}
