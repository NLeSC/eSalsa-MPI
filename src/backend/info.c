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
#include "info.h"

static info *infos[MAX_INFOS];
static int next_info = 0;

int init_infos()
{
   int i;

   for (i=0;i<MAX_INFOS;i++) {
      infos[i] = NULL;
   }

   return EMPI_SUCCESS;
}

info *handle_to_info(EMPI_Info handle)
{
   if (handle < 0 || handle >= MAX_INFOS) {
      ERROR(1, "handle_to_info(handle=%d) Handle out of bounds!\n", handle);
      return NULL;
   }

   return infos[handle];
}

EMPI_Info info_to_handle(info *i)
{
   if (i == NULL) {
      ERROR(1, "info_to_handle(i=NULL) Info is NULL!\n");
      return EMPI_INFO_NULL;
   }

   return i->handle;
}

info *add_info(MPI_Info i)
{
   int handle, j;

   if (i == MPI_INFO_NULL) {
      return NULL;
   }

   if (next_info < MAX_INFOS) {
      handle = next_info;
   } else {
      handle = EMPI_INFO_NULL;

      for (j=0;j<MAX_INFOS;j++) {
         if (infos[j] == NULL) {
            handle = j;
            break;
         }
      }

      if (handle == EMPI_INFO_NULL) {
         IERROR(1, "Failed to initialize info -- all handles in use!\n");
         return NULL;
      }
   }

   infos[handle] = malloc(sizeof(info));

   if (infos[handle] == NULL) {
      IERROR(1, "Failed to initialize info -- cannot allocate handle %d!\n", handle);
      return NULL;
   }

   infos[handle]->handle = handle;
   infos[handle]->info = i;

   if (handle >= next_info) {
      next_info = next_info+1;
   }

   return infos[handle];
}


void remove_info(EMPI_Info i)
{
   if (i == EMPI_FILE_NULL) {
      return;
   }

   if (i < 0 || i >= MAX_INFOS) {
      IERROR(1, "Failed to remove info handle -- handle %d out of bounds!\n", i);
      return;
   }

   if (infos[i] == NULL) {
      IERROR(1, "Failed to remove info handle -- handle %d not in use!\n", i);
      return;
   }

   free(infos[i]);
   infos[i] = NULL;

   // TODO: reset next_info if possible!
}
