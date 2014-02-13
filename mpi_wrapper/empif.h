!!
!! This is the fortran header for the eMPI wrapper library.
!!
!! Note that we only define the bits we need in CESM, so large parts of the MPI interface may be missing!

! Various special communicators

	integer EMPI_COMM_WORLD
	integer EMPI_COMM_SELF
	integer EMPI_COMM_NULL

        parameter (EMPI_COMM_WORLD=0)
        parameter (EMPI_COMM_SELF=1)
        parameter (EMPI_COMM_NULL=2)

! Various constants

	integer EMPI_MAX_PROCESSOR_NAME
	integer EMPI_MAX_ERROR_STRING
	integer EMPI_STATUS_SIZE

        parameter (EMPI_MAX_PROCESSOR_NAME=255)
        parameter (EMPI_MAX_ERROR_STRING=255)
        parameter (EMPI_STATUS_SIZE=5)

! Kind parameters

      	integer EMPI_OFFSET_KIND
      	integer EMPI_ADDRESS_KIND
      	integer EMPI_INTEGER_KIND

      	parameter (EMPI_INTEGER_KIND=4)
      	parameter (EMPI_ADDRESS_KIND=8)
      	parameter (EMPI_OFFSET_KIND=8)

! Wildcards

 	integer EMPI_ANY_SOURCE
        integer EMPI_ANY_TAG
      	integer EMPI_PROC_NULL

	parameter (EMPI_ANY_SOURCE=-1)
	parameter (EMPI_ANY_TAG=-1)
	parameter (EMPI_PROC_NULL=-2)

! EMPI_status_ignore is defined in a common block

	integer EMPI_STATUS_IGNORE(EMPI_STATUS_SIZE)
	common/mpi_fortran_status_ignore/MPI_STATUS_IGNORE

! Handles

	integer EMPI_GROUP_NULL
     	integer EMPI_DATATYPE_NULL
      	integer EMPI_REQUEST_NULL
      	integer EMPI_OP_NULL
      	integer EMPI_ERRHANDLER_NULL
      	integer EMPI_INFO_NULL

      	parameter (EMPI_GROUP_NULL=0)
      	parameter (EMPI_DATATYPE_NULL=0)
      	parameter (EMPI_REQUEST_NULL=0)
     	parameter (EMPI_OP_NULL=0)
     	parameter (EMPI_ERRHANDLER_NULL=0)
      	parameter (EMPI_INFO_NULL=0)

! return codes, only EMPI_SUCCESS is used ;-)

	integer EMPI_SUCCESS
	parameter (EMPI_SUCCESS=0)

! Types

	integer EMPI_BYTE
	integer EMPI_PACKED
	integer EMPI_UB
	integer EMPI_LB
      	integer EMPI_CHARACTER
	integer EMPI_LOGICAL
      	integer EMPI_INTEGER
	integer EMPI_INTEGER1
	integer EMPI_INTEGER2
	integer EMPI_INTEGER4
      	integer EMPI_INTEGER8
	integer EMPI_INTEGER16
      	integer EMPI_REAL
	integer EMPI_REAL2
	integer EMPI_REAL4
	integer EMPI_REAL8
	integer EMPI_REAL16
      	integer EMPI_DOUBLE_PRECISION
      	integer EMPI_COMPLEX
	integer EMPI_COMPLEX8
	integer EMPI_COMPLEX16
	integer EMPI_COMPLEX32
      	integer EMPI_DOUBLE_COMPLEX
      	integer EMPI_2REAL
	integer EMPI_2DOUBLE_PRECISION
	integer EMPI_2INTEGER
      	integer EMPI_2COMPLEX
	integer EMPI_2DOUBLE_COMPLEX
      	integer EMPI_LOGICAL1
	integer EMPI_LOGICAL2
	integer EMPI_LOGICAL4
	integer EMPI_LOGICAL8

      	parameter (EMPI_BYTE=1)
	parameter (EMPI_PACKED=2)
      	parameter (EMPI_UB=3)
      	parameter (EMPI_LB=4)
      	parameter (EMPI_CHARACTER=5)
      	parameter (EMPI_LOGICAL=6)
      	parameter (EMPI_INTEGER=7)
      	parameter (EMPI_INTEGER1=8)
      	parameter (EMPI_INTEGER2=9)
      	parameter (EMPI_INTEGER4=10)
      	parameter (EMPI_INTEGER8=11)
      	parameter (EMPI_INTEGER16=12)
      	parameter (EMPI_REAL=13)
      	parameter (EMPI_REAL4=14)
      	parameter (EMPI_REAL8=15)
      	parameter (EMPI_REAL16=16)
      	parameter (EMPI_DOUBLE_PRECISION=17)
      	parameter (EMPI_COMPLEX=18)
      	parameter (EMPI_COMPLEX8=19)
      	parameter (EMPI_COMPLEX16=20)
      	parameter (EMPI_COMPLEX32=21)
      	parameter (EMPI_DOUBLE_COMPLEX=22)
      	parameter (EMPI_2REAL=23)
      	parameter (EMPI_2DOUBLE_PRECISION=24)
      	parameter (EMPI_2INTEGER=25)
      	parameter (EMPI_2COMPLEX=26)
      	parameter (EMPI_2DOUBLE_COMPLEX=27)
      	parameter (EMPI_REAL2=28)
      	parameter (EMPI_LOGICAL1=29)
      	parameter (EMPI_LOGICAL2=30)
      	parameter (EMPI_LOGICAL4=31)
      	parameter (EMPI_LOGICAL8=32)

! Operations

      	integer EMPI_MAX
	integer EMPI_MIN
	integer EMPI_SUM
	integer EMPI_PROD
	integer EMPI_LAND
      	integer EMPI_BAND
	integer EMPI_LOR
	integer EMPI_BOR
	integer EMPI_LXOR
	integer EMPI_BXOR
      	integer EMPI_MAXLOC
	integer EMPI_MINLOC
	integer EMPI_REPLACE

      	parameter (EMPI_MAX=1)
      	parameter (EMPI_MIN=2)
      	parameter (EMPI_SUM=3)
      	parameter (EMPI_PROD=4)
      	parameter (EMPI_LAND=5)
      	parameter (EMPI_BAND=6)
      	parameter (EMPI_LOR=7)
      	parameter (EMPI_BOR=8)
      	parameter (EMPI_LXOR=9)
      	parameter (EMPI_BXOR=10)
      	parameter (EMPI_MAXLOC=11)
      	parameter (EMPI_MINLOC=12)
      	parameter (EMPI_REPLACE=13)

! I/O

	integer EMPI_FILE_NULL
	integer EMPI_MODE_RDONLY

	parameter (EMPI_FILE_NULL=0)
	parameter (EMPI_MODE_RDONLY=2)

!
!
!


