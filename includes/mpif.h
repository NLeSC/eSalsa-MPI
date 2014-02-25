!!
!! This is the fortran header for the eMPI wrapper library.
!!
!! Note that we only define the bits we need in CESM, so large parts of the MPI interface may be missing!

! Various special communicators

	integer MPI_COMM_WORLD
	integer MPI_COMM_SELF
	integer MPI_COMM_NULL

        parameter (MPI_COMM_WORLD=0)
        parameter (MPI_COMM_SELF=1)
        parameter (MPI_COMM_NULL=2)

! Various constants

	integer MPI_MAX_PROCESSOR_NAME
	integer MPI_MAX_ERROR_STRING
	integer MPI_STATUS_SIZE

        parameter (MPI_MAX_PROCESSOR_NAME=255)
        parameter (MPI_MAX_ERROR_STRING=255)
        parameter (MPI_STATUS_SIZE=6)

        integer MPI_ROOT
        parameter (MPI_ROOT=-4)

! Kind parameters

      	integer MPI_OFFSET_KIND
      	integer MPI_ADDRESS_KIND
      	integer MPI_INTEGER_KIND

      	parameter (MPI_INTEGER_KIND=4)
      	parameter (MPI_ADDRESS_KIND=8)
      	parameter (MPI_OFFSET_KIND=8)

! Wildcards

 	integer MPI_ANY_SOURCE
        integer MPI_ANY_TAG
      	integer MPI_PROC_NULL

	parameter (MPI_ANY_SOURCE=-1)
	parameter (MPI_ANY_TAG=-1)
!	parameter (MPI_PROC_NULL=-2)

! MPI_status_ignore is defined in a common block

	integer MPI_STATUS_IGNORE(MPI_STATUS_SIZE)
	common/mpi_fortran_status_ignore/MPI_STATUS_IGNORE

! Handles

	integer MPI_GROUP_NULL
	integer MPI_GROUP_EMPTY

      	parameter (MPI_GROUP_NULL=0)
      	parameter (MPI_GROUP_EMPTY=1)

      	integer MPI_REQUEST_NULL

      	parameter (MPI_REQUEST_NULL=-1)

!      	integer MPI_ERRHANDLER_NULL
!
!     	parameter (MPI_ERRHANDLER_NULL=0)

      	integer MPI_INFO_NULL
      	parameter (MPI_INFO_NULL=0)

! return codes, only MPI_SUCCESS is used ;-)

        integer MPI_SUCCESS
        integer MPI_ERR_BUFFER
        integer MPI_ERR_COUNT
        integer MPI_ERR_TYPE
        integer MPI_ERR_TAG
        integer MPI_ERR_COMM
        integer MPI_ERR_RANK
        integer MPI_ERR_REQUEST
        integer MPI_ERR_ROOT
        integer MPI_ERR_GROUP
        integer MPI_ERR_OP
        integer MPI_ERR_TOPOLOGY
        integer MPI_ERR_DIMS
        integer MPI_ERR_ARG
        integer MPI_ERR_UNKNOWN
        integer MPI_ERR_TRUNCATE
        integer MPI_ERR_OTHER
        integer MPI_ERR_INTERN
        integer MPI_ERR_IN_STATUS
        integer MPI_ERR_PENDING               
        integer MPI_ERR_ACCESS                
        integer MPI_ERR_AMODE                 
        integer MPI_ERR_ASSERT                
        integer MPI_ERR_BAD_FILE              
        integer MPI_ERR_BASE                 
        integer MPI_ERR_CONVERSION            
        integer MPI_ERR_DISP                  
        integer MPI_ERR_DUP_DATAREP           
        integer MPI_ERR_FILE_EXISTS           
        integer MPI_ERR_FILE_IN_USE           
        integer MPI_ERR_FILE                  
        integer MPI_ERR_INFO_KEY              
        integer MPI_ERR_INFO_NOKEY            
        integer MPI_ERR_INFO_VALUE            
        integer MPI_ERR_INFO                  
        integer MPI_ERR_IO                    
        integer MPI_ERR_KEYVAL                
        integer MPI_ERR_LOCKTYPE              
        integer MPI_ERR_NAME                  
        integer MPI_ERR_NO_MEM                
        integer MPI_ERR_NOT_SAME              
        integer MPI_ERR_NO_SPACE              
        integer MPI_ERR_NO_SUCH_FILE          
        integer MPI_ERR_PORT                  
        integer MPI_ERR_QUOTA                 
        integer MPI_ERR_READ_ONLY             
        integer MPI_ERR_RMA_CONFLICT          
        integer MPI_ERR_RMA_SYNC              
        integer MPI_ERR_SERVICE               
        integer MPI_ERR_SIZE                  
        integer MPI_ERR_SPAWN                 
        integer MPI_ERR_UNSUPPORTED_DATAREP   
        integer MPI_ERR_UNSUPPORTED_OPERATION 
        integer MPI_ERR_WIN                   
        integer MPI_ERR_LASTCODE              

        parameter(MPI_SUCCESS=                   0)
        parameter(MPI_ERR_BUFFER=                1)
        parameter(MPI_ERR_COUNT=                 2)
        parameter(MPI_ERR_TYPE=                  3)
        parameter(MPI_ERR_TAG=                   4)
        parameter(MPI_ERR_COMM=                  5)
        parameter(MPI_ERR_RANK=                  6)
        parameter(MPI_ERR_REQUEST=               7)
        parameter(MPI_ERR_ROOT=                  8)
        parameter(MPI_ERR_GROUP=                 9)
        parameter(MPI_ERR_OP=                    10)
        parameter(MPI_ERR_TOPOLOGY=              11)
        parameter(MPI_ERR_DIMS=                  12)
        parameter(MPI_ERR_ARG=                   13)
        parameter(MPI_ERR_UNKNOWN=               14)
        parameter(MPI_ERR_TRUNCATE=              15)
        parameter(MPI_ERR_OTHER=                 16)
        parameter(MPI_ERR_INTERN=                17)
        parameter(MPI_ERR_IN_STATUS=             18)
        parameter(MPI_ERR_PENDING=               19)
        parameter(MPI_ERR_ACCESS=                20)
        parameter(MPI_ERR_AMODE=                 21)
        parameter(MPI_ERR_ASSERT=                22)
        parameter(MPI_ERR_BAD_FILE=              23)
        parameter(MPI_ERR_BASE=                  24)
        parameter(MPI_ERR_CONVERSION=            25)
        parameter(MPI_ERR_DISP=                  26)
        parameter(MPI_ERR_DUP_DATAREP=           27)
        parameter(MPI_ERR_FILE_EXISTS=           28)
        parameter(MPI_ERR_FILE_IN_USE=           29)
        parameter(MPI_ERR_FILE=                  30)
        parameter(MPI_ERR_INFO_KEY=              31)
        parameter(MPI_ERR_INFO_NOKEY=            32)
        parameter(MPI_ERR_INFO_VALUE=            33)
        parameter(MPI_ERR_INFO=                  34)
        parameter(MPI_ERR_IO=                    35)
        parameter(MPI_ERR_KEYVAL=                36)
        parameter(MPI_ERR_LOCKTYPE=              37)
        parameter(MPI_ERR_NAME=                  38)
        parameter(MPI_ERR_NO_MEM=                39)
        parameter(MPI_ERR_NOT_SAME=              40)
        parameter(MPI_ERR_NO_SPACE=              41)
        parameter(MPI_ERR_NO_SUCH_FILE=          42)
        parameter(MPI_ERR_PORT=                  43)
        parameter(MPI_ERR_QUOTA=                 44)
        parameter(MPI_ERR_READ_ONLY=             45)
        parameter(MPI_ERR_RMA_CONFLICT=          46)
        parameter(MPI_ERR_RMA_SYNC=              47)
        parameter(MPI_ERR_SERVICE=               48)
        parameter(MPI_ERR_SIZE=                  49)
        parameter(MPI_ERR_SPAWN=                 50)
        parameter(MPI_ERR_UNSUPPORTED_DATAREP=   51)
        parameter(MPI_ERR_UNSUPPORTED_OPERATION= 52)
        parameter(MPI_ERR_WIN=                   53)
        parameter(MPI_ERR_LASTCODE=              54)

! Types

     	integer MPI_DATATYPE_NULL
	integer MPI_BYTE
	integer MPI_PACKED
	integer MPI_UB
	integer MPI_LB
      	integer MPI_CHARACTER
	integer MPI_LOGICAL
      	integer MPI_INTEGER
	integer MPI_INTEGER1
	integer MPI_INTEGER2
	integer MPI_INTEGER4
      	integer MPI_INTEGER8
!	integer MPI_INTEGER16
      	integer MPI_REAL
	integer MPI_REAL2
	integer MPI_REAL4
	integer MPI_REAL8
	integer MPI_REAL16
      	integer MPI_DOUBLE_PRECISION
      	integer MPI_COMPLEX
	integer MPI_COMPLEX8
	integer MPI_COMPLEX16
!	integer MPI_COMPLEX32
      	integer MPI_DOUBLE_COMPLEX
      	integer MPI_2REAL
	integer MPI_2DOUBLE_PRECISION
	integer MPI_2INTEGER
!      	integer MPI_2COMPLEX
!	integer MPI_2DOUBLE_COMPLEX
!     	integer MPI_LOGICAL1
!	integer MPI_LOGICAL2
!	integer MPI_LOGICAL4
!	integer MPI_LOGICAL8


      	parameter (MPI_DATATYPE_NULL=0)
      	parameter (MPI_BYTE=28)
	parameter (MPI_PACKED=29)
      	parameter (MPI_UB=3)
      	parameter (MPI_LB=4)
      	parameter (MPI_CHARACTER=1)
      	parameter (MPI_LOGICAL=3)
      	parameter (MPI_INTEGER=3)
      	parameter (MPI_INTEGER1=6)
      	parameter (MPI_INTEGER2=2)
      	parameter (MPI_INTEGER4=3)
      	parameter (MPI_INTEGER8=5)
!      	parameter (MPI_INTEGER16=12)
      	parameter (MPI_REAL=12)
      	parameter (MPI_REAL4=12)
      	parameter (MPI_REAL8=13)
      	parameter (MPI_REAL16=14)
      	parameter (MPI_DOUBLE_PRECISION=13)
      	parameter (MPI_COMPLEX=36)
      	parameter (MPI_COMPLEX8=36)
      	parameter (MPI_COMPLEX16=37)
!      	parameter (MPI_COMPLEX32=21)
      	parameter (MPI_DOUBLE_COMPLEX=37)
      	parameter (MPI_2REAL=38)
      	parameter (MPI_2DOUBLE_PRECISION=39)
      	parameter (MPI_2INTEGER=33)
!      	parameter (MPI_2COMPLEX=26)
!      	parameter (MPI_2DOUBLE_COMPLEX=27)
!      	parameter (MPI_LOGICAL1=29)
!      	parameter (MPI_LOGICAL2=30)
!      	parameter (MPI_LOGICAL4=31)
!      	parameter (MPI_LOGICAL8=32)

! Operations


      	integer MPI_OP_NULL
      	integer MPI_MAX
	integer MPI_MIN
	integer MPI_SUM
	integer MPI_PROD
	integer MPI_LAND
      	integer MPI_BAND
	integer MPI_LOR
	integer MPI_BOR
	integer MPI_LXOR
	integer MPI_BXOR
      	integer MPI_MAXLOC
	integer MPI_MINLOC
	integer MPI_REPLACE

     	parameter (MPI_OP_NULL=0)
      	parameter (MPI_MAX=1)
      	parameter (MPI_MIN=2)
      	parameter (MPI_SUM=3)
      	parameter (MPI_PROD=4)
      	parameter (MPI_MAXLOC=5)
      	parameter (MPI_MINLOC=6)
      	parameter (MPI_BOR=7)
      	parameter (MPI_BAND=8)
      	parameter (MPI_BXOR=9)
      	parameter (MPI_LOR=10)
      	parameter (MPI_LAND=11)
      	parameter (MPI_LXOR=12)
!      	parameter (MPI_REPLACE=13)

! I/O

	integer MPI_FILE_NULL
	integer MPI_MODE_CREATE
	integer MPI_MODE_RDONLY
	integer MPI_MODE_WRONLY
	integer MPI_MODE_RDWR
	integer MPI_MODE_APPEND

	parameter (MPI_FILE_NULL=0)
	parameter (MPI_MODE_CREATE=1)
	parameter (MPI_MODE_RDONLY=2)
	parameter (MPI_MODE_WRONLY=4)
	parameter (MPI_MODE_RDWR=8)
	parameter (MPI_MODE_APPEND=128)


! Functions that return a double preceision result

      external MPI_WTIME, MPI_WTICK
      double precision MPI_WTIME, MPI_WTICK


