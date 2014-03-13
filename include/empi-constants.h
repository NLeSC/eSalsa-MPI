#ifndef __EMPI_CONST_H_
#define __EMPI_CONST_H_

/* BASIC C Datatypes -- as defined in MPI 2.1 standard. */

#define EMPI_CONST_DATATYPE_NULL         0
#define EMPI_CONST_CHAR                  1
#define EMPI_CONST_SHORT                 2
#define EMPI_CONST_INT                   3
#define EMPI_CONST_LONG                  4
#define EMPI_CONST_LONG_LONG_INT         5
#define EMPI_CONST_LONG_LONG             EMPI_CONST_LONG_LONG_INT // Official synonym since MPI 2.1

#define EMPI_CONST_SIGNED_CHAR           6
#define EMPI_CONST_UNSIGNED_CHAR         7
#define EMPI_CONST_UNSIGNED_SHORT        8
#define EMPI_CONST_UNSIGNED              9
#define EMPI_CONST_UNSIGNED_LONG         10
#define EMPI_CONST_UNSIGNED_LONG_LONG    11

#define EMPI_CONST_FLOAT                 12
#define EMPI_CONST_DOUBLE                13
#define EMPI_CONST_LONG_DOUBLE           14

#define EMPI_CONST_WCHAR                 15

#define EMPI_CONST_BYTE                  16
#define EMPI_CONST_PACKED                17

/* Special datatypes for C reduction operations -- as defined in MPI 2.1 standard. */

#define EMPI_CONST_FLOAT_INT             18
#define EMPI_CONST_DOUBLE_INT            19
#define EMPI_CONST_LONG_INT              20
#define EMPI_CONST_2INT                  21
#define EMPI_CONST_SHORT_INT             22
#define EMPI_CONST_LONG_DOUBLE_INT       23

/* BASIC Fortran Datatypes -- as defined in MPI 2.1 standard. */

#define EMPI_CONST_INTEGER               24
#define EMPI_CONST_REAL                  25
#define EMPI_CONST_DOUBLE_PRECISION      26
#define EMPI_CONST_COMPLEX               27
#define EMPI_CONST_LOGICAL               28
#define EMPI_CONST_CHARACTER             29

/* Optional Fortran Datatypes -- as defined in MPI 2.1 standard. */

#define EMPI_CONST_DOUBLE_COMPLEX        30

#define EMPI_CONST_INTEGER1              31
#define EMPI_CONST_INTEGER2              32
#define EMPI_CONST_INTEGER4              33
#define EMPI_CONST_INTEGER8              34

#define EMPI_CONST_REAL2                 35
#define EMPI_CONST_REAL4                 36
#define EMPI_CONST_REAL8                 37
#define EMPI_CONST_REAL16                38

/* These Fortran types are mentioned in the MPI 2.1 standard, but their status is unclear (optional synonym?) */

#define EMPI_CONST_COMPLEX8              EMPI_CONST_COMPLEX
#define EMPI_CONST_COMPLEX16             EMPI_CONST_DOUBLE_COMPLEX

/* Special datatypes for Fortran reduction operations -- as defined in MPI 2.1 standard. */

#define EMPI_CONST_2REAL                 39
#define EMPI_CONST_2DOUBLE_PRECISION     40
#define EMPI_CONST_2INTEGER              41

#define EMPI_CONST_DEFINED_DATATYPES     42

/* These are depricated! */
#define EMPI_CONST_UB                    -1
#define EMPI_CONST_LB                    -2

/*
#ifdef HAVE_MPI_2_2

#define MPI_C_BOOL                16
#define MPI_INT8_T                17
#define MPI_INT16_T               18
#define MPI_INT32_T               19
#define MPI_INT64_T               20
#define MPI_UINT8_T               21
#define MPI_UINT16_T              22
#define MPI_UINT32_T              23
#define MPI_UINT64_T              24
#define MPI_C_COMPLEX             25
#define MPI_C_FLOAT_COMPLEX       MPI_C_COMPLEX
#define MPI_C_DOUBLE_COMPLEX      26
#define MPI_C_LONG_DOUBLE_COMPLEX 27

#define MPI_COMPLEX8              MPI_C_FLOAT_COMPLEX
#define MPI_COMPLEX16             MPI_C_DOUBLE_COMPLEX
#define MPI_COMPLEX32             MPI_C_LONG_DOUBLE_COMPLEX

#endif
*/


/* Communicators */

#define EMPI_CONST_COMM_WORLD 0
#define EMPI_CONST_COMM_SELF  1
#define EMPI_CONST_COMM_NULL  2

/* Groups */

// FIXME: check!
#define EMPI_CONST_GROUP_NULL 0
#define EMPI_CONST_GROUP_EMPTY 1

/* Requests */

// FIXME: check!
#define EMPI_CONST_REQUEST_NULL -1

/* Source / tags */
#define EMPI_CONST_ANY_SOURCE -1
#define EMPI_CONST_ANY_TAG    -1
#define EMPI_CONST_TAG_LB     0      // not officially defined in MPI standard (but value is set in standard)
#define EMPI_CONST_TAG_UB     32767  // minimum as defined by MPI standard
#define EMPI_CONST_UNDEFINED  -32766 // borrowed from openmpi

/* Files */
#define EMPI_CONST_FILE_NULL  -1

#define EMPI_CONST_MODE_CREATE              1
#define EMPI_CONST_MODE_RDONLY              2
#define EMPI_CONST_MODE_WRONLY              4
#define EMPI_CONST_MODE_RDWR                8
#define EMPI_CONST_MODE_DELETE_ON_CLOSE    16
#define EMPI_CONST_MODE_UNIQUE_OPEN        32
#define EMPI_CONST_MODE_EXCL               64
#define EMPI_CONST_MODE_APPEND            128
#define EMPI_CONST_MODE_SEQUENTIAL        256

/* Info */
#define EMPI_CONST_INFO_NULL  -1

/*
 * Error codes borrowed from OpenMPI.
 */
#define EMPI_CONST_SUCCESS                   0
#define EMPI_CONST_ERR_BUFFER                1
#define EMPI_CONST_ERR_COUNT                 2
#define EMPI_CONST_ERR_TYPE                  3
#define EMPI_CONST_ERR_TAG                   4
#define EMPI_CONST_ERR_COMM                  5
#define EMPI_CONST_ERR_RANK                  6
#define EMPI_CONST_ERR_REQUEST               7
#define EMPI_CONST_ERR_ROOT                  8
#define EMPI_CONST_ERR_GROUP                 9
#define EMPI_CONST_ERR_OP                    10
#define EMPI_CONST_ERR_TOPOLOGY              11
#define EMPI_CONST_ERR_DIMS                  12
#define EMPI_CONST_ERR_ARG                   13
#define EMPI_CONST_ERR_UNKNOWN               14
#define EMPI_CONST_ERR_TRUNCATE              15
#define EMPI_CONST_ERR_OTHER                 16
#define EMPI_CONST_ERR_INTERN                17
#define EMPI_CONST_ERR_IN_STATUS             18
#define EMPI_CONST_ERR_PENDING               19
#define EMPI_CONST_ERR_ACCESS                20
#define EMPI_CONST_ERR_AMODE                 21
#define EMPI_CONST_ERR_ASSERT                22
#define EMPI_CONST_ERR_BAD_FILE              23
#define EMPI_CONST_ERR_BASE                  24
#define EMPI_CONST_ERR_CONVERSION            25
#define EMPI_CONST_ERR_DISP                  26
#define EMPI_CONST_ERR_DUP_DATAREP           27
#define EMPI_CONST_ERR_FILE_EXISTS           28
#define EMPI_CONST_ERR_FILE_IN_USE           29
#define EMPI_CONST_ERR_FILE                  30
#define EMPI_CONST_ERR_INFO_KEY              31
#define EMPI_CONST_ERR_INFO_NOKEY            32
#define EMPI_CONST_ERR_INFO_VALUE            33
#define EMPI_CONST_ERR_INFO                  34
#define EMPI_CONST_ERR_IO                    35
#define EMPI_CONST_ERR_KEYVAL                36
#define EMPI_CONST_ERR_LOCKTYPE              37
#define EMPI_CONST_ERR_NAME                  38
#define EMPI_CONST_ERR_NO_MEM                39
#define EMPI_CONST_ERR_NOT_SAME              40
#define EMPI_CONST_ERR_NO_SPACE              41
#define EMPI_CONST_ERR_NO_SUCH_FILE          42
#define EMPI_CONST_ERR_PORT                  43
#define EMPI_CONST_ERR_QUOTA                 44
#define EMPI_CONST_ERR_READ_ONLY             45
#define EMPI_CONST_ERR_RMA_CONFLICT          46
#define EMPI_CONST_ERR_RMA_SYNC              47
#define EMPI_CONST_ERR_SERVICE               48
#define EMPI_CONST_ERR_SIZE                  49
#define EMPI_CONST_ERR_SPAWN                 50
#define EMPI_CONST_ERR_UNSUPPORTED_DATAREP   51
#define EMPI_CONST_ERR_UNSUPPORTED_OPERATION 52
#define EMPI_CONST_ERR_WIN                   53
#define EMPI_CONST_ERR_LASTCODE              54

#define EMPI_CONST_ERR_SYSRESOURCE          -2

/* Predefined operations */

#define EMPI_CONST_OP_NULL  0
#define EMPI_CONST_MAX      1
#define EMPI_CONST_MIN      2
#define EMPI_CONST_SUM      3
#define EMPI_CONST_PROD     4
#define EMPI_CONST_MAXLOC   5
#define EMPI_CONST_MINLOC   6
#define EMPI_CONST_BOR      7
#define EMPI_CONST_BAND     8
#define EMPI_CONST_BXOR     9
#define EMPI_CONST_LOR     10
#define EMPI_CONST_LAND    11
#define EMPI_CONST_LXOR    12


/* Various constants */
#define EMPI_CONST_MAX_PROCESSOR_NAME 255
#define EMPI_CONST_MAX_ERROR_STRING 255

#define EMPI_CONST_ROOT -1

#endif // __EMPI_CONST_H_
