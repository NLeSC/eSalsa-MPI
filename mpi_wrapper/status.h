#ifndef _STATUS_H_
#define _STATUS_H_

struct s_status {
  int MPI_SOURCE;
  int MPI_TAG;
  int MPI_ERROR;
  int count;
  int cancelled;
  int ignore;
};

int clear_status(EMPI_Status *s);


#endif // _STATUS_H_
