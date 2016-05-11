// -----------------------------------------------------------------
// Polar decomposition now uses LAPACK to isolate gauge vs. scalar d.o.f.
#include "susy_includes.h"
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Take log of hermitian part of decomposition to define scalar field
void matrix_log(matrix *in, matrix *out) {
  char V = 'V';     // Ask LAPACK for both eigenvalues and eigenvectors
  char U = 'U';     // Have LAPACK store upper triangle of U.Ubar
  int row, col, Npt = NCOL, stat = 0, Nwork = 2 * NCOL;
  double *store, *work, *Rwork, *eigs;
  matrix evecs, tmat;

  // Allocate double arrays to be used by LAPACK
  store = malloc(2 * NCOL * NCOL * sizeof(*store));
  work = malloc(2 * Nwork * sizeof(*work));
  Rwork = malloc((3 * NCOL - 2) * sizeof(*Rwork));
  eigs = malloc(NCOL * sizeof(*eigs));

  // Convert in to column-major double array used by LAPACK
  for (row = 0; row < NCOL; row++) {
    for (col = 0; col < NCOL; col++) {
      store[2 * (col * NCOL + row)] = in->e[row][col].real;
      store[2 * (col * NCOL + row) + 1] = in->e[row][col].imag;
    }
  }
//  dumpmat(in);

  // Compute eigenvalues and eigenvectors of in
  zheev_(&V, &U, &Npt, store, &Npt, eigs, work, &Nwork, Rwork, &stat);

  // Check for degenerate eigenvalues (broke previous Jacobi algorithm)
  for (row = 0; row < NCOL; row++) {
    for (col = row + 1; col < NCOL; col++) {
      if (fabs(eigs[row] - eigs[col]) < IMAG_TOL)
        printf("WARNING: w[%d] = w[%d] = %.8g\n", row, col, eigs[row]);
    }
  }

  // Move the results back into matrix structures
  // Use evecs to hold the eigenvectors for projection
  clear_mat(out);
  for (row = 0; row < NCOL; row++) {
    for (col = 0; col < NCOL; col++) {
      evecs.e[row][col].real = store[2 * (col * NCOL + row)];
      evecs.e[row][col].imag = store[2 * (col * NCOL + row) + 1];
    }
//    node0_printf("%.4g, ", eigs[row]);
    out->e[row][row].real = log(eigs[row]);
  }
  // Inverse of eigenvector matrix is simply adjoint
  mult_na(out, &evecs, &tmat);
  mult_nn(&evecs, &tmat, out);
//  node0_printf("\n");
//  dumpmat(out);
//  node0_printf("\n");

  // Free double arrays used by LAPACK
  free(store);
  free(work);
  free(Rwork);
  free(eigs);
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// Given matrix in = P.u, calculate the unitary matrix u = [1 / P].in
//   and the positive P = sqrt[in.in^dag]
// We diagonalize PSq = in.in^dag using LAPACK,
// then project out its inverse square root
void polar(matrix *in, matrix *u, matrix *P) {
  char V = 'V';     // Ask LAPACK for both eigenvalues and eigenvectors
  char U = 'U';     // Have LAPACK store upper triangle of U.Ubar
  int row, col, Npt = NCOL, stat = 0, Nwork = 2 * NCOL;
  double *store, *work, *Rwork, *eigs;
  complex minus1 = cmplx(-1.0, 0.0);
  matrix PSq, Pinv, tmat;

  // Allocate double arrays to be used by LAPACK
  store = malloc(2 * NCOL * NCOL * sizeof(*store));
  work = malloc(2 * Nwork * sizeof(*work));
  Rwork = malloc((3 * NCOL - 2) * sizeof(*Rwork));
  eigs = malloc(NCOL * sizeof(*eigs));

  // Convert PSq to column-major double array used by LAPACK
  mult_na(in, in, &PSq);
  for (row = 0; row < NCOL; row++) {
    for (col = 0; col < NCOL; col++) {
      store[2 * (col * NCOL + row)] = PSq.e[row][col].real;
      store[2 * (col * NCOL + row) + 1] = PSq.e[row][col].imag;
    }
  }
//  dumpmat(&PSq);

  // Compute eigenvalues and eigenvectors of PSq
  zheev_(&V, &U, &Npt, store, &Npt, eigs, work, &Nwork, Rwork, &stat);

  // Check for degenerate eigenvalues (broke previous Jacobi algorithm)
  for (row = 0; row < NCOL; row++) {
    for (col = row + 1; col < NCOL; col++) {
      if (fabs(eigs[row] - eigs[col]) < IMAG_TOL)
        printf("WARNING: w[%d] = w[%d] = %.8g\n", row, col, eigs[row]);
    }
  }

  // Move the results back into matrix structures
  // Overwrite PSq to hold the eigenvectors for projection
  for (row = 0; row < NCOL; row++) {
    for (col = 0; col < NCOL; col++) {
      PSq.e[row][col].real = store[2 * (col * NCOL + row)];
      PSq.e[row][col].imag = store[2 * (col * NCOL + row) + 1];
      P->e[row][col] = cmplx(0.0, 0.0);
      Pinv.e[row][col] = cmplx(0.0, 0.0);
    }
//    node0_printf("%.4g, ", eigs[row]);
    P->e[row][row].real = sqrt(eigs[row]);
    Pinv.e[row][row].real = 1.0 / sqrt(eigs[row]);
  }
  mult_na(P, &PSq, &tmat);
  mult_nn(&PSq, &tmat, P);
//  node0_printf("\n");
//  dumpmat(&PSq);
//  node0_printf("\n");

  // Now project out 1 / sqrt[in.in^dag] to find u = [1 / P].in
  mult_na(&Pinv, &PSq, &tmat);
  mult_nn(&PSq, &tmat, &Pinv);
  mult_nn(&Pinv, in, u);

  // Check unitarity of u
  mult_na(u, u, &PSq);
  c_scalar_add_diag(&PSq, &minus1);
  for (row = 0; row < NCOL; row++) {
    for (col = 0; col < NCOL; col++) {
      if (cabs_sq(&(PSq.e[row][col])) > SQ_TOL) {
        printf("Error getting unitary piece: ");
        printf("%.4g > %.4g for [%d, %d]\n",
               cabs(&(PSq.e[row][col])), IMAG_TOL, row, col);

        dumpmat(in);
        dumpmat(u);
        dumpmat(P);
        return;
      }
    }
  }

  // Check hermiticity of P
  adjoint(P, &tmat);
  sub_matrix(P, &tmat, &PSq);
  for (row = 0; row < NCOL; row++) {
    for (col = 0; col < NCOL; col++) {
      if (cabs_sq(&(PSq.e[row][col])) > SQ_TOL) {
        printf("Error getting hermitian piece: ");
        printf("%.4g > %.4g for [%d, %d]\n",
               cabs(&(PSq.e[row][col])), IMAG_TOL, row, col);

        dumpmat(in);
        dumpmat(u);
        dumpmat(P);
        return;
      }
    }
  }

  // Check that in = P.u
  mult_nn(P, u, &tmat);
  sub_matrix(in, &tmat, &PSq);
  for (row = 0; row < NCOL; row++) {
    for (col = 0; col < NCOL; col++) {
      if (cabs_sq(&(PSq.e[row][col])) > SQ_TOL) {
        printf("Error reconstructing initial matrix: ");
        printf("%.4g > %.4g for [%d, %d]\n",
               cabs(&(PSq.e[row][col])), IMAG_TOL, row, col);

        dumpmat(in);
        dumpmat(u);
        dumpmat(P);
        return;
      }
    }
  }

  // Free double arrays used by LAPACK
  free(store);
  free(work);
  free(Rwork);
  free(eigs);
}
// -----------------------------------------------------------------
