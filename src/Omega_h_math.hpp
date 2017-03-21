#ifndef OMEGA_H_MATH_HPP
#define OMEGA_H_MATH_HPP

#include <cfloat>
#include <climits>
#include <cmath>

#include <Omega_h_array.hpp>
#include <Omega_h_defines.hpp>
#include <Omega_h_few.hpp>
#include <Omega_h_scalar.hpp>
#include <Omega_h_vector.hpp>

namespace Omega_h {

/* column-first storage and indexing !!! */
template <Int m, Int n>
class Matrix : public Few<Vector<m>, n> {
 public:
  OMEGA_H_INLINE Matrix() {}
  /* these constructors accept the matrix in
   * row-first order for convenience.
   */
  inline Matrix(std::initializer_list<Vector<m>> l) : Few<Vector<m>, n>(l) {}
  inline Matrix(std::initializer_list<Real> l);
  OMEGA_H_INLINE void operator=(Matrix<m, n> const& rhs) volatile {
    Few<Vector<m>, n>::operator=(rhs);
  }
  OMEGA_H_INLINE Matrix(Matrix<m, n> const& rhs) : Few<Vector<m>, n>(rhs) {}
  OMEGA_H_INLINE Matrix(const volatile Matrix<m, n>& rhs)
      : Few<Vector<m>, n>(rhs) {}
};

template <Int m, Int n>
inline Matrix<m, n>::Matrix(std::initializer_list<Real> l) {
  Int k = 0;
  for (Real v : l) {
    (*this)[k % n][k / n] = v;
    ++k;
  }
}

template <Int m, Int n>
OMEGA_H_INLINE Vector<m> operator*(Matrix<m, n> a, Vector<n> b) {
  Vector<m> c = a[0] * b[0];
  for (Int j = 1; j < n; ++j) c = c + a[j] * b[j];
  return c;
}

template <Int m, Int n, Int p>
OMEGA_H_INLINE Matrix<m, n> operator*(Matrix<m, p> a, Matrix<p, n> b) {
  Matrix<m, n> c;
  for (Int j = 0; j < n; ++j) c[j] = a * b[j];
  return c;
}

template <Int m, Int n>
OMEGA_H_INLINE Matrix<n, m> transpose(Matrix<m, n> a) {
  Matrix<n, m> b;
  for (Int i = 0; i < m; ++i)
    for (Int j = 0; j < n; ++j) b[i][j] = a[j][i];
  return b;
}

template <Int max_m, Int max_n>
OMEGA_H_INLINE Matrix<max_m, max_n> identity_matrix(Int m, Int n) {
  Matrix<max_m, max_n> a;
  for (Int j = 0; j < n; ++j)
    for (Int i = 0; i < m; ++i) a[j][i] = (i == j);
  return a;
}

template <Int max_m, Int max_n>
OMEGA_H_INLINE Matrix<max_m, max_n> identity_matrix() {
  return identity_matrix<max_m, max_n>(max_m, max_n);
}

OMEGA_H_INLINE Matrix<1, 1> matrix_1x1(Real a) {
  Matrix<1, 1> o;
  o[0][0] = a;
  return o;
}

OMEGA_H_INLINE Matrix<2, 2> matrix_2x2(Real a, Real b, Real c, Real d) {
  Matrix<2, 2> o;
  o[0] = vector_2(a, c);
  o[1] = vector_2(b, d);
  return o;
}

OMEGA_H_INLINE Matrix<3, 3> matrix_3x3(
    Real a, Real b, Real c, Real d, Real e, Real f, Real g, Real h, Real i) {
  Matrix<3, 3> o;
  o[0] = vector_3(a, d, g);
  o[1] = vector_3(b, e, h);
  o[2] = vector_3(c, f, i);
  return o;
}

OMEGA_H_INLINE Matrix<3, 3> cross(Vector<3> a) {
  return matrix_3x3(0, -a[2], a[1], a[2], 0, -a[0], -a[1], a[0], 0);
}

OMEGA_H_INLINE Vector<3> cross(Vector<3> a, Vector<3> b) {
  return vector_3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
      a[0] * b[1] - a[1] * b[0]);
}

template <Int m>
OMEGA_H_INLINE Matrix<m, m> diagonal(Vector<m> v) {
  Matrix<m, m> a;
  for (Int i = 0; i < m; ++i)
    for (Int j = i + 1; j < m; ++j) a[i][j] = a[j][i] = 0.0;
  for (Int i = 0; i < m; ++i) a[i][i] = v[i];
  return a;
}

template <Int n>
OMEGA_H_DEVICE void set_vector(Write<Real> const& a, Int i, Vector<n> v) {
  for (Int j = 0; j < n; ++j) a[i * n + j] = v[j];
}

template <Int n, class Arr>
OMEGA_H_DEVICE Vector<n> get_vector(Arr const& a, Int i) {
  Vector<n> v;
  for (Int j = 0; j < n; ++j) v[j] = a[i * n + j];
  return v;
}

OMEGA_H_INLINE constexpr Int symm_dofs(Int dim) {
  return ((dim + 1) * dim) / 2;
}

OMEGA_H_INLINE Vector<1> symm2vector(Matrix<1, 1> symm) {
  return vector_1(symm[0][0]);
}

OMEGA_H_INLINE Matrix<1, 1> vector2symm(Vector<1> v) {
  return matrix_1x1(v[0]);
}

OMEGA_H_INLINE Vector<3> symm2vector(Matrix<2, 2> symm) {
  Vector<3> v;
  v[0] = symm[0][0];
  v[1] = symm[1][1];
  v[2] = symm[1][0];
  return v;
}

OMEGA_H_INLINE Matrix<2, 2> vector2symm(Vector<3> v) {
  Matrix<2, 2> symm;
  symm[0][0] = v[0];
  symm[1][1] = v[1];
  symm[1][0] = v[2];
  symm[0][1] = symm[1][0];
  return symm;
}

OMEGA_H_INLINE Vector<6> symm2vector(Matrix<3, 3> symm) {
  Vector<6> v;
  v[0] = symm[0][0];
  v[1] = symm[1][1];
  v[2] = symm[2][2];
  v[3] = symm[1][0];
  v[4] = symm[2][1];
  v[5] = symm[2][0];
  return v;
}

OMEGA_H_INLINE Matrix<3, 3> vector2symm(Vector<6> v) {
  Matrix<3, 3> symm;
  symm[0][0] = v[0];
  symm[1][1] = v[1];
  symm[2][2] = v[2];
  symm[1][0] = v[3];
  symm[2][1] = v[4];
  symm[2][0] = v[5];
  symm[0][1] = symm[1][0];
  symm[1][2] = symm[2][1];
  symm[0][2] = symm[2][0];
  return symm;
}

template <Int n>
OMEGA_H_DEVICE void set_symm(Write<Real> const& a, Int i, Matrix<n, n> symm) {
  set_vector(a, i, symm2vector(symm));
}

template <Int n, typename Arr>
OMEGA_H_DEVICE Matrix<n, n> get_symm(Arr const& a, Int i) {
  return vector2symm(get_vector<symm_dofs(n)>(a, i));
}

template <Int dim>
OMEGA_H_INLINE Vector<dim> metric_eigenvalues(Vector<dim> h) {
  Vector<dim> l;
  for (Int i = 0; i < dim; ++i) l[i] = 1.0 / square(h[i]);
  return l;
}

template <Int dim>
OMEGA_H_INLINE Matrix<dim, dim> compose_metric(
    Matrix<dim, dim> r, Vector<dim> h) {
  auto l = metric_eigenvalues(h);
  return r * diagonal(l) * transpose(r);
}

template <Int dim>
Reals repeat_symm(LO n, Matrix<dim, dim> symm);
extern template Reals repeat_symm(LO n, Matrix<3, 3> symm);
extern template Reals repeat_symm(LO n, Matrix<2, 2> symm);

OMEGA_H_INLINE Real metric_eigenvalue_from_length(Real h) {
  return 1.0 / square(h);
}

}  // end namespace Omega_h

#endif
