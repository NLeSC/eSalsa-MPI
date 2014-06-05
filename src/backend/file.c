#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "settings.h"

#include "shared.h"
#include "types.h"
#include "logging.h"
#include "file.h"

static file *files[MAX_FILES];
static int next_file = 0;

int init_files()
{
   int i;

   for (i=0;i<MAX_FILES;i++) {
      files[i] = NULL;
   }

   return EMPI_SUCCESS;
}

file *handle_to_file(EMPI_File handle)
{
   if (handle < 0 || handle >= MAX_FILES) {
      ERROR(1, "handle_to_file(handle=%d) Handle out of bounds!\n", handle);
      return NULL;
   }

   return files[handle];
}

EMPI_File file_to_handle(file *f)
{
   if (f == NULL) {
      ERROR(1, "file_to_handle(f=NULL) File is NULL!\n");
      return -1;
   }

   return f->handle;
}

file *add_file(communicator *c, MPI_File f)
{
   int handle, i;

   if (f == MPI_FILE_NULL) {
      return NULL;
   }

   if (next_file < MAX_FILES) {
      handle = next_file;
   } else {
      handle = -1;

      for (i=0;i<MAX_FILES;i++) {
         if (files[i] == NULL) {
            handle = i;
            break;
         }
      }

      if (handle == -1) {
         IERROR(1, "Failed to initialize file -- all file handles in use!\n");
         return NULL;
      }
   }

   files[handle] = malloc(sizeof(file));

   if (files[handle] == NULL) {
      IERROR(1, "Failed to initialize file -- cannot allocate file structure %d!\n", handle);
      return NULL;
   }

   files[handle]->handle = handle;
   files[handle]->comm = c;
   files[handle]->file = f;

   if (handle >= next_file) {
      next_file = next_file+1;
   }

   return files[handle];
}


void remove_file(EMPI_File f)
{
   if (f == EMPI_FILE_NULL) {
      return;
   }

   if (f < 0 || f >= MAX_FILES) {
      IERROR(1, "Failed to remove file handle -- handle %d out of bounds!\n", f);
      return;
   }

   if (files[f] == NULL) {
      IERROR(1, "Failed to remove file handle -- handle %d not in use!\n", f);
      return;
   }

   free(files[f]);
   files[f] = NULL;
}
