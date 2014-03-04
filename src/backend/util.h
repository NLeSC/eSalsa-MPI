#ifndef _UTIL_H_
#define _UTIL_H_

#define DELETE_ARG { for (cnt=i; cnt<(*argc)-1; cnt++){ \
                       (*argv)[cnt] = (*argv)[cnt+1]; \
                     }\
                     changed = 1;\
                     (*argc)--;\
                   }

void init_fortran_logical_(int *, int*);

#endif

