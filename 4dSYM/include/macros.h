// -----------------------------------------------------------------
// Macros common to all applications
#ifndef _MACROS_H
#define _MACROS_H

#include <defines.h>
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Constants
#define    PI 3.14159265358979323846
#define TWOPI 6.283185307179586

// Conventions for defining checkerboard parity
#define EVEN 0x02
#define ODD 0x01
#define EVENANDODD 0x03

// ASCII string length for all file names
#define MAXFILENAME 256

// Gauge fixing options
#define NO_GAUGE_FIX 30
#define COULOMB_GAUGE_FIX 31
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// field offset and field pointer
// Used when fields in the site are arguments to subroutines
/* Usage:  fo = F_OFFSET( field ), where "field" is the name of a field
  in lattice.
     address = F_PT( &site , fo ), where &site is the address of the
  site and fo is a field_offset.  Usually, the result will have to be
  cast to a pointer to the appropriate type. (It is naturally a char *).
*/
typedef int field_offset;
#define F_OFFSET(a) \
  ((field_offset)(((char *)&(lattice[0]. a ))-((char *)&(lattice[0])) ))
#define F_PT( site , fo )  ((char *)( site ) + (fo))
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Macros for looping over directions
#define FORALLUPDIR(dir) for (dir = XUP; dir <= TUP; dir++)

#define FORALLUPDIRBUT(direction, dir) \
   for (dir = XUP; dir <= TUP; dir++) if (dir != direction)

// Switches EVEN and ODD, nulls EVENANDODD
#define OPP_PAR(parity) (0x03 ^ parity)
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// printf on node zero only
#define node0_printf if(this_node==0)printf

#define ON 1
#define OFF 0
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Macros for looping over sites on a node
// Usage:
//  int i;      // Index of the site on the node
//  site *s;    // Pointer to the current site
//  FORALLSITES(i, s) {
//    ...
//  }
//
//  int subl;   // Sublattice to loop over
//  FORSOMESUBLATTICE(i, s, subl) {
//    ...
//  }

// See loopend.h for FORSOMEPARITY definition

#ifndef N_SUBL32
// Standard red-black checkerboard
#define FOREVENSITES(i,s) \
    for(i=0,s=lattice;i<even_sites_on_node;i++,s++)
#define FORODDSITES(i,s) \
    for(i=even_sites_on_node,s= &(lattice[i]);i<sites_on_node;i++,s++)
#define FORSOMEPARITY(i,s,choice) \
    for( i=((choice)==ODD ? even_sites_on_node : 0 ),  \
    s= &(lattice[i]); \
    i< ( (choice)==EVEN ? even_sites_on_node : sites_on_node); \
    i++,s++)
#define FORALLSITES(i,s) \
    for(i=0,s=lattice;i<sites_on_node;i++,s++)
#else
// 32 sublattice checkerboard
#define FORALLSITES(i,s) \
    for(i=0,s=lattice;i<sites_on_node;i++,s++)
#define FORSOMESUBLATTICE(i,s,subl) \
    for (i=(subl*subl_sites_on_node), s= &(lattice[i]), \
   last_in_loop=((subl+1)*subl_sites_on_node); \
   i< last_in_loop; i++,s++)
#endif
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Timing switches
#ifdef TIMING
#define TIC(n) tmptime[n] = -dclock();
#define TOC(n,mytimer) tmptime[n] += dclock(); mytimer+=tmptime[n];
#else
#define TIC(n)
#define TOC(n,mytimer)
#endif

#endif
// -----------------------------------------------------------------
