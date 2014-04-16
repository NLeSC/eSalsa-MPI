#ifndef _GATEWAY_H_
#define _GATEWAY_H_

int master_gateway_init(int rank, int size, int *argc, char ***argv);
int generic_gateway_init(int rank, int size);
int run_gateway();

#endif // _GATEWAY_H_

