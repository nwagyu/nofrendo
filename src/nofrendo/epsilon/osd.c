/* vim: set tabstop=3 expandtab:
**
** This file is in the public domain.
**
** osd.c
**
** $Id: osd.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <noftypes.h>
#include <nofconfig.h>
#include <log.h>
#include <osd.h>
#include <nofrendo.h>

#include <version.h>

char configfilename[]="na";


/* This is os-specific part of main() */
int osd_main(int argc, char *argv[])
{
  config.filename = configfilename;

  printf("Running main loop for %s\n", argv[1]);
  return main_loop(argv[1], system_autodetect);
}

/* File system interface */
void osd_fullname(char *fullname, const char *shortname)
{
   strncpy(fullname, shortname, PATH_MAX);
}

/* This gives filenames for storage of saves */
char *osd_newextension(char *string, char *ext)
{
    int l=strlen(string);
    while(l && string[l]!='.') {
        l--;
    }
    if (l) string[l]=0;
    strcat(string, ext);
    return string;
}

/* This gives filenames for storage of PCX snapshots */
int osd_makesnapname(char *filename, int len)
{
   return -1;
}
