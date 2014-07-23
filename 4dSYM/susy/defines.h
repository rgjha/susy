// -----------------------------------------------------------------
// Compiler macros common to all targets in this application
#ifndef _DEFINES_H
#define _DEFINES_H

#define SITERAND              // Use site-based random number generators
#define GAUGE_FIX_TOL 1.0e-7  // For gauge fixing
//#define TIMING              // Not currently used
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Integrator stuff
// Omelyan lambda, 2lambda and 1 - 2lambda
#define LAMBDA 0.193
#define TWO_LAMBDA 0.386
#define LAMBDA_MID 0.614

// RHMC degree -- copied to Norder, which can be reset (e.g., to 1)
#define DEGREE 6
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Temporal boundary condition: 1.0 for PBC, -1.0 for APBC
#define PBC -1.0
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Susy stuff
#define SV                  // Site/vector terms in action
#define VP                  // Vector/plaquette terms in action
#define QCLOSED             // Q-closed terms in action
//#define DEBUG_CHECK         // Print lambdas, offsets, etc.

#define NQLINK 28           // Number of offsets for Q-closed terms
#define NTERMS 30           // Number of DbmP, DbpP F1Q and F2Q terms

// Tunable parameter in gauge action
#define C2 1.0

// Factor on Q-closed term
#define CQ 2.0
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Measurement stuff
// Threshold to print warning about non-zero imaginary components
// of quantities expected to be real
#define IMAG_TOL 1e-12

// Maximum time value and spatial distance for Wilson loops
#define MAX_T (nt / 2 - 1)
#define MAX_X (nx / 2 - 1)
#endif
// -----------------------------------------------------------------
