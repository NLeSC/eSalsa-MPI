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
        parameter (EMPI_STATUS_SIZE=6)

        integer EMPI_ROOT
        parameter (EMPI_ROOT=-4)

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
!	parameter (EMPI_PROC_NULL=-2)

! EMPI_status_ignore is defined in a common block

	integer EMPI_STATUS_IGNORE(EMPI_STATUS_SIZE)
	common/mpi_fortran_status_ignore/EMPI_STATUS_IGNORE

! Handles

	integer EMPI_GROUP_NULL
	integer EMPI_GROUP_EMPTY

      	parameter (EMPI_GROUP_NULL=0)
      	parameter (EMPI_GROUP_EMPTY=1)

      	integer EMPI_REQUEST_NULL

      	parameter (EMPI_REQUEST_NULL=-1)

!      	integer EMPI_ERRHANDLER_NULL
!
!     	parameter (EMPI_ERRHANDLER_NULL=0)

      	integer EMPI_INFO_NULL
      	parameter (EMPI_INFO_NULL=0)

! return codes, only EMPI_SUCCESS is used ;-)

        integer EMPI_SUCCESS
        integer EMPI_ERR_BUFFER
        integer EMPI_ERR_COUNT
        integer EMPI_ERR_TYPE
        integer EMPI_ERR_TAG
        integer EMPI_ERR_COMM
        integer EMPI_ERR_RANK
        integer EMPI_ERR_REQUEST
        integer EMPI_ERR_ROOT
        integer EMPI_ERR_GROUP
        integer EMPI_ERR_OP
        integer EMPI_ERR_TOPOLOGY
        integer EMPI_ERR_DIMS
        integer EMPI_ERR_ARG
        integer EMPI_ERR_UNKNOWN
        integer EMPI_ERR_TRUNCATE
        integer EMPI_ERR_OTHER
        integer EMPI_ERR_INTERN
        integer EMPI_ERR_IN_STATUS
        integer EMPI_ERR_PENDING               
        integer EMPI_ERR_ACCESS                
        integer EMPI_ERR_AMODE                 
        integer EMPI_ERR_ASSERT                
        integer EMPI_ERR_BAD_FILE              
        integer EMPI_ERR_BASE                 
        integer EMPI_ERR_CONVERSION            
        integer EMPI_ERR_DISP                  
        integer EMPI_ERR_DUP_DATAREP           
        integer EMPI_ERR_FILE_EXISTS           
        integer EMPI_ERR_FILE_IN_USE           
        integer EMPI_ERR_FILE                  
        integer EMPI_ERR_INFO_KEY              
        integer EMPI_ERR_INFO_NOKEY            
        integer EMPI_ERR_INFO_VALUE            
        integer EMPI_ERR_INFO                  
        integer EMPI_ERR_IO                    
        integer EMPI_ERR_KEYVAL                
        integer EMPI_ERR_LOCKTYPE              
        integer EMPI_ERR_NAME                  
        integer EMPI_ERR_NO_MEM                
        integer EMPI_ERR_NOT_SAME              
        integer EMPI_ERR_NO_SPACE              
        integer EMPI_ERR_NO_SUCH_FILE          
        integer EMPI_ERR_PORT                  
        integer EMPI_ERR_QUOTA                 
        integer EMPI_ERR_READ_ONLY             
        integer EMPI_ERR_RMA_CONFLICT          
        integer EMPI_ERR_RMA_SYNC              
        integer EMPI_ERR_SERVICE               
        integer EMPI_ERR_SIZE                  
        integer EMPI_ERR_SPAWN                 
        integer EMPI_ERR_UNSUPPORTED_DATAREP   
        integer EMPI_ERR_UNSUPPORTED_OPERATION 
        integer EMPI_ERR_WIN                   
        integer EMPI_ERR_LASTCODE              

        parameter(EMPI_SUCCESS=                   0)
        parameter(EMPI_ERR_BUFFER=                1)
        parameter(EMPI_ERR_COUNT=                 2)
        parameter(EMPI_ERR_TYPE=                  3)
        parameter(EMPI_ERR_TAG=                   4)
        parameter(EMPI_ERR_COMM=                  5)
        parameter(EMPI_ERR_RANK=                  6)
        parameter(EMPI_ERR_REQUEST=               7)
        parameter(EMPI_ERR_ROOT=                  8)
        parameter(EMPI_ERR_GROUP=                 9)
        parameter(EMPI_ERR_OP=                    10)
        parameter(EMPI_ERR_TOPOLOGY=              11)
        parameter(EMPI_ERR_DIMS=                  12)
        parameter(EMPI_ERR_ARG=                   13)
        parameter(EMPI_ERR_UNKNOWN=               14)
        parameter(EMPI_ERR_TRUNCATE=              15)
        parameter(EMPI_ERR_OTHER=                 16)
        parameter(EMPI_ERR_INTERN=                17)
        parameter(EMPI_ERR_IN_STATUS=             18)
        parameter(EMPI_ERR_PENDING=               19)
        parameter(EMPI_ERR_ACCESS=                20)
        parameter(EMPI_ERR_AMODE=                 21)
        parameter(EMPI_ERR_ASSERT=                22)
        parameter(EMPI_ERR_BAD_FILE=              23)
        parameter(EMPI_ERR_BASE=                  24)
        parameter(EMPI_ERR_CONVERSION=            25)
        parameter(EMPI_ERR_DISP=                  26)
        parameter(EMPI_ERR_DUP_DATAREP=           27)
        parameter(EMPI_ERR_FILE_EXISTS=           28)
        parameter(EMPI_ERR_FILE_IN_USE=           29)
        parameter(EMPI_ERR_FILE=                  30)
        parameter(EMPI_ERR_INFO_KEY=              31)
        parameter(EMPI_ERR_INFO_NOKEY=            32)
        parameter(EMPI_ERR_INFO_VALUE=            33)
        parameter(EMPI_ERR_INFO=                  34)
        parameter(EMPI_ERR_IO=                    35)
        parameter(EMPI_ERR_KEYVAL=                36)
        parameter(EMPI_ERR_LOCKTYPE=              37)
        parameter(EMPI_ERR_NAME=                  38)
        parameter(EMPI_ERR_NO_MEM=                39)
        parameter(EMPI_ERR_NOT_SAME=              40)
        parameter(EMPI_ERR_NO_SPACE=              41)
        parameter(EMPI_ERR_NO_SUCH_FILE=          42)
        parameter(EMPI_ERR_PORT=                  43)
        parameter(EMPI_ERR_QUOTA=                 44)
        parameter(EMPI_ERR_READ_ONLY=             45)
        parameter(EMPI_ERR_RMA_CONFLICT=          46)
        parameter(EMPI_ERR_RMA_SYNC=              47)
        parameter(EMPI_ERR_SERVICE=               48)
        parameter(EMPI_ERR_SIZE=                  49)
        parameter(EMPI_ERR_SPAWN=                 50)
        parameter(EMPI_ERR_UNSUPPORTED_DATAREP=   51)
        parameter(EMPI_ERR_UNSUPPORTED_OPERATION= 52)
        parameter(EMPI_ERR_WIN=                   53)
        parameter(EMPI_ERR_LASTCODE=              54)

! Types

     	integer EMPI_DATATYPE_NULL
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
!	integer EMPI_INTEGER16
      	integer EMPI_REAL
	integer EMPI_REAL2
	integer EMPI_REAL4
	integer EMPI_REAL8
	integer EMPI_REAL16
      	integer EMPI_DOUBLE_PRECISION
      	integer EMPI_COMPLEX
	integer EMPI_COMPLEX8
	integer EMPI_COMPLEX16
!	integer EMPI_COMPLEX32
      	integer EMPI_DOUBLE_COMPLEX
      	integer EMPI_2REAL
	integer EMPI_2DOUBLE_PRECISION
	integer EMPI_2INTEGER
!      	integer EMPI_2COMPLEX
!	integer EMPI_2DOUBLE_COMPLEX
!     	integer EMPI_LOGICAL1
!	integer EMPI_LOGICAL2
!	integer EMPI_LOGICAL4
!	integer EMPI_LOGICAL8


      	parameter (EMPI_DATATYPE_NULL=0)
      	parameter (EMPI_BYTE=28)
	parameter (EMPI_PACKED=29)
      	parameter (EMPI_UB=3)
      	parameter (EMPI_LB=4)
      	parameter (EMPI_CHARACTER=1)
      	parameter (EMPI_LOGICAL=3)
      	parameter (EMPI_INTEGER=3)
      	parameter (EMPI_INTEGER1=6)
      	parameter (EMPI_INTEGER2=2)
      	parameter (EMPI_INTEGER4=3)
      	parameter (EMPI_INTEGER8=5)
!      	parameter (EMPI_INTEGER16=12)
      	parameter (EMPI_REAL=12)
      	parameter (EMPI_REAL4=12)
      	parameter (EMPI_REAL8=13)
      	parameter (EMPI_REAL16=14)
      	parameter (EMPI_DOUBLE_PRECISION=13)
      	parameter (EMPI_COMPLEX=36)
      	parameter (EMPI_COMPLEX8=36)
      	parameter (EMPI_COMPLEX16=37)
!      	parameter (EMPI_COMPLEX32=21)
      	parameter (EMPI_DOUBLE_COMPLEX=37)
      	parameter (EMPI_2REAL=38)
      	parameter (EMPI_2DOUBLE_PRECISION=39)
      	parameter (EMPI_2INTEGER=33)
!      	parameter (EMPI_2COMPLEX=26)
!      	parameter (EMPI_2DOUBLE_COMPLEX=27)
!      	parameter (EMPI_LOGICAL1=29)
!      	parameter (EMPI_LOGICAL2=30)
!      	parameter (EMPI_LOGICAL4=31)
!      	parameter (EMPI_LOGICAL8=32)

! Operations


      	integer EMPI_OP_NULL
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

     	parameter (EMPI_OP_NULL=0)
      	parameter (EMPI_MAX=1)
      	parameter (EMPI_MIN=2)
      	parameter (EMPI_SUM=3)
      	parameter (EMPI_PROD=4)
      	parameter (EMPI_MAXLOC=5)
      	parameter (EMPI_MINLOC=6)
      	parameter (EMPI_BOR=7)
      	parameter (EMPI_BAND=8)
      	parameter (EMPI_BXOR=9)
      	parameter (EMPI_LOR=10)
      	parameter (EMPI_LAND=11)
      	parameter (EMPI_LXOR=12)
!      	parameter (EMPI_REPLACE=13)

! I/O

	integer EMPI_FILE_NULL
	integer EMPI_MODE_CREATE
	integer EMPI_MODE_RDONLY
	integer EMPI_MODE_WRONLY
	integer EMPI_MODE_RDWR
	integer EMPI_MODE_APPEND

	parameter (EMPI_FILE_NULL=0)
	parameter (EMPI_MODE_CREATE=1)
	parameter (EMPI_MODE_RDONLY=2)
	parameter (EMPI_MODE_WRONLY=4)
	parameter (EMPI_MODE_RDWR=8)
	parameter (EMPI_MODE_APPEND=128)


! Functions that return a double preceision result

      external EMPI_WTIME, EMPI_WTICK
      double precision EMPI_WTIME, EMPI_WTICK

