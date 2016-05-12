/* Trefethen, Lloyd N., and David Bau III.
   Numerical linear algebra. Vol. 50. Siam, 1997.
   Algorithm 10.1. Householder QR Factorization

   for k=1 to n
     x = A_{k:m,k}
     v_k = sign(x_1)\|x\|_2 e_1 + x
     v_k = v_k / \|v_k\|_2
     A_{k:m,k:n} = A_{k:m,k:n} - 2 v_k (v_k^* A_{k:m,k:n}) */

template <U8 m, U8 n>
INLINE Vector<m> get_householder_vector(Matrix<m,n> a, U8 k, U8 o) {
  Real norm_x = 0;
  for (U8 i = k + o; i < m; ++i)
    norm_x += square(a[k][i]);
  norm_x = sqrt(norm_x);
  Vector<m> v_k;
  for (U8 i = k + o; i < m; ++i)
    v_k[i] = a[k][i];
  v_k[k + o] += sign(a[k][k + o]) * norm_x;
  Real norm_v_k = 0;
  for (U8 i = k + o; i < m; ++i)
    norm_v_k += square(v_k[i]);
  norm_v_k = sqrt(norm_v_k);
  for (U8 i = k + o; i < m; ++i)
    v_k[i] /= norm_v_k;
  return v_k;
}

template <U8 m, U8 n>
INLINE void reflect_columns(Matrix<m,n>& a, Vector<m> v_k, U8 k, U8 o) {
  for (U8 j = 0; j < n; ++j) {
    Real dot = 0;
    for (U8 i = k + o; i < m; ++i)
      dot += a[j][i] * v_k[i];
    for (U8 i = k + o; i < m; ++i)
      a[j][i] -= 2 * dot * v_k[i];
  }
}

template <U8 m, U8 n>
INLINE void factorize_qr_householder(Matrix<m,n>& a,
    Few<Vector<m>, n>& v) {
  for (U8 k = 0; k < n; ++k) {
    v[k] = get_householder_vector(a, k, 0);
    reflect_columns(a, v[k], k, 0);
  }
}

/* Trefethen, Lloyd N., and David Bau III.
   Numerical linear algebra. Vol. 50. Siam, 1997.
   Algorithm 10.2. Implicit Calculation of a Product $Q^*b$

   for k=1 to n
     b_{k:m} = b_{k:m} - 2 v_k (v_k^* b_{k:m}) */
template <U8 m, U8 n>
INLINE void implicit_q_trans_b(Vector<m>& b,
    Few<Vector<m>, n> v) {
  for (U8 k = 0; k < n; ++k) {
    Real dot = 0;
    for (U8 i = k; i < m; ++i)
      dot += v[k][i] * b[i];
    for (U8 i = k; i < m; ++i)
      b[i] -= 2 * dot * v[k][i];
  }
}

/* Trefethen, Lloyd N., and David Bau III.
   Numerical linear algebra. Vol. 50. Siam, 1997.
   Algorithm 10.2. Implicit Calculation of a Product $Qx$

   for k=n downto 1
     x_{k:m} = x_{k:m} - 2 v_k (v_k^* b_{k:m}) */
template <U8 m, U8 n>
INLINE void implicit_q_x(Vector<m>& x,
    Few<Vector<m>, n> v) {
  for (U8 k2 = 0; k2 < n; ++k2) {
    U8 k = n - k2;
    Real dot = 0;
    for (U8 i = k; i < m; ++i)
      dot += v[k][i] * x[i];
    for (U8 i = k; i < m; ++i)
      x[i] -= 2 * dot * v[k][i];
  }
}

template <U8 m, U8 n>
INLINE void decompose_qr_reduced(Matrix<m,n> a, Matrix<m,n>& q, Matrix<n,n>& r) {
  Few<Vector<m>, n> v;
  factorize_qr_householder(a, v);
  for (U8 j = 0; j < n; ++j)
    for (U8 i = 0; i < n; ++i)
      r[j][i] = a[j][i];
  for (U8 j = 0; j < n; ++j) {
    for (U8 i = 0; i < m; ++i)
      q[j][i] = (i == j);
    implicit_q_x(q[j], v);
  }
}
