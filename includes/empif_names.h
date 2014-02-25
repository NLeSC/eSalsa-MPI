#ifndef __MPIF_NAMES_H_
#define __MPIF_NAMES_H_

/*
#if   defined(FORTRAN_UNDERSCORE_) || defined(FORTRANUNDERSCORE)
#define FORT_NAME(lower,upper) lower##_
#elif defined(FORTRAN_GNUF2C)
#define FORT_NAME(lower,upper) lower##__
#elif defined(FORTRAN_SAME)
#define FORT_NAME(lower,upper) lower
#elif defined(FORTRAN_CAPS_)
#define FORT_NAME(lower,upper) upper
#else
#error "Unrecognized Fortran-mangle type"
#endif
*/

#define FORT_NAME(lower,upper) lower##_


#endif // __MPIF_NAMES_H_
