﻿#ifndef ARIS_DYNAMIC_MATRIX_H_
#define ARIS_DYNAMIC_MATRIX_H_

#include <vector>
#include <numeric>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

#include <aris_lib_export.h>
#include <aris/core/basic_type.hpp>

namespace aris::dynamic{
	template<typename Type>
	class RowIterator {
	public:
		using size_type = Size; // optional

		using difference_type = typename std::ptrdiff_t;
		using value_type = typename double;
		using reference = typename double&;
		using pointer = typename double*;
		using iterator_category = std::random_access_iterator_tag; //or another tag
		
		auto operator=(const RowIterator&other)->RowIterator& = default;
		auto operator==(const RowIterator&other) const->bool { return iter_ == other.iter_; }
		auto operator!=(const RowIterator&other) const->bool { return iter_ != other.iter_; }
		auto operator<(const RowIterator&other) const->bool { return iter_ < other.iter_; } //optional
		auto operator>(const RowIterator&other) const->bool { return iter_ > other.iter_; } //optional
		auto operator<=(const RowIterator&other) const->bool { return iter_ <= other.iter_; } //optional
		auto operator>=(const RowIterator&other) const->bool { return iter_ >= other.iter_; } //optional

		auto operator++()->RowIterator& { iter_ += next_c(0, type_); return *this; }
		auto operator++(int)->RowIterator { RowIterator ret(*this); operator++(); return ret; } //optional
		auto operator--()->RowIterator& { iter_ -= next_c(0, type_); return *this; } //optional
		auto operator--(int)->RowIterator { RowIterator ret(*this); operator--(); return ret; } //optional
		auto operator+=(size_type size)->RowIterator& { iter_ += size * next_c(0, type_); return *this; } //optional
		auto operator+(size_type size) const->RowIterator { return RowIterator(iter_ + size * next_c(0, type_), type_);} //optional
		friend auto operator+(size_type size, const RowIterator&iter)->RowIterator { return *this + size; } //optional
		auto operator-=(size_type size)->RowIterator& { iter_ -= size * next_c(0, type_); return *this; } //optional
		auto operator-(size_type size) const->RowIterator { return RowIterator(iter_ + size * next_c(0, type_), type_); } //optional
		auto operator-(RowIterator iter) const->difference_type { return (iter_ - iter.iter_)/ next_c(0, type_);} //optional

		auto operator*() const->reference { return *iter_; }
		auto operator->() const->pointer { return iter_; }
		auto operator[](size_type size) const->reference { return iter_[size*next_c(0, type_)]; } //optional

		~RowIterator() = default;
		RowIterator() = default;
		RowIterator(const RowIterator& other) = default;
		RowIterator(double* iter, Type t) :iter_(iter), type_(t) {} //
		
	private:
		double *iter_;
		Type type_;
	};

	
	
	struct ARIS_API RowMajor { Size r_ld; constexpr RowMajor(Size r_ld_)noexcept :r_ld(r_ld_) {}; };
	struct ARIS_API ColMajor { Size c_ld; constexpr ColMajor(Size c_ld_)noexcept :c_ld(c_ld_) {}; };
	struct ARIS_API Stride { Size r_ld, c_ld; Stride()noexcept = default; constexpr Stride(Size r_ld_, Size c_ld_)noexcept :r_ld(r_ld_), c_ld(c_ld_) {}; };

	auto inline constexpr T(Size ld)noexcept->ColMajor { return ColMajor(ld); }
	auto inline constexpr at(Size i, Size ld)noexcept->Size { return i * ld; }
	auto inline constexpr at(Size i, Size j, Size ld)noexcept->Size { return i * ld + j; }
	auto inline constexpr next_r(Size at, Size ld)noexcept->Size { return at + ld; }
	auto inline constexpr last_r(Size at, Size ld)noexcept->Size { return at - ld; }
	auto inline constexpr next_c(Size at, Size ld)noexcept->Size { return at + 1; }
	auto inline constexpr last_c(Size at, Size ld)noexcept->Size { return at - 1; }
	auto inline constexpr next_d(Size at, Size ld)noexcept->Size { return at + 1 + ld; }
	auto inline constexpr last_d(Size at, Size ld)noexcept->Size { return at - 1 - ld; }

	auto inline constexpr T(RowMajor r)noexcept->ColMajor { return ColMajor(r.r_ld); }
	auto inline constexpr at(Size i, RowMajor row_major)noexcept->Size { return i * row_major.r_ld; }
	auto inline constexpr at(Size i, Size j, RowMajor row_major)noexcept->Size { return i * row_major.r_ld + j; }
	auto inline constexpr next_r(Size at, RowMajor row_major)noexcept->Size { return at + row_major.r_ld; }
	auto inline constexpr last_r(Size at, RowMajor row_major)noexcept->Size { return at - row_major.r_ld; }
	auto inline constexpr next_c(Size at, RowMajor row_major)noexcept->Size { return at + 1; }
	auto inline constexpr last_c(Size at, RowMajor row_major)noexcept->Size { return at - 1; }
	auto inline constexpr next_d(Size at, RowMajor row_major)noexcept->Size { return at + 1 + row_major.r_ld; }
	auto inline constexpr last_d(Size at, RowMajor row_major)noexcept->Size { return at - 1 - row_major.r_ld; }

	auto inline constexpr T(ColMajor c)noexcept->RowMajor { return RowMajor(c.c_ld); }
	auto inline constexpr at(Size i, ColMajor col_major)noexcept->Size { return i; }
	auto inline constexpr at(Size i, Size j, ColMajor col_major)noexcept->Size { return i + j * col_major.c_ld; }
	auto inline constexpr next_r(Size at, ColMajor col_major)noexcept->Size { return at + 1; }
	auto inline constexpr last_r(Size at, ColMajor col_major)noexcept->Size { return at - 1; }
	auto inline constexpr next_c(Size at, ColMajor col_major)noexcept->Size { return at + col_major.c_ld; }
	auto inline constexpr last_c(Size at, ColMajor col_major)noexcept->Size { return at - col_major.c_ld; }
	auto inline constexpr next_d(Size at, ColMajor col_major)noexcept->Size { return at + 1 + col_major.c_ld; }
	auto inline constexpr last_d(Size at, ColMajor col_major)noexcept->Size { return at - 1 - col_major.c_ld; }

	auto inline constexpr T(Stride s)noexcept->Stride { return Stride(s.c_ld, s.r_ld); }
	auto inline constexpr at(Size i, Stride stride)noexcept->Size { return i * stride.r_ld; }
	auto inline constexpr at(Size i, Size j, Stride stride)noexcept->Size { return i * stride.r_ld + j * stride.c_ld; }
	auto inline constexpr next_r(Size at, Stride stride)noexcept->Size { return at + stride.r_ld; }
	auto inline constexpr last_r(Size at, Stride stride)noexcept->Size { return at - stride.r_ld; }
	auto inline constexpr next_c(Size at, Stride stride)noexcept->Size { return at + stride.c_ld; }
	auto inline constexpr last_c(Size at, Stride stride)noexcept->Size { return at - stride.c_ld; }
	auto inline constexpr next_d(Size at, Stride stride)noexcept->Size { return at + stride.c_ld + stride.r_ld; }
	auto inline constexpr last_d(Size at, Stride stride)noexcept->Size { return at - stride.c_ld - stride.r_ld; }

	template <typename T, typename TType>
	auto inline dsp(Size m, Size n, const T *data, TType d_t)noexcept->void
	{
		std::cout << std::setiosflags(std::ios::fixed) << std::setiosflags(std::ios::right) << std::setprecision(14);

		std::cout << std::endl;
		for (Size i = 0; i < m; i++)
		{
			for (Size j = 0; j < n; j++)
			{
				std::cout << data[at(i, j, d_t)] << "   ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	};
	template <typename T>
	auto inline dsp(Size m, Size n, const T *data)noexcept->void { dsp(m, n, data, n); }
	template<typename AType>
	auto dlmwrite(const Size m, const Size n, const double *A, AType a_t, const char *filename)->void
	{
		std::ofstream file;

		file.open(filename);

		file << std::setprecision(15);

		for (Size i(-1); ++i < m;)
		{
			for (Size j(-1); ++j < n;)
			{
				file << A[at(i, j, a_t)] << "   ";
			}
			file << std::endl;
		}
	}
	auto inline dlmwrite(const Size m, const Size n, const double *A, const char *filename)->void { dlmwrite(m, n, A, n, filename); }
	auto ARIS_API dlmread(const char *filename, double *mtx)->void;
	auto ARIS_API dlmread(const char *filename)->std::vector<double>;

	template <typename T>
	auto inline s_sgn(T val)noexcept->T { return T(T(0) < val) - (val < T(0)); }
	template <typename T>
	auto inline s_sgn(T val, T zero_check)noexcept->T { return std::abs(val)<zero_check ? T(0) : s_sgn(val); }
	template <typename T>
	auto inline s_sgn2(T val)noexcept->T { return val < T(0) ? T(-1) : T(1); }

	auto inline s_is_equal(double a, double b, double error)noexcept { return std::abs(a - b) < error; }
	template <typename V1Type, typename V2Type>
	auto inline s_is_equal(Size n, const double *v1, V1Type v1_t, const double *v2, V2Type v2_t, double error) noexcept->bool{
		for (Size i = 0; i < n; ++i)if (!s_is_equal(v1[at(i, v1_t)], v2[at(i, v2_t)], error))return false;
		return true;
	}
	auto inline s_is_equal(Size n, const double *v1, const double *v2, double error) noexcept->bool { return s_is_equal(n, v1, 1, v2, 1, error); };
	template <typename M1Type, typename M2Type>
	auto inline s_is_equal(Size m, Size n, const double *m1, M1Type m1_t, const double *m2, M2Type m2_t, double error) noexcept->bool{
		for (Size i = 0; i < m; ++i)for (Size j = 0; j < n; ++j)if (!s_is_equal(m1[at(i, j, m1_t)], m2[at(i, j, m2_t)], error)) return false;
		return true;
	}
	auto inline s_is_equal(Size m, Size n, const double *m1, const double *m2, double error) noexcept->bool { return s_is_equal(m, n, m1, n, m2, n, error); };

	template <typename AType>
	auto inline s_eye(Size m, double *A, AType a_t) noexcept->void{
		for (Size i(-1), ai0{ 0 }, aii{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), aii = next_d(aii, a_t)){
			for (Size j(-1), aij{ ai0 }; ++j < m; aij = next_c(aij, a_t))
				A[aij] = 0.0;
			A[aii] = 1.0;
		}
	}
	auto inline s_eye(Size m, double *A) noexcept->void { return s_eye(m, A, m); }
	template <typename AType>
	auto inline s_rmx(double angle, double *A, AType a_t) noexcept->void{
		A[at(0, 0, a_t)] = 1.0;
		A[at(0, 1, a_t)] = 0.0;
		A[at(0, 2, a_t)] = 0.0;
		A[at(1, 0, a_t)] = 0.0;
		A[at(1, 1, a_t)] = std::cos(angle);
		A[at(1, 2, a_t)] = -std::sin(angle);
		A[at(2, 0, a_t)] = 0.0;
		A[at(2, 1, a_t)] = std::sin(angle);
		A[at(2, 2, a_t)] = std::cos(angle);
	}
	auto inline s_rmx(double angle, double *A) noexcept->void { return s_rmx(angle, A, 3); }
	template <typename AType>
	auto inline s_rmy(double angle, double *A, AType a_t) noexcept->void{
		A[at(0, 0, a_t)] = std::cos(angle);
		A[at(0, 1, a_t)] = 0.0;
		A[at(0, 2, a_t)] = std::sin(angle);
		A[at(1, 0, a_t)] = 0.0;
		A[at(1, 1, a_t)] = 1.0;
		A[at(1, 2, a_t)] = 0.0;
		A[at(2, 0, a_t)] = -std::sin(angle);
		A[at(2, 1, a_t)] = 0.0;
		A[at(2, 2, a_t)] = std::cos(angle);
	}
	auto inline s_rmy(double angle, double *A) noexcept->void { return s_rmy(angle, A, 3); }
	template <typename AType>
	auto inline s_rmz(double angle, double *A, AType a_t) noexcept->void{
		A[at(0, 0, a_t)] = std::cos(angle);
		A[at(0, 1, a_t)] = -std::sin(angle);
		A[at(0, 2, a_t)] = 0.0;
		A[at(1, 0, a_t)] = std::sin(angle);
		A[at(1, 1, a_t)] = std::cos(angle);
		A[at(1, 2, a_t)] = 0.0;
		A[at(2, 0, a_t)] = 0.0;
		A[at(2, 1, a_t)] = 0.0;
		A[at(2, 2, a_t)] = 1.0;
	}
	auto inline s_rmz(double angle, double *A) noexcept->void { return s_rmz(angle, A, 3); }

	template<typename XType>
	auto inline s_norm(Size n, const double *x, XType x_t) noexcept->double
	{
		double norm = 0;
		for (Size i(-1), x_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t))norm += x[x_id] * x[x_id];
		return std::sqrt(norm);
	}
	auto inline s_norm(Size n, const double *x) noexcept->double { return s_norm(n, x, 1); }
	template<typename XType, typename YType>
	auto inline s_swap_v(Size n, double *x, XType x_t, double *y, YType y_t) noexcept->void
	{
		for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))
			std::swap(x[x_id], y[y_id]);
	}
	auto inline s_swap_v(Size n, double *x, double *y) noexcept->void { s_swap_v(n, x, 1, y, 1); }
	template<typename AType, typename BType>
	auto inline s_swap_m(Size m, Size n, double *a, AType a_t, double *b, BType b_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, bi0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), bi0 = next_r(bi0, b_t))
		{
			for (Size j(-1), aij{ ai0 }, bij{ bi0 }; ++j < n; aij = next_c(aij, a_t), bij = next_c(bij, b_t))
			{
				std::swap(a[aij], b[bij]);
			}
		}
	}
	auto inline s_swap_m(Size m, Size n, double *a, double *b) noexcept->void { s_swap_m(m, n, a, n, b, n); }
	template<typename AType>
	auto inline s_fill(Size m, Size n, double value, double *A, AType a_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t))
			for (Size j(-1), a_ij{ ai0 }; ++j < n; a_ij = next_c(a_ij, a_t))
				A[a_ij] = value;
	}// fill matrix with value
	auto inline s_fill(Size m, Size n, double value, double *A) noexcept->void { std::fill(A, A + m * n, value); }
	template<typename XType>
	auto inline s_iv(Size n, double *x, XType x_t) noexcept->void { for (Size i(-1), vid{ 0 }; ++i < n; vid = next_r(vid, x_t))x[vid] = -x[vid]; }
	auto inline s_iv(Size n, double *x) noexcept->void { for (Size i(-1); ++i < n;)x[i] = -x[i]; }
	template<typename XType>
	auto inline s_nv(Size n, double alpha, double *x, XType x_t) noexcept->void { for (Size i(-1), vid{ 0 }; ++i < n; vid = next_r(vid, x_t))x[vid] *= alpha; }
	auto inline s_nv(Size n, double alpha, double *x) noexcept->void { for (Size i(-1); ++i < n;)x[i] *= alpha; }
	template<typename XType, typename YType>
	auto inline s_vc(Size n, const double *x, XType x_t, double *y, YType y_t) noexcept->void { for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))y[y_id] = x[x_id]; }
	template<typename XType, typename YType>
	auto inline s_vc(Size n, double alpha, const double *x, XType x_t, double *y, YType y_t) noexcept->void { for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))y[y_id] = alpha * x[x_id]; }
	auto inline s_vc(Size n, const double *x, double *y) noexcept->void { std::copy_n(x, n, y); }
	auto inline s_vc(Size n, double alpha, const double *x, double *y) noexcept->void { for (Size i(-1); ++i < n;)y[i] = alpha * x[i]; }
	template<typename XType, typename YType>
	auto inline s_vi(Size n, const double *x, XType x_t, double *y, YType y_t) noexcept->void { for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))y[y_id] = -x[x_id]; }
	auto inline s_vi(Size n, const double *x, double *y) noexcept->void { for (Size i(-1); ++i < n;)y[i] = -x[i]; }
	template<typename XType, typename YType>
	auto inline s_va(Size n, const double* x, XType x_t, double* y, YType y_t) noexcept->void { for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))y[y_id] += x[x_id]; }
	template<typename XType, typename YType>
	auto inline s_va(Size n, double alpha, const double* x, XType x_t, double* y, YType y_t) noexcept->void { for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))y[y_id] += alpha * x[x_id]; }
	auto inline s_va(Size n, const double* x, double* y) noexcept->void { for (Size i = 0; i < n; ++i)y[i] += x[i]; }
	auto inline s_va(Size n, double alpha, const double* x, double* y) noexcept->void { for (Size i = 0; i < n; ++i)y[i] += alpha * x[i]; }
	template<typename XType, typename YType>
	auto inline s_vs(Size n, const double* x, XType x_t, double* y, YType y_t) noexcept->void { for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))y[y_id] -= x[x_id]; }
	auto inline s_vs(Size n, const double* x, double* y) noexcept->void { for (Size i = 0; i < n; ++i)y[i] -= x[i]; }
	template<typename XType, typename YType>
	auto inline s_vv(Size n, const double *x, XType x_t, const double *y, YType y_t) noexcept->double { double ret{ 0 }; for (Size i(-1), x_id{ 0 }, y_id{ 0 }; ++i < n; x_id = next_r(x_id, x_t), y_id = next_r(y_id, y_t))ret += x[x_id] * y[y_id]; return ret; }
	auto inline s_vv(Size n, const double *x, const double *y) noexcept->double { double ret{ 0 }; for (Size i = 0; i < n; ++i)ret += x[i] * y[i];	return ret; }
	template<typename AType>
	auto inline s_nm(Size m, Size n, double alpha, double* A, AType a_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t))
			for (Size j(-1), aij{ ai0 }; ++j < n; aij = next_c(aij, a_t))
				A[aij] *= alpha;
	}
	auto inline s_nm(Size m, Size n, double alpha, double* A) noexcept->void { s_nv(m*n, alpha, A); }
	template<typename AType>
	auto inline s_im(Size m, Size n, double* A, AType a_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t))
			for (Size j(-1), aij{ ai0 }; ++j < n; aij = next_c(aij, a_t))
				A[aij] = -A[aij];
	}
	auto inline s_im(Size m, Size n, double* A) noexcept->void { s_iv(m*n, A); }
	template<typename AType, typename BType>
	auto inline s_mc(Size m, Size n, const double *A, AType a_t, double *B, BType b_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, bi0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), bi0 = next_r(bi0, b_t))
			for (Size j(-1), aij{ ai0 }, bij{ bi0 }; ++j < n; aij = next_c(aij, a_t), bij = next_c(bij, b_t))
				B[bij] = A[aij];
	}
	template<typename AType, typename BType>
	auto inline s_mc(Size m, Size n, double alpha, const double *A, AType a_t, double *B, BType b_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, bi0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), bi0 = next_r(bi0, b_t))
			for (Size j(-1), aij{ ai0 }, bij{ bi0 }; ++j < n; aij = next_c(aij, a_t), bij = next_c(bij, b_t))
				B[bij] = alpha * A[aij];
	}
	auto inline s_mc(Size m, Size n, const double *A, double *B) noexcept->void { s_vc(m*n, A, B); }
	auto inline s_mc(Size m, Size n, double alpha, const double *A, double *B) noexcept->void { s_vc(m*n, alpha, A, B); }
	template<typename AType, typename BType>
	auto inline s_ma(Size m, Size n, const double* A, AType a_t, double* B, BType b_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, bi0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), bi0 = next_r(bi0, b_t))
			for (Size j(-1), aij{ ai0 }, bij{ bi0 }; ++j < n; aij = next_c(aij, a_t), bij = next_c(bij, b_t))
				B[bij] += A[aij];
	}
	template<typename AType, typename BType>
	auto inline s_ma(Size m, Size n, double alpha, const double* A, AType a_t, double* B, BType b_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, bi0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), bi0 = next_r(bi0, b_t))
			for (Size j(-1), aij{ ai0 }, bij{ bi0 }; ++j < n; aij = next_c(aij, a_t), bij = next_c(bij, b_t))
				B[bij] += alpha * A[aij];
	}
	auto inline s_ma(Size m, Size n, const double* A, double* B) noexcept->void { s_va(m*n, A, B); }
	auto inline s_ma(Size m, Size n, double alpha, const double* A, double* B) noexcept->void { s_va(m*n, alpha, A, B); }
	template<typename AType, typename BType>
	auto inline s_mi(Size m, Size n, const double* A, AType a_t, double* B, BType b_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, bi0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), bi0 = next_r(bi0, b_t))
			for (Size j(-1), aij{ ai0 }, bij{ bi0 }; ++j < n; aij = next_c(aij, a_t), bij = next_c(bij, b_t))
				B[bij] = -A[aij];
	}
	auto inline s_mi(Size m, Size n, const double* A, double* B) noexcept->void { s_vi(m*n, A, B); }
	template<typename AType, typename BType>
	auto inline s_ms(Size m, Size n, const double* A, AType a_t, double* B, BType b_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, bi0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), bi0 = next_r(bi0, b_t))
			for (Size j(-1), aij{ ai0 }, bij{ bi0 }; ++j < n; aij = next_c(aij, a_t), bij = next_c(bij, b_t))
				B[bij] -= A[aij];
	}
	auto inline s_ms(Size m, Size n, const double* A, double* B) noexcept->void { s_vs(m*n, A, B); }
	template<typename AType, typename BType, typename CType>
	auto inline s_mma(Size m, Size n, Size k, const double* A, AType a_t, const double* B, BType b_t, double *C, CType c_t)noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, ci0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), ci0 = next_r(ci0, c_t))
		{
			for (Size j(-1), b0j{ 0 }, cij{ ci0 }; ++j < n; b0j = next_c(b0j, b_t), cij = next_c(cij, c_t))
			{
				for (Size u(-1), aiu{ ai0 }, buj{ b0j }; ++u < k; aiu = next_c(aiu, a_t), buj = next_r(buj, b_t))
					C[cij] += A[aiu] * B[buj];
			}
		}
	}
	template<typename AType, typename BType, typename CType>
	auto inline s_mma(Size m, Size n, Size k, double alpha, const double* A, AType a_t, const double* B, BType b_t, double *C, CType c_t)noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, ci0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), ci0 = next_r(ci0, c_t))
		{
			for (Size j(-1), b0j{ 0 }, cij{ ci0 }; ++j < n; b0j = next_c(b0j, b_t), cij = next_c(cij, c_t))
			{
				double value{ 0 };
				for (Size u(-1), aiu{ ai0 }, buj{ b0j }; ++u < k; aiu = next_c(aiu, a_t), buj = next_r(buj, b_t))
					value += A[aiu] * B[buj];
				C[cij] += alpha * value;
			}
		}
	}
	auto inline s_mma(Size m, Size n, Size k, const double* A, const double* B, double *C) noexcept->void { s_mma(m, n, k, A, k, B, n, C, n); }
	auto inline s_mma(Size m, Size n, Size k, double alpha, const double* A, const double* B, double *C) noexcept->void { s_mma(m, n, k, alpha, A, k, B, n, C, n); }
	template<typename AType, typename BType, typename CType>
	auto inline s_mms(Size m, Size n, Size k, const double* A, AType a_t, const double* B, BType b_t, double *C, CType c_t)noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, ci0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), ci0 = next_r(ci0, c_t))
		{
			for (Size j(-1), b0j{ 0 }, cij{ ci0 }; ++j < n; b0j = next_c(b0j, b_t), cij = next_c(cij, c_t))
			{
				for (Size u(-1), aiu{ ai0 }, buj{ b0j }; ++u < k; aiu = next_c(aiu, a_t), buj = next_r(buj, b_t))
					C[cij] -= A[aiu] * B[buj];
			}
		}
	}
	auto inline s_mms(Size m, Size n, Size k, const double* A, const double* B, double *C) noexcept->void { s_mms(m, n, k, A, k, B, n, C, n); }
	template<typename AType, typename BType, typename CType>
	auto inline s_mm(Size m, Size n, Size k, const double* A, AType a_t, const double* B, BType b_t, double *C, CType c_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, ci0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), ci0 = next_r(ci0, c_t))
		{
			for (Size j(-1), b0j{ 0 }, cij{ ci0 }; ++j < n; b0j = next_c(b0j, b_t), cij = next_c(cij, c_t))
			{
				C[cij] = 0.0;
				for (Size u(-1), aiu{ ai0 }, buj{ b0j }; ++u < k; aiu = next_c(aiu, a_t), buj = next_r(buj, b_t))
					C[cij] += A[aiu] * B[buj];
			}
		}
	}
	template<typename AType, typename BType, typename CType>
	auto inline s_mm(Size m, Size n, Size k, double alpha, const double* A, AType a_t, const double* B, BType b_t, double *C, CType c_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, ci0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), ci0 = next_r(ci0, c_t))
		{
			for (Size j(-1), b0j{ 0 }, cij{ ci0 }; ++j < n; b0j = next_c(b0j, b_t), cij = next_c(cij, c_t))
			{
				C[cij] = 0.0;
				for (Size u(-1), aiu{ ai0 }, buj{ b0j }; ++u < k; aiu = next_c(aiu, a_t), buj = next_r(buj, b_t))
					C[cij] += A[aiu] * B[buj];
				C[cij] *= alpha;
			}
		}
	}
	auto inline s_mm(Size m, Size n, Size k, const double* A, const double* B, double *C) noexcept->void { s_fill(m, n, 0, C); s_mma(m, n, k, A, B, C); }
	auto inline s_mm(Size m, Size n, Size k, double alpha, const double* A, const double* B, double *C) noexcept->void { s_mm(m, n, k, A, B, C); s_nm(m, n, alpha, C); }
	template<typename AType, typename BType, typename CType>
	auto inline s_mmi(Size m, Size n, Size k, const double* A, AType a_t, const double* B, BType b_t, double *C, CType c_t) noexcept->void
	{
		for (Size i(-1), ai0{ 0 }, ci0{ 0 }; ++i < m; ai0 = next_r(ai0, a_t), ci0 = next_r(ci0, c_t))
		{
			for (Size j(-1), b0j{ 0 }, cij{ ci0 }; ++j < n; b0j = next_c(b0j, b_t), cij = next_c(cij, c_t))
			{
				C[cij] = 0.0;
				for (Size u(-1), aiu{ ai0 }, buj{ b0j }; ++u < k; aiu = next_c(aiu, a_t), buj = next_r(buj, b_t))
					C[cij] -= A[aiu] * B[buj];
			}
		}
	}
	auto inline s_mmi(Size m, Size n, Size k, const double* A, const double* B, double *C) noexcept->void { s_fill(m, n, 0, C); s_mms(m, n, k, A, B, C); }

	// x_new[i] = x[p[i]]
	template<typename PIter, typename XType>
	auto inline s_permutate(Size m, Size rhs, PIter p, double *x, XType x_t)noexcept->void{
		for (Size i(-1), xi0{ 0 }; ++i < m; xi0 = next_r(xi0, x_t)){
			Size k{ static_cast<Size>(p[i]) };
			for (; k < i; k = static_cast<Size>(p[k]));
			for (Size j(-1), xij{ xi0 }, xkj{ at(k,0,x_t) }; ++j < rhs; xij = next_c(xij, x_t), xkj = next_c(xkj, x_t))std::swap(x[xij], x[xkj]);
		}
	}
	auto inline s_permutate(Size m, Size rhs, const Size *p, double *x)noexcept->void { s_permutate(m, rhs, p, x, rhs); }
	// x_new[p[i]] = x[i]
	template<typename PIter, typename XType>
	auto inline s_permutate_inv(Size m, Size rhs, PIter p, double *x, XType x_t)noexcept->void{
		for (Size i(-1), xi0{ 0 }; ++i < m; xi0 = next_r(xi0, x_t)){
			Size check_k{ static_cast<Size>(p[i]) };
			for (; check_k > i; check_k = static_cast<Size>(p[check_k]));
			if (check_k == i){
				for (Size k{ static_cast<Size>(p[i]) }; k > i; k = static_cast<Size>(p[k])){
					for (Size j(-1), xij{ xi0 }, xkj{ at(k,0,x_t) }; ++j < rhs; xij = next_c(xij, x_t), xkj = next_c(xkj, x_t))
						std::swap(x[xij], x[xkj]);
				}
			}
		}
	}
	auto inline s_permutate_inv(Size m, Size rhs, const Size *p, double *x)noexcept->void { s_permutate_inv(m, rhs, p, x, rhs); }

	// A can be the same as L, only when they have same type
	template<typename AType, typename LType>
	auto inline s_llt(Size m, const double *A, AType a_t, double *L, LType l_t) noexcept->void
	{
		for (Size j(-1), ajj{ 0 }, ljj{ 0 }, lj0{ 0 }; ++j < m; ajj = next_d(ajj, a_t), ljj = next_d(ljj, l_t), lj0 = next_r(lj0, l_t))
		{
			L[ljj] = A[ajj];
			for (Size k(-1), ljk{ lj0 }; ++k < j; ljk = next_c(ljk, l_t))
			{
				L[ljj] -= L[ljk] * L[ljk];
			}
			L[ljj] = std::sqrt(L[ljj]);


			for (Size i(j), li0{ next_r(lj0,l_t) }, lji{ next_c(ljj, l_t) }, lij{ next_r(ljj, l_t) }, a_ij{ next_r(ajj,a_t) }; ++i < m; li0 = next_r(li0, l_t), lji = next_c(lji, l_t), lij = next_r(lij, l_t), a_ij = next_r(a_ij, a_t))
			{
				L[lij] = A[a_ij];
				for (Size k(-1), l_ik{ li0 }, ljk{ lj0 }; ++k < j; l_ik = next_c(l_ik, l_t), ljk = next_c(ljk, l_t))
				{
					L[lij] -= L[l_ik] * L[ljk];
				}
				L[lij] /= L[ljj];
				L[lji] = L[lij];
			}
		}
	}
	auto inline s_llt(Size m, const double *A, double *L) noexcept->void { s_llt(m, A, m, L, m); };
	// L can be the same as inv_L, only when they have same type
	template<typename LType, typename InvLType>
	auto inline s_inv_lm(Size m, const double *L, LType l_t, double *inv_L, InvLType inv_l_t) noexcept->void
	{
		for (Size j(-1), inv_Ljj{ 0 }, Ljj{ 0 }; ++j < m; inv_Ljj = next_r(next_c(inv_Ljj, inv_l_t), inv_l_t), Ljj = next_r(next_c(Ljj, l_t), l_t))
		{
			inv_L[inv_Ljj] = 1.0 / L[Ljj];

			for (Size i(j), inv_Lij{ next_r(inv_Ljj, inv_l_t) }, inv_Lji{ next_c(inv_Ljj, inv_l_t) }; ++i < m; inv_Lij = next_r(inv_Lij, inv_l_t), inv_Lji = next_c(inv_Lji, inv_l_t))
			{
				double alpha{ 0 };
				for (Size k(j - 1), Lik{ at(i, j, l_t) }, inv_Lkj{ at(j, j, inv_l_t) }; ++k < i; Lik = next_c(Lik, l_t), inv_Lkj = next_r(inv_Lkj, inv_l_t))
				{
					alpha -= L[Lik] * inv_L[inv_Lkj];
				}
				inv_L[inv_Lij] = alpha / L[at(i, i, l_t)];
				inv_L[inv_Lji] = 0.0;
			}
		}
	}
	auto inline s_inv_lm(Size m, const double *L, double *inv_L) noexcept->void { s_inv_lm(m, L, m, inv_L, m); }
	// U can be the same as inv_U, only when they have same type
	template<typename LType, typename InvLType>
	auto inline s_inv_um(Size m, const double *U, LType l_t, double *inv_U, InvLType inv_l_t) noexcept->void { s_inv_lm(m, U, T(l_t), inv_U, T(inv_l_t)); }
	auto inline s_inv_um(Size m, const double *U, double *inv_U) noexcept->void { s_inv_um(m, U, m, inv_U, m); }
	// b can be the same as x, only when they have same type
	template<typename LType, typename bType, typename xType>
	auto inline s_sov_lm(Size m, Size rhs, const double *L, LType l_t, const double *b, bType b_t, double *x, xType x_t, double zero_check = 1e-10) noexcept->void{
		for (Size j(-1), x0j{ 0 }, b0j{ 0 }; ++j < rhs; x0j = next_c(x0j, x_t), b0j = next_c(b0j, b_t)){
			for (Size i(-1), xij{ x0j }, bij{ b0j }, li0{ 0 }, lii{ 0 }; ++i < m; xij = next_r(xij, x_t), bij = next_r(bij, b_t), li0 = next_r(li0, l_t), lii = next_d(lii, l_t)){
				x[xij] = b[bij];

				for (Size k(-1), lik{ li0 }, xkj{ x0j }; ++k < i; lik = next_c(lik, l_t), xkj = next_r(xkj, x_t)){
					x[xij] -= L[lik] * x[xkj];
				}
				x[xij] = std::abs(L[lii]) > zero_check ? x[xij] / L[lii] : 0.0;
			}
		}
	}
	auto inline s_sov_lm(Size m, Size rhs, const double *L, const double *b, double *x, double zero_check = 1e-10) noexcept->void { s_sov_lm(m, rhs, L, m, b, rhs, x, rhs, zero_check); }
	// b can be the same as x, only when they have same type
	template<typename LType, typename bType, typename xType>
	auto inline s_sov_um(Size m, Size rhs, const double *L, LType l_t, const double *b, bType b_t, double *x, xType x_t, double zero_check = 1e-10) noexcept->void{
		for (Size j(-1), xmj{ at(m - 1, 0, x_t) }, bmj{ at(m - 1, 0, b_t) }; ++j < rhs; xmj = next_c(xmj, x_t), bmj = next_c(bmj, b_t)){
			for (Size i(m), xij{ xmj }, bij{ bmj }, lii{ at(m - 1, m - 1, l_t) }; --i < m; xij = last_r(xij, x_t), bij = last_r(bij, b_t), lii = last_d(lii, l_t)){
				x[xij] = b[bij];

				for (Size k(i), lik{ next_c(lii, l_t) }, xkj{ next_r(xij, x_t) }; ++k < m; lik = next_c(lik, l_t), xkj = next_r(xkj, x_t)){
					x[xij] -= L[lik] * x[xkj];
				}
				x[xij] = std::abs(L[lii]) > zero_check ? x[xij] / L[lii] : 0.0;
			}
		}
	}
	auto inline s_sov_um(Size m, Size rhs, const double *L, const double *b, double *x, double zero_check = 1e-10) noexcept->void { s_sov_um(m, rhs, L, m, b, rhs, x, rhs, zero_check); }

	// solve decomposition of A
	//
	//    A :        m x n
	//    U :        m x n
	//  tau : max(m,n) x 1
	//
	//    U can be the same address with A
	template<typename AType, typename UType, typename TauType>
	auto inline s_householder_ut(Size m, Size n, const double *A, AType a_t, double *U, UType u_t, double *tau, TauType tau_t, double zero_check = 1e-10)noexcept->void
	{
		s_mc(m, n, A, a_t, U, u_t);
		// 这里防止 m - 1 变成 -1（既最大）
		for (Size i(-1), uii{ 0 }, ti{ 0 }; ++i < std::min({ m - 1, m, n }); uii = next_d(uii, u_t), ti = next_r(ti, tau_t))
		{
			// compute householder vector //
			double rho = -s_norm(m - i, U + uii, u_t) * s_sgn2(U[uii]);
			if (std::abs(rho) > zero_check)
			{
				auto U_i1_i = U + next_r(uii, u_t);

				tau[ti] = U[uii] / rho - 1.0;
				s_nv(m - 1 - i, 1.0 / (U[uii] - rho), U_i1_i, u_t);
				U[uii] = rho;

				for (Size j(i), uij{ next_c(uii,u_t) }; ++j < n; uij = next_c(uij, u_t))
				{
					auto U_i1_j = U + next_r(uij, u_t);

					double k = tau[ti] * (s_vv(m - i - 1, U_i1_i, u_t, U_i1_j, u_t) + U[uij]);
					U[uij] += k;
					s_va(m - i - 1, k, U_i1_i, u_t, U_i1_j, u_t);
				}
			}
			else
			{
				tau[ti] = 0.0;
			}
		}
	}
	auto inline s_householder_ut(Size m, Size n, const double *A, double *U, double *tau, double zero_check = 1e-10)noexcept->void { s_householder_ut(m, n, A, n, U, n, tau, 1, zero_check); }
	
	//    Q = f(U,tau)  where Q * R = A 
	//
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    Q :        m x m
	//
	//    Q can be the same address with U, if m < n
	template<typename UType, typename TauType, typename QType>
	auto inline s_householder_ut2q(Size m, Size n, const double *U, UType u_t, const double *tau, TauType tau_t, double *Q, QType q_t)noexcept->void
	{
		auto size = std::min(m, n);
		s_fill(m - size, m - size, 0.0, Q + at(n, n, q_t), q_t);
		for (Size i(-1), qii(at(n, n, q_t)); ++i < m - size; qii = next_d(qii, q_t)) Q[qii] = 1.0;
		if (m > 0) Q[at(m - 1, m - 1, q_t)] = 1.0;

		// make Q
		for (Size j(std::min({ m - 1, m, n })), qjj(at(j - 1, j - 1, q_t)), uj1j(at(j, j - 1, u_t)), tj(at(j - 1, tau_t)); --j < std::min({ m - 1, m, n }); qjj = last_d(qjj, q_t), uj1j = last_d(uj1j, u_t), tj = last_r(tj, tau_t))
		{
			Q[qjj] = 1 + tau[tj];
			s_vc(m - j - 1, tau[tj], U + uj1j, u_t, Q + next_r(qjj, q_t), q_t);
			s_mm(1, m - j - 1, m - j - 1, Q + next_r(qjj, q_t), T(q_t), Q + next_d(qjj, q_t), q_t, Q + next_c(qjj, q_t), q_t);
			s_mma(m - j - 1, m - j - 1, 1, U + uj1j, u_t, Q + next_c(qjj, q_t), q_t, Q + next_d(qjj, q_t), q_t);
		}
	}
	auto inline s_householder_ut2q(Size m, Size n, const double *U, const double *tau, double *Q)noexcept->void { s_householder_ut2q(m, n, U, n, tau, 1, Q, m); }
	
	//    Q = f(U,tau)  where Q * R = A 
	//
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    Q :        m x n
	//
	//    Q can be the same address with U
	template<typename UType, typename TauType, typename QType>
	auto inline s_householder_ut2qmn(Size m, Size n, const double *U, UType u_t, const double *tau, TauType tau_t, double *Q, QType q_t)noexcept->void
	{
		if (m <= n && m > 0) Q[at(m - 1, m - 1, q_t)] = 1.0;
		// make Q
		for (Size j(std::min({ m - 1, m, n })), qjj(at(j - 1, j - 1, q_t)), uj1j(at(j, j - 1, u_t)), tj(at(j - 1, tau_t)); --j < std::min({ m - 1, m, n }); qjj = last_d(qjj, q_t), uj1j = last_d(uj1j, u_t), tj = last_r(tj, tau_t))
		{
			Q[qjj] = 1 + tau[tj];
			s_vc(m - j - 1, tau[tj], U + uj1j, u_t, Q + next_r(qjj, q_t), q_t);
			s_mm(1, std::min(m, n) - j - 1, m - j - 1, Q + next_r(qjj, q_t), T(q_t), Q + next_d(qjj, q_t), q_t, Q + next_c(qjj, q_t), q_t);
			s_mma(m - j - 1, std::min(m, n) - j - 1, 1, U + uj1j, u_t, Q + next_c(qjj, q_t), q_t, Q + next_d(qjj, q_t), q_t);
		}
	}
	auto inline s_householder_ut2qmn(Size m, Size n, const double *U, const double *tau, double *Q)noexcept->void { s_householder_ut2qmn(m, n, U, n, tau, 1, Q, n); }
	
	//    R = f(U,tau)  where Q * R = A 
	//
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    R :        m x n
	//
	//    R can be the same address with U
	template<typename UType, typename TauType, typename RType>
	auto inline s_householder_ut2r(Size m, Size n, const double *U, UType u_t, const double *tau, TauType tau_t, double *R, RType r_t)noexcept->void
	{
		s_mc(m, n, U, u_t, R, r_t);
		for (Size i(-1), rj1j{ next_r(0,r_t) }; ++i < std::min({ m - 1, m, n }); rj1j = next_d(rj1j, r_t))
		{
			s_fill(m - i - 1, 1, 0.0, R + rj1j, r_t);
		}
	}
	auto inline s_householder_ut2r(Size m, Size n, const double *U, const double *tau, double *R)noexcept->void { s_householder_ut2r(m, n, U, n, tau, 1, R, n); }
	
	// [Q,R]= f(U,tau)  where Q * R = A 
	//
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    Q :        m x m
	//    R :        m x n
	//
	//    R can be the same address with U
	template<typename UType, typename TauType, typename QType, typename RType>
	auto inline s_householder_ut2qr(Size m, Size n, const double *U, UType u_t, const double *tau, TauType tau_t, double *Q, QType q_t, double *R, RType r_t)noexcept->void
	{
		s_householder_ut2q(m, n, U, u_t, tau, tau_t, Q, q_t);
		s_householder_ut2r(m, n, U, u_t, tau, tau_t, R, r_t);
	}
	auto inline s_householder_ut2qr(Size m, Size n, const double *U, const double *tau, double *Q, double *R)noexcept->void { s_householder_ut2qr(m, n, U, n, tau, 1, Q, m, R, n); }
	
	//    x = Q * b 
	//
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    b :        m x rhs
	//    x : max(n,m) x rhs
	//
	//    x can be the same address with b
	template<typename UType, typename TauType, typename BType, typename XType>
	auto inline s_householder_ut_q_dot(Size m, Size n, Size rhs, const double *U, UType u_t, const double *tau, TauType tau_t, const double *b, BType b_t, double *x, XType x_t)noexcept->void{
		s_mc(m, rhs, b, b_t, x, x_t);

		Size i_begin{ std::min({ m - 1, m, n }) };
		for (Size i(i_begin), ti{ at(i - 1, tau_t) }, xi0{ at(i - 1, 0, x_t) }, uii{ at(i - 1, i - 1, u_t) }; --i < i_begin; ti = last_r(ti, tau_t), xi0 = last_r(xi0, x_t), uii = last_d(uii, u_t)){
			for (Size j(-1), xij{ xi0 }; ++j < rhs; xij = next_c(xij, x_t)){
				auto Xi1j = x + next_r(xij, x_t);
				auto Ui1j = U + next_r(uii, u_t);

				double k = tau[ti] * (x[xij] + s_vv(m - i - 1, Ui1j, u_t, Xi1j, x_t));
				x[xij] += k;
				s_ma(m - i - 1, 1, k, Ui1j, u_t, Xi1j, x_t);
			}
		}
	}
	auto inline s_householder_ut_q_dot(Size m, Size n, Size rhs, const double *U, const double *tau, const double *b, double *x)noexcept->void { s_householder_ut_q_dot(m, n, rhs, U, n, tau, 1, b, rhs, x, rhs); }
	
	//    x = Q^T * b
	//
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    b :        m x rhs
	//    x :        m x rhs
	//
	//    x can be the same address with b
	template<typename UType, typename TauType, typename BType, typename XType>
	auto inline s_householder_ut_qt_dot(Size m, Size n, Size rhs, const double *U, UType u_t, const double *tau, TauType tau_t, const double *b, BType b_t, double *x, XType x_t)noexcept->void
	{
		s_mc(m, rhs, b, b_t, x, x_t);

		for (Size i(-1), ti{ 0 }, xi0{ 0 }, uii{ 0 }; ++i < std::min({ m - 1, m, n }); ti = next_r(ti, tau_t), xi0 = next_r(xi0, x_t), uii = next_d(uii, u_t))
		{
			for (Size j(-1), xij{ xi0 }; ++j < rhs; xij = next_c(xij, x_t))
			{
				auto Xi1j = x + next_r(xij, x_t);
				auto Ui1j = U + next_r(uii, u_t);

				double k = tau[ti] * (x[xij] + s_vv(m - i - 1, Ui1j, u_t, Xi1j, x_t));
				x[xij] += k;
				s_ma(m - i - 1, 1, k, Ui1j, u_t, Xi1j, x_t);
			}
		}
	}
	auto inline s_householder_ut_qt_dot(Size m, Size n, Size rhs, const double *U, const double *tau, const double *b, double *x)noexcept->void { s_householder_ut_qt_dot(m, n, rhs, U, n, tau, 1, b, rhs, x, rhs); }
	
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    p : max(m,n) x 1
	//    b :        m x rhs
	//    x : max(n,m) x rhs
	//
	//    x can be the same address with b
	template<typename UType, typename TauType, typename BType, typename XType>
	auto inline s_householder_ut_sov(Size m, Size n, Size rhs, const double *U, UType u_t, const double *tau, TauType tau_t, const double *b, BType b_t, double *x, XType x_t, double zero_check = 1e-10)noexcept->void
	{
		s_householder_ut_qt_dot(m, n, rhs, U, u_t, tau, tau_t, b, b_t, x, x_t);
		s_sov_um(std::min(m, n), rhs, U, u_t, x, x_t, x, x_t, zero_check);

		if (n > m)s_fill(n - m, rhs, 0.0, x + at(m, 0, x_t), x_t);
	}
	auto inline s_householder_ut_sov(Size m, Size n, Size rhs, const double *U, const double *tau, const double *b, double *x, double zero_check = 1e-10)noexcept->void { s_householder_ut_sov(m, n, rhs, U, n, tau, 1, b, rhs, x, rhs, zero_check); }
	
	// solve decomposition of A * P
	//    A :        m x n
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    p : max(m,n) x 1
	//
	//    U can be the same address with A
	template<typename AType, typename UType, typename TauType>
	auto inline s_householder_utp(Size m, Size n, const double *A, AType a_t, double *U, UType u_t, double *tau, TauType tau_t, Size *p, Size &rank, double zero_check = 1e-10)noexcept->void{
		rank = 0;

		// init u //
		s_mc(m, n, A, a_t, U, u_t);

		// init p //
		std::iota(p, p + n, 0);

		// compute //
		for (Size i(-1), uii{ 0 }, ti{ 0 }; ++i < std::min(m, n); uii = next_d(uii, u_t), ti = next_r(ti, tau_t)){
			// 找到模最大的一列 //
			double max_value{ 0 };
			Size max_pos{ i };

			for (Size j(i - 1), uij{ uii }; ++j < n; uij = next_c(uij, u_t)){
				double value = s_vv(m - i, U + uij, u_t, U + uij, u_t);
				max_pos = value > max_value ? j : max_pos;
				max_value = value > max_value ? value : max_value;
			}

			// 判断是否返回 //
			max_value = std::sqrt(max_value);
			if (max_value < zero_check) { s_fill(m - i, 1, 0.0, tau + ti, tau_t); return; }
			
			++rank;
			s_swap_v(m, U + at(0, max_pos, u_t), u_t, U + at(0, i, u_t), u_t);
			std::swap(p[max_pos], p[i]);

			// 若已经到达最后一行，那么就返回，因为最后一行不需要householder化 //
			if (i == m - 1) return;

			// compute householder vector //
			auto U_i1_i = U + next_r(uii, u_t);

			double rho = -max_value * s_sgn2(U[uii]);
			tau[ti] = U[uii] / rho - 1.0;
			s_nm(m - 1 - i, 1, 1.0 / (U[uii] - rho), U_i1_i, u_t);
			U[uii] = rho;

			// update matrix //
			for (Size j(i), uij(next_c(uii, u_t)); ++j < n; uij = next_c(uij, u_t))
			{
				auto U_i1_j = U + next_r(uij, u_t);

				double k = tau[ti] * (s_vv(m - i - 1, U_i1_i, u_t, U_i1_j, u_t) + U[uij]);
				U[uij] += k;
				s_va(m - i - 1, k, U_i1_i, u_t, U_i1_j, u_t);
			}
		}
	}
	auto inline s_householder_utp(Size m, Size n, const double *A, double *U, double *tau, Size *p, Size &rank, double zero_check = 1e-10)noexcept->void { s_householder_utp(m, n, A, n, U, n, tau, 1, p, rank, zero_check); }
	
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    p : max(m,n) x 1
	//    b :        m x rhs
	//    x : max(n,m) x rhs
	//
	//    x can be the same address with b
	template<typename UType, typename TauType, typename BType, typename XType>
	auto inline s_householder_utp_sov(Size m, Size n, Size rhs, Size rank, const double *U, UType u_t, const double *tau, TauType tau_t, const Size *p, const double *b, BType b_t, double *x, XType x_t, double zero_check = 1e-10)noexcept->void
	{
		s_householder_ut_qt_dot(m, rank, rhs, U, u_t, tau, tau_t, b, b_t, x, x_t);
		s_sov_um(rank, rhs, U, u_t, x, x_t, x, x_t, zero_check);
		s_fill(n - rank, rhs, 0.0, x + at(rank, 0, x_t), x_t);
		s_permutate_inv(n, rhs, p, x, x_t);
	}
	auto inline s_householder_utp_sov(Size m, Size n, Size rhs, Size rank, const double *U, const double *tau, const Size *p, const double *b, double *x, double zero_check = 1e-10)noexcept->void { s_householder_utp_sov(m, n, rhs, rank, U, n, tau, 1, p, b, rhs, x, rhs, zero_check); }
	
	template<typename UType, typename TauType, typename XType>
	auto inline s_householder_utp_sov_solution_space(Size m, Size n, Size rank, const double *U, UType u_t, const double *tau, TauType tau_t, const Size *p, double *x, XType x_t, double zero_check = 1e-10)noexcept->void
	{
		// R is:
		// [R1, R2]
		// [0 , 0 ]
		//
		// its solution space is:
		// [ R1\R2 ]
		// [  -I   ]

		s_sov_um(rank, n - rank, U, u_t, U + at(0, rank, u_t), u_t, x, x_t, zero_check);
		s_fill(n - rank, n - rank, 0.0, x + at(rank, 0, x_t), x_t);
		for (Size i(-1); ++i < n - rank; )x[at(i + rank, i, x_t)] = -1.0;
		s_permutate_inv(n, n - rank, p, x, x_t);
	}
	auto inline s_householder_utp_sov_solution_space(Size m, Size n, Size rank, const double *U, const double *tau, const Size *p, double *x, double zero_check = 1e-10)noexcept->void { s_householder_utp_sov_solution_space(m, n, rank, U, n, tau, 1, p, x, n - rank, zero_check); }
	
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    p : max(m,n) x 1
	//    x :        n x m
	// tau2 : max(n,m) x 1
	template<typename UType, typename TauType, typename XType, typename TauType2>
	auto inline s_householder_utp2pinv(Size m, Size n, Size rank, const double *U, UType u_t, const double *tau, TauType tau_t, const Size *p, double *x, XType x_t, double *tau2, TauType2 t_t, double zero_check = 1e-10)noexcept->void
	{
		// X 是 A 的 moore penrose 逆，为 n x m 维
		//
		// A 的qr分解:
		// A_mn * P_nn = Q_mxm * R_mxn
		// 若A的秩为r
		// 此时R_mn 为：
		//     1           r   r+1     n
		// 1   [             .         ]
		//     |    R1       .    R2   |
		// r   |             .         |
		//     |.......................|
		// r+1 |             .         |
		//     |             .         |
		// m   [             .         ] 
		// 
		// A*P*x = b的最小范数的最小二乘解（linear least square:lls）为：
		// 
		// c = QT * b
		// y = P * x
		// 
		// y的某一个特解为y_t：
		// 1   [         ]
		//     |  R1\c1  |
		// r   |         |
		//     |.........|
		// r+1 |         |
		//     |         |
		// n   [    0    ]
		// 
		// y的通解为y_s：
		//      1         n-r
		// 1   [             ]
		// r   |    R1\R2    |
		//     | ............|
		// r+1 | -1          |           
		//     |    -1       |
		//     |       ..    |
		// n   [          -1 ]
		// 
		// 于是y的lls解中需求通解y_s的系数，它以下方程的最小二乘解：
		// 
		// y_s * k = y_t
		// k =  y_s\(R1\c1) = y_s \ y_t
		//
		// 于是y的lls解为：
		// y = y_t - y_s*(y_s \ y_t）=(I - y_s * y_s^-1) y_t
		// x = P * y
		//
		// 若求广义逆，那么令b为mxm的单位阵，此时所求出x即为广义逆A+：
		// 同时令QR分解矩阵为：
		// A_mn * P_nn = Q_mxr * R_rxn
		// 此时y_t：
		//      1            m
		// 1   [              ]
		// r   |  R1\Q_mxr^T  |
		//     |..............|
		// r+1 |              |
		// n   [    0         ]
		//
		// 而此时y_s可以进行qr分解：
		// y_s = S_nxn * T_nxn-r
		// 其中 T_nxn-r：
		//     1     n-r               
		//  1  [      ]           
		//     |  T1  |
		// n-r |......|
		//     |      |
		//  n  [  T2  ]
		// 这里T_nxn-r ^ -1为：
		// 
		//     1       n-r    n               
		//  1  [        .     ]           
		//     | T1^-1  .     |
		// n-r [        .     ]
		// 
		// 于是 y_s * y_s ^ -1 为：
		//   S * T * T^-1 *S^T
		// = S(:,1:n-r) * S(:,1:n-r) ^ T
		// 
		// 而 I - y_s * y_s^-1 = S*S^T - S(:,1:n-r) * S(:,1:n-r) ^ T
		//  = S(:,n-r+1:end) * S(:,n-r+1:end) ^ T
		// 带入上式：
		// y = S(:,n-r+1:end)_nxr * S(:,n-r+1:end)^T_rxn * y_t
		//   = S(:,n-r+1:end)_nxr * S(1:r,n-r+1:end)^T_rxr * (R1\Q^T)_rxm
		// 
		// x = P^-1 * y;
		// 
		//


		// step 1:
		// change x to:
		//     1           r         m
		// 1   [                     ]
		// r   |         R1\QT       |
		//     |.....................|
		// r+1 | (R1\R2)^T   .       |
		// n   [             .       ] 
		//
		//

		// QT
		s_householder_ut2qmn(m, rank, U, u_t, tau, tau_t, x, T(x_t));
		// R1\QT
		s_sov_um(rank, m, U, u_t, x + at(0, 0, x_t), x_t, x, x_t, zero_check);
		// R1\R2
		s_sov_um(rank, n - rank, U, u_t, U + at(0, rank, u_t), u_t, x + at(rank, 0, x_t), T(x_t), zero_check);

		// step 2:
		// 将通解矩阵做个行变换如下：
		//
		//     1           n-r         
		//  1  [ -1          |           
		//     |    -1       |
		//     |       ..    |
		//     |          -1 |
		// n-r | ............|
		//     |    R1\R2    |
		//  n  [             ]
		//
		// 其household变换产生的U（记作S）和tau有如下特征：
		//     1           n-r         
		//  1  [ *  *  *  * |
		//     |    *  *  * |
		//     |       *  * |
		//     |          * |
		// n-r |............|
		//     | *  *  *  * |
		//  n  [ *  *  *  * ]
		//
		//   1       n-r
		// [ *  *  *  * ]
		// 
		// 这里将上述S的下三角部分储存在x中
		//
		// x变成:
		//   1           r         m
		// 1 [                     ]
		//   |         R1\QT       |
		// r |.....................|
		//   |    S      .         |
		// n [           .         ]
		//
		//
		// make S and tau
		for (Size i(-1), k0i{ at(0, rank, T(x_t)) }, ti{ 0 }; ++i < std::min({ n - rank, n, n - 1 }); k0i = next_c(k0i, T(x_t)), ti = next_r(ti, t_t))
		{
			double rho = std::sqrt(s_vv(rank, x + k0i, T(x_t), x + k0i, T(x_t)) + 1.0);

			// Aii 为 -1.0
			s_nv(rank, 1.0 / (-1.0 - rho), x + k0i, T(x_t));
			tau2[ti] = -1.0 / rho - 1.0;

			for (Size j(i), kij{ next_c(k0i,T(x_t)) }; ++j < n - rank; kij = next_c(kij, T(x_t)))
			{
				double k = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + kij, T(x_t)));
				s_va(rank, k, x + k0i, T(x_t), x + kij, T(x_t));
			}
		}

		// step 3:
		// 利用S产生的QT来乘以R1\QT，S事实上为一个很大的矩阵，但其左下角很多为0，因此每一列只用做rank维的乘法
		//
		// x变成:
		//   1           r         m
		// 1 [                     ]
		//   |     ST * (R1\QT)    |
		// r |.....................|
		//   |     S     .         |
		// n [           .         ]  
		//
		for (Size i(-1), k0i{ at(0, rank, T(x_t)) }, ti{ 0 }; ++i < n - rank; k0i = next_c(k0i, T(x_t)), ti = next_r(ti, t_t)) {
			for (Size j(-1), x0j{ at(0, 0, x_t) }; ++j < m; x0j = next_c(x0j, x_t)) {
				double alpha = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + x0j, x_t));
				s_ma(rank, 1, alpha, x + k0i, T(x_t), x + x0j, x_t);
			}
		}

		// step 4:
		// 利用S产生的Q来乘以ST（R1\QT），这里防止S被覆盖，因此局部会用tau(n-r+1:n)的内存来局部存储
		//
		// x变成:
		//   1                     m
		// 1 [                     ]
		//   |                     |
		//   |   S * ST * (R1\QT)  |
		//   |                     |
		// n [                     ]
		//
		for (Size i(n - rank), k0i{ at(0, n - 1, T(x_t)) }, ti{ at(n - rank - 1, t_t) }; --i < n - rank; k0i = last_c(k0i, T(x_t)), ti = last_r(ti, t_t)) {
			// 因为需要少占内存，因此先将
			for (Size j(-1), x0j{ at(0, 0, x_t) }, tj{ at(n - rank,t_t) }; ++j < rank; x0j = next_c(x0j, x_t), tj = next_r(tj, t_t))
			{
				double alpha = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + x0j, x_t));
				s_ma(rank, 1, alpha, x + k0i, T(x_t), x + x0j, x_t);
				tau2[tj] = alpha;
			}
			for (Size j(rank - 1), x0j{ at(0, rank, x_t) }; ++j < m; x0j = next_c(x0j, x_t))
			{
				double alpha = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + x0j, x_t));
				s_ma(rank, 1, alpha, x + k0i, T(x_t), x + x0j, x_t);
				x[at(i + rank, j, x_t)] = alpha;
			}
			s_vc(rank, tau2 + at(n - rank, t_t), t_t, x + k0i, T(x_t));
		}

		// step 5:
		// permutate
		s_permutate_inv(n, m, p, x, x_t);
	}
	auto inline s_householder_utp2pinv(Size m, Size n, Size rank, const double *U, const double *tau, const Size *p, double *x, double *tau2, double zero_check = 1e-10)noexcept->void { s_householder_utp2pinv(m, n, rank, U, n, tau, 1, p, x, m, tau2, 1, zero_check); }

	//    x = Q * b 
	//
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    b :        m x rhs
	//    x : max(n,m) x rhs
	//
	//    x can be the same address with b
	template<typename UType, typename BType, typename XType>
	auto inline s_householder_u_q_dot(Size m, Size n, Size rhs, const double *U, UType u_t, const double *b, BType b_t, double *x, XType x_t)noexcept->void {
		s_mc(m, rhs, b, b_t, x, x_t);

		Size i_begin{ std::min({ m - 1, m, n }) };
		for (Size i(i_begin), xi0{ at(i - 1, 0, x_t) }, uii{ at(i - 1, i - 1, u_t) }; --i < i_begin; xi0 = last_r(xi0, x_t), uii = last_d(uii, u_t)) {
			auto U_ia1_i = U + next_r(uii, u_t);
			double tau = (1.0 + s_vv(m - i - 1, U_ia1_i, u_t, U_ia1_i, u_t)) / 2;
			
			for (Size j(-1), xij{ xi0 }; ++j < rhs; xij = next_c(xij, x_t)) {
				auto Xi1j = x + next_r(xij, x_t);
				
				double k = -1.0 / tau * (x[xij] + s_vv(m - i - 1, U_ia1_i, u_t, Xi1j, x_t));
				x[xij] += k;
				s_ma(m - i - 1, 1, k, U_ia1_i, u_t, Xi1j, x_t);
			}
		}
	}
	auto inline s_householder_u_q_dot(Size m, Size n, Size rhs, const double *U, const double *b, double *x)noexcept->void { s_householder_u_q_dot(m, n, rhs, U, n, b, rhs, x, rhs); }

	//    x = Q^T * b
	//
	//    U :        m x n
	//    b :        m x rhs
	//    x :        m x rhs
	//
	//    x can be the same address with b
	template<typename UType, typename BType, typename XType>
	auto inline s_householder_u_qt_dot(Size m, Size n, Size rhs, const double *U, UType u_t, const double *b, BType b_t, double *x, XType x_t)noexcept->void{
		s_mc(m, rhs, b, b_t, x, x_t);

		for (Size i(-1), xi0{ 0 }, uii{ 0 }; ++i < std::min({ m - 1, m, n }); xi0 = next_r(xi0, x_t), uii = next_d(uii, u_t)){
			auto U_ia1_i = U + next_r(uii, u_t);
			double tau = (1.0+s_vv(m - i - 1, U_ia1_i, u_t, U_ia1_i, u_t))/2;

			for (Size j(-1), xij{ xi0 }; ++j < rhs; xij = next_c(xij, x_t)){
				auto X_ia1_j = x + next_r(xij, x_t);

				double k = -1.0/tau * (x[xij] + s_vv(m - i - 1, U_ia1_i, u_t, X_ia1_j, x_t));
				x[xij] += k;
				s_ma(m - i - 1, 1, k, U_ia1_i, u_t, X_ia1_j, x_t);
			}
		}
	}
	auto inline s_householder_u_qt_dot(Size m, Size n, Size rhs, const double *U, const double *tau, const double *b, double *x)noexcept->void { s_householder_u_qt_dot(m, n, rhs, U, n, b, rhs, x, rhs); }

	// solve decomposition of A * P
	//    A :        m x n
	//    U :        m x n
	//    p : max(m,n) x 1
	//
	//    U can be the same address with A
	template<typename AType, typename UType>
	auto inline s_householder_up(Size m, Size n, const double *A, AType a_t, double *U, UType u_t, Size *p, Size &rank, double zero_check = 1e-10)noexcept->void {
		rank = 0;

		// init u //
		s_mc(m, n, A, a_t, U, u_t);

		// init p //
		std::iota(p, p + n, 0);

		// compute //
		for (Size i(-1), uii{ 0 }, ti{ 0 }; ++i < std::min(m, n); uii = next_d(uii, u_t)) {
			// 找到模最大的一列 //
			double max_value{ 0 };
			Size max_pos{ i };

			for (Size j(i - 1), uij{ uii }; ++j < n; uij = next_c(uij, u_t)) {
				double value = s_vv(m - i, U + uij, u_t, U + uij, u_t);
				max_pos = value > max_value ? j : max_pos;
				max_value = value > max_value ? value : max_value;
			}

			// 判断是否返回 //
			max_value = std::sqrt(max_value);
			if (max_value < zero_check) return;

			++rank;
			s_swap_v(m, U + at(0, max_pos, u_t), u_t, U + at(0, i, u_t), u_t);
			std::swap(p[max_pos], p[i]);

			// 若已经到达最后一行，那么就返回，因为最后一行不需要householder化 //
			if (i == m - 1) return;

			// compute householder vector //
			auto U_i1_i = U + next_r(uii, u_t);

			double rho = -max_value * s_sgn2(U[uii]);
			double tau = U[uii] / rho - 1.0;
			s_nm(m - 1 - i, 1, 1.0 / (U[uii] - rho), U_i1_i, u_t);
			U[uii] = rho;

			// update matrix //
			for (Size j(i), uij(next_c(uii, u_t)); ++j < n; uij = next_c(uij, u_t))
			{
				auto U_i1_j = U + next_r(uij, u_t);

				double k = tau * (s_vv(m - i - 1, U_i1_i, u_t, U_i1_j, u_t) + U[uij]);
				U[uij] += k;
				s_va(m - i - 1, k, U_i1_i, u_t, U_i1_j, u_t);
			}
		}
	}
	auto inline s_householder_up(Size m, Size n, const double *A, double *U, Size *p, Size &rank, double zero_check = 1e-10)noexcept->void { s_householder_up(m, n, A, n, U, n, p, rank, zero_check); }

	//    U :        m x n
	//    p : max(m,n) x 1
	//    b :        m x rhs
	//    x : max(n,m) x rhs
	//
	//    x can be the same address with b
	template<typename UType, typename BType, typename XType>
	auto inline s_householder_up_sov(Size m, Size n, Size rhs, Size rank, const double *U, UType u_t, const Size *p, const double *b, BType b_t, double *x, XType x_t, double zero_check = 1e-10)noexcept->void{
		s_householder_u_qt_dot(m, rank, rhs, U, u_t, b, b_t, x, x_t);
		s_sov_um(rank, rhs, U, u_t, x, x_t, x, x_t, zero_check);
		s_fill(n - rank, rhs, 0.0, x + at(rank, 0, x_t), x_t);
		s_permutate_inv(n, rhs, p, x, x_t);
	}
	auto inline s_householder_up_sov(Size m, Size n, Size rhs, Size rank, const double *U, const Size *p, const double *b, double *x, double zero_check = 1e-10)noexcept->void { s_householder_up_sov(m, n, rhs, rank, U, n, p, b, rhs, x, rhs, zero_check); }
	
	//    U :        m x n
	//  tau : max(m,n) x 1
	//    p : max(m,n) x 1
	//    x :        n x m
	// tau2 : max(n,m) x 1
	template<typename UType, typename TauType, typename XType, typename TauType2>
	auto inline s_householder_up2pinv(Size m, Size n, Size rank, const double *U, UType u_t, const double *tau, TauType tau_t, const Size *p, double *x, XType x_t, double *tau2, TauType2 t_t, double zero_check = 1e-10)noexcept->void
	{
		// X 是 A 的 moore penrose 逆，为 n x m 维
		//
		// A 的qr分解:
		// A_mn * P_nn = Q_mxm * R_mxn
		// 若A的秩为r
		// 此时R_mn 为：
		//     1           r   r+1     n
		// 1   [             .         ]
		//     |    R1       .    R2   |
		// r   |             .         |
		//     |.......................|
		// r+1 |             .         |
		//     |             .         |
		// m   [             .         ] 
		// 
		// A*P*x = b的最小范数的最小二乘解（linear least square:lls）为：
		// 
		// c = QT * b
		// y = P * x
		// 
		// y的某一个特解为y_t：
		// 1   [         ]
		//     |  R1\c1  |
		// r   |         |
		//     |.........|
		// r+1 |         |
		//     |         |
		// n   [    0    ]
		// 
		// y的通解为y_s：
		//      1         n-r
		// 1   [             ]
		// r   |    R1\R2    |
		//     | ............|
		// r+1 | -1          |           
		//     |    -1       |
		//     |       ..    |
		// n   [          -1 ]
		// 
		// 于是y的lls解中需求通解y_s的系数，它以下方程的最小二乘解：
		// 
		// y_s * k = y_t
		// k =  y_s\(R1\c1) = y_s \ y_t
		//
		// 于是y的lls解为：
		// y = y_t - y_s*(y_s \ y_t）=(I - y_s * y_s^-1) y_t
		// x = P * y
		//
		// 若求广义逆，那么令b为mxm的单位阵，此时所求出x即为广义逆A+：
		// 同时令QR分解矩阵为：
		// A_mn * P_nn = Q_mxr * R_rxn
		// 此时y_t：
		//      1            m
		// 1   [              ]
		// r   |  R1\Q_mxr^T  |
		//     |..............|
		// r+1 |              |
		// n   [    0         ]
		//
		// 而此时y_s可以进行qr分解：
		// y_s = S_nxn * T_nxn-r
		// 其中 T_nxn-r：
		//     1     n-r               
		//  1  [      |           
		//     |  T1  |
		// n-r |......|
		//     |      |
		//  n  [  T2  ]
		// 这里T_nxn-r ^ -1为：
		// 
		//     1       n-r    n               
		//  1  [        .     ]           
		//     | T1^-1  .     |
		// n-r [        .     ]
		// 
		// 于是 y_s * y_s ^ -1 为：
		//   S * T * T^-1 *S^T
		// = S(:,1:n-r) * S(:,1:n-r) ^ T
		// 
		// 而 I - y_s * y_s^-1 = S*S^T - S(:,1:n-r) * S(:,1:n-r) ^ T
		//  = S(:,n-r+1:end) * S(:,n-r+1:end) ^ T
		// 带入上式：
		// y = S(:,n-r+1:end)_nxr * S(:,n-r+1:end)^T_rxn * y_t
		//   = S(:,n-r+1:end)_nxr * S(1:r,n-r+1:end)^T_rxr * (R1\Q^T)_rxm
		// 
		// x = P^-1 * y;
		// 
		//


		// step 1:
		// change x to:
		//     1           r         m
		// 1   [                     ]
		// r   |         R1\QT       |
		//     |.....................|
		// r+1 | (R1\R2)^T   .       |
		// n   [             .       ] 
		//
		//

		// QT
		s_householder_ut2qmn(m, rank, U, u_t, tau, tau_t, x, T(x_t));
		// R1\QT
		s_sov_um(rank, m, U, u_t, x + at(0, 0, x_t), x_t, x, x_t, zero_check);
		// R1\R2
		s_sov_um(rank, n - rank, U, u_t, U + at(0, rank, u_t), u_t, x + at(rank, 0, x_t), T(x_t), zero_check);

		// step 2:
		// 将通解矩阵做个行变换如下：
		//
		//     1           n-r         
		//  1  [ -1          |           
		//     |    -1       |
		//     |       ..    |
		//     |          -1 |
		// n-r | ............|
		//     |    R1\R2    |
		//  n  [             ]
		//
		// 其household变换产生的U（记作S）和tau有如下特征：
		//     1           n-r         
		//  1  [ *  *  *  * |
		//     |    *  *  * |
		//     |       *  * |
		//     |          * |
		// n-r |............|
		//     | *  *  *  * |
		//  n  [ *  *  *  * ]
		//
		//   1       n-r
		// [ *  *  *  * ]
		// 
		// 这里将上述S的下三角部分储存在x中
		//
		// x变成:
		//   1           r         m
		// 1 [                     ]
		//   |         R1\QT       |
		// r |.....................|
		//   |    S      .         |
		// n [           .         ]
		//
		//
		// make S and tau
		for (Size i(-1), k0i{ at(0, rank, T(x_t)) }, ti{ 0 }; ++i < std::min({ n - rank, n, n - 1 }); k0i = next_c(k0i, T(x_t)), ti = next_r(ti, t_t))
		{
			double rho = std::sqrt(s_vv(rank, x + k0i, T(x_t), x + k0i, T(x_t)) + 1.0);

			// Aii 为 -1.0
			s_nv(rank, 1.0 / (-1.0 - rho), x + k0i, T(x_t));
			tau2[ti] = -1.0 / rho - 1.0;

			for (Size j(i), kij{ next_c(k0i,T(x_t)) }; ++j < n - rank; kij = next_c(kij, T(x_t)))
			{
				double k = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + kij, T(x_t)));
				s_va(rank, k, x + k0i, T(x_t), x + kij, T(x_t));
			}
		}

		// step 3:
		// 利用S产生的QT来乘以R1\QT，S事实上为一个很大的矩阵，但其左下角很多为0，因此每一列只用做rank维的乘法
		//
		// x变成:
		//   1           r         m
		// 1 [                     ]
		//   |     ST * (R1\QT)    |
		// r |.....................|
		//   |     S     .         |
		// n [           .         ]  
		//
		for (Size i(-1), k0i{ at(0, rank, T(x_t)) }, ti{ 0 }; ++i < n - rank; k0i = next_c(k0i, T(x_t)), ti = next_r(ti, t_t)){
			for (Size j(-1), x0j{ at(0, 0, x_t) }; ++j < m; x0j = next_c(x0j, x_t)){
				double alpha = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + x0j, x_t));
				s_ma(rank, 1, alpha, x + k0i, T(x_t), x + x0j, x_t);
			}
		}

		// step 4:
		// 利用S产生的Q来乘以ST（R1\QT），这里防止S被覆盖，因此局部会用tau(n-r+1:n)的内存来局部存储
		//
		// x变成:
		//   1                     m
		// 1 [                     ]
		//   |                     |
		//   |   S * ST * (R1\QT)  |
		//   |                     |
		// n [                     ]
		for (Size i(n - rank), k0i{ at(0, n - 1, T(x_t)) }, ti{ at(n - rank - 1, t_t) }; --i < n - rank; k0i = last_c(k0i, T(x_t)), ti = last_r(ti, t_t)){
			// 因为需要少占内存，因此先将
			for (Size j(-1), x0j{ at(0, 0, x_t) }, tj{ at(n - rank,t_t) }; ++j < rank; x0j = next_c(x0j, x_t), tj = next_r(tj, t_t))
			{
				double alpha = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + x0j, x_t));
				s_ma(rank, 1, alpha, x + k0i, T(x_t), x + x0j, x_t);
				tau2[tj] = alpha;
			}
			for (Size j(rank - 1), x0j{ at(0, rank, x_t) }; ++j < m; x0j = next_c(x0j, x_t))
			{
				double alpha = tau2[ti] * (s_vv(rank, x + k0i, T(x_t), x + x0j, x_t));
				s_ma(rank, 1, alpha, x + k0i, T(x_t), x + x0j, x_t);
				x[at(i + rank, j, x_t)] = alpha;
			}

			s_vc(rank, tau2 + at(n - rank, t_t), t_t, x + k0i, T(x_t));
		}

		// step 5:
		// permutate
		s_permutate_inv(n, m, p, x, x_t);
	}
	auto inline s_householder_up2pinv(Size m, Size n, Size rank, const double *U, const double *tau, const Size *p, double *x, double *tau2, double zero_check = 1e-10)noexcept->void { s_householder_up2pinv(m, n, rank, U, n, tau, 1, p, x, m, tau2, 1, zero_check); }


	extern double *global_U, *global_V, *global_S;

	//   
	template<typename AType, typename UType, typename SType, typename VType>
	auto inline s_svd(Size m, Size n, const double *A, AType a_t, double *U, UType u_t, double *S, SType s_t, double *V, VType v_t, double zero_check = 1e-10)noexcept->void
	{




		//step 6：计算V
		//	令 vi 位于 U 的第一行，并计算vi 
		//	
		//	V(1  :h  , p_c(i))      = V1*vi(2:h+1);
		//  V(h+1    , p_c(i))      = vi(1);
		//  V(h+2:n  , p_c(i))      = V2*vi(h+2:n);

		//	此后不再需要V1 V2，内存更新为：
		//	--------------------------------------------
		//	U：
		//		   n-h-1  h+1   
		//	h+1 [ EMPTY     ]              
		//	n-h [ EMPTY     ]
		//		    h     n-h
		//	--------------------------------------------
		//	S：
		//     1      h  h+1    n 
		//   [ u1 ... u1 d ... z ]
		//   | .  ... .  . ... . |
		//   | u1 ... u1 d ... z |
		//   | u2 ... u2 d ... z |
		//   | u2 ... u2 d ... z |
		//   [ u2 ... u2 d ... z ]
		//	--------------------------------------------
		//	V:
		//	[ RETURN V ]
		//	--------------------------------------------
		//step 7：计算U
		//	
		//	
		//	
		// final:return s u v



		
		///////////////////////////// part 1 dvc/////////////////////////////////////////
		// 迭代 
		// [U S V] = dvc(A)
		// S&A: n+1 x n   
		// U  : n+1 x n+1
		// V  :   n x n
		// S 和 A 位于同一片内存

		// A 为 双对角矩阵：
		// [ *         ]
		// | * *       |
		// |   * *     |
		// |     * *   |
		// |       * * |
		// [         * ]

		//但是在内存中储存为，记做：
		//[ * *  ... * ]
		//| * *  ... * |            
		//|            |
		//[            ]
		const auto dvc = [](Size n, double *U, UType u_t, double *S, UType s_t, double *V, VType v_t, const auto&dvc)->void {
			
			static int num = 0;
			int local_num = ++num;
			std::cout << "global usv:" << local_num << std::endl;
			dsp(9, 9, global_U);
			dsp(9, 8, global_S);
			dsp(8, 8, global_V);
			
			if (n == 1) {
				auto &a1 = S[at(0, 0, s_t)];
				auto &a2 = S[at(1, 0, s_t)];
				
				auto theta = std::atan2(a2, a1);
				S[at(0, 0, s_t)] = std::sqrt(a1 * a1 + a2 * a2);

				U[at(0, 0, u_t)] = std::cos(theta);
				U[at(0, 1, u_t)] = -std::sin(theta);
				U[at(1, 0, u_t)] = std::sin(theta);
				U[at(1, 1, u_t)] = std::cos(theta);

				V[0] = 1;
			}
			else if (n == 2){
				auto theta1 = std::atan2(S[at(1, 0, s_t)], S[at(0, 0, s_t)]);
				auto c1 = std::cos(theta1);
				auto s1 = std::sin(theta1);

				auto theta2 = std::atan2(S[at(1, 1, s_t)], c1 * S[at(0, 1, s_t)]);
				auto c2 = std::cos(theta2);
				auto s2 = std::sin(theta2);

				auto a = c1 * S[at(0, 0, s_t)] + s1 * S[at(1, 0, s_t)];
				auto b = s1 * S[at(0, 1, s_t)];
				auto d = c2 * c1 * S[at(0, 1, s_t)] + s2 * S[at(1, 1, s_t)];

				auto theta3 = 0.5* std::atan2(2*b*d, a*a + b*b - d*d);
				auto c3=std::cos(theta3);
				auto s3=std::sin(theta3);

				U[at(0, 0, u_t)] = c1*c3 - c2*s1*s3;
				U[at(0, 1, u_t)] = -c1*s3 - c2*c3*s1;
				U[at(0, 2, u_t)] = s1*s2;
				U[at(1, 0, u_t)] = c3*s1 + c1*c2*s3;
				U[at(1, 1, u_t)] = c1*c2*c3 - s1*s3;
				U[at(1, 2, u_t)] = -c1*s2;
				U[at(2, 0, u_t)] = s2*s3;
				U[at(2, 1, u_t)] = c3*s2;
				U[at(2, 2, u_t)] = c2;

				auto S1=a*a + b*b + d*d;
				auto S2=std::sqrt((a*a + b*b - d*d)*(a*a + b*b - d*d) + 4*b*b*d*d);

				S[at(0, 0, s_t)] = std::sqrt((S1 + S2) / 2);
				S[at(0, 1, s_t)] = std::sqrt((S1 - S2) / 2);

				auto phi = 0.5*std::atan2(2*a*b,a*a - b*b - d*d);
				auto c_p = std::cos(phi);
				auto s_p = std::sin(phi);
				auto s11 = (a*c3)*c_p + (b*c3+d*s3)*s_p;
				auto s22 = (a*s3)*s_p + (-b*s3+d*c3)*c_p;
				
				V[at(0, 0, v_t)] = s_sgn2(s11) * c_p;
				V[at(0, 1, v_t)] = -s_sgn2(s22) * s_p;
				V[at(1, 0, v_t)] = s_sgn2(s11) * s_p;
				V[at(1, 1, v_t)] = s_sgn2(s22) * c_p;
			} 
			else {
				//  step 1：递归调用
				//  h = n/2, n为偶数；或 (n-1)/2, n为奇数
				//  [U2 S2 V2] = dvc(A( h+2:n+1 , h+2:n ));
				//  [U1,S1,V1] = dvc(A(   1:h+1 , 1:h   ));
				//
				//  各矩阵维度：  U1:h+1 x h+1    S1:h+1 x h        V1:    h x h
				//                U2:n-h x n-h    S2:n-h x n-h-1    V2:n-h-1 x n-h-1
				//
				//  调用前的内存布局：
				//	--------------------------------------------
				//	U：
				//	[ EMPTY ]
				//	--------------------------------------------
				//	S：
				//	      h   1   n-h-1 
				//	2   [ A1  ek  A2   ]
				//	n-1 [ EMPTY....... ]
				//	--------------------------------------------
				//	V:
				//	[ empty ]
				//	--------------------------------------------
				//  调用后的内存布局：
				//	--------------------------------------------
				//	U：
				//	    1   h   n-h-1 
				//	1 [ 0  S1  S2   ]
				//	n [ EMPTY.......]
				//	--------------------------------------------
				//	S：
				//       1      h  h+1    n 
				//  1  [ u1 ... u1 v1 ... v1 ]
				//     | .  ... .  .  ... .  |
				// h+1 | u1 ... u1 v1 ... v1 |
				// h+2 | u2 ... u2 v2 ... v2 |
				//     | u2 ... u2 v2 ... v2 |
				// n+1 [ u2 ... u2 v2 ... v2 ]
				//	--------------------------------------------
				//	V:
				//	[ empty ]
				//	--------------------------------------------
				//
				//	V：
				//    n 为奇数时：  此时 mU1 == mU2 (h+1 == n-h)
				//           1      h   h+1    h+2    n
				//      1  [ u1 ... u1  u1     v1 ... v1 ]
				//         | .  ... .   .      v1 ... v1 |
				//     h+1 | u1 ... u1  u1&u2  u2 ... u2 |
				//     h+2 | v2 ... v2  .      .  ... .  |
				//         | .  ... .   .      .  ... .  |
				//      n  [ v2 ... v2  u2     u2 ... u2 ]
				//
				//    n 为偶数时：  此时 mU1 == mU2+1 (h+1 == n-h + 1)
				//           1      h-1  h      h+1    h+2    n-1 n
				//      1  [ u1 ... u1   u1     u1     v1 ... v1  v1]
				//         | .  ... .    .      .      .  ... .   . |
				//     h-1 | .  ... .    .      u1     v1 ... v1  v1|
				//     h+1 | u1 ... u1   u1&v1  u1&u2  u2 ... u2  u2|
				//     h+2 | v2 ... v2   v1     u2     .  ... .   . |
				//         | .  ... .    .      .      .  ... .   . |
				//      n  [ v2 ... v2   v1     u2     u2 ... u2  u2]
				//
				/////////////////
				//   WARNING:  //
				/////////////////
				//    此时，V1的最后一列储存中间下方

				auto h    = n / 2;
				auto U1   = V + at(0,     0,     v_t);
				auto u1_t = v_t;
				auto S1   = S + at(0,     0,     s_t);
				auto s1_t = s_t;
				auto V1   = V + at(0,     h + 1, v_t);
				auto v1t  = v_t;
				auto U2   = V + at(h,     h,     v_t);
				auto u2_t = v_t;
				auto S2   = S + at(0,     h + 1, s_t);
				auto s2_t = s_t;
				auto V2   = V + at(h + 1, 0,     v_t);
				auto v2t  = v_t;

				//s_mc(2, h,         S + at(0, 0,     u_t), s_t, S1 + at(0, 0, s1_t), s1_t);
				//s_mc(2, n - h - 1, S + at(0, h + 1, u_t), s_t, S2 + at(0, 0, s2_t), s2_t);
				
				constexpr auto a_h_h   = S[at(0, h, s_t)];
				constexpr auto a_hp1_h = S[at(1, h, s_t)];

				//s_fill(n + 1, n, -99.0, S, s_t);



				dvc(n - h - 1, U2, u2_t, S2, s2_t, V2, v2t, dvc);
				dvc(h, U1, u1_t, S1, s1_t, V1, v1t, dvc);

				//step 2：构造 d、z、p
				//
				//d 和 z 构成了 M：
				//[ z1            ]
				//| z2  d2        |
				//| ...    ...    |
				//[ zn         dn ]
				//
				//p 让 d 正序排列，
				//此时内存S有变化，其他不变：
				//U：
				//	     1 [ d....       ]
				//         | z....       |        
				//	       | p....       |
				//		   | mu...       |
				//	       | EMPTY...    |
				//	   n+1 [    0        ]
				auto r0 = std::sqrt(a_h_h*a_h_h*U1[at(h, h, u1_t)] * U1[at(h, h, u1_t)] + a_hp1_h * a_hp1_h*U2[at(0, n - h - 1, u2_t)] * U2[at(0, n - h - 1, u2_t)]);
				auto theta = std::atan2(a_hp1_h*U2[at(0, n - h - 1, u2_t)], a_h_h*U1[at(h, h, u1_t)]);
				auto c0 = std::cos(theta);
				auto s0 = std::sin(theta);

				//double p_data_[10];
				//Size p_t_ = 10;
				auto d = U;
				auto d_t = u_t;
				auto z = U + at(1, 0, u_t);
				auto z_t = u_t;
				auto mu = U + at(2, 0, u_t);
				auto mu_t = u_t;
				auto p = U + at(3, 0, u_t);
				auto p_t = u_t;
				//auto p = &*p_data_;
				//auto p_t = p_t_;

				d[0] = 0.0;
				
				z[0] = r0;
				s_mc(1, h, a_h_h, U1 + at(h, 0, u1_t), z + at(0, 1, z_t));
				s_mc(1, n - h - 1, a_hp1_h, U2 + at(0, 0, u2_t), z + at(0, h + 1, z_t));

				for (auto i = 0; i < n; ++i)p[at(0, i, p_t)] = i;
				std::sort(RowIterator<UType>(p, p_t), RowIterator<UType>(p, p_t) + n, [&d, &d_t](const auto &left, const auto &right) {
					return d[at(0, static_cast<Size>(left), d_t)] < d[at(0, static_cast<Size>(right), d_t)];
				});
				// d和z两个一起变换
				s_permutate(n, 2, RowIterator<UType>(p, p_t), d, T(d_t));

				//step 3：deflation
				constexpr auto max_abs_z = std::abs(*std::max_element(RowIterator<UType>(z, z_t), RowIterator<UType>(z, z_t) + n, [](const auto &l, const auto &r) {return std::abs(l) < std::abs(r); }));
				constexpr auto max_d = d[at(0, n - 1, d_t)];
				constexpr auto consider_zero = std::numeric_limits<double>::min();
				constexpr auto epsilon_strict = std::max(consider_zero, std::numeric_limits<double>::epsilon() * max_d);
				constexpr auto epsilon_coarse = std::max(consider_zero, 8.0 * std::numeric_limits<double>::epsilon() * std::max(max_abs_z, max_d));

				auto Mn = n;
				for (Size i = 1; i < Mn; ){
					// check zi near zero, deflation type 2
					if (std::abs(z[at(0, i, z_t)]) < epsilon_strict) {
						mu[at(0, Mn - 1, mu_t)] = -2;
						auto tem = d[at(0, i, d_t)];
						s_mc(1, Mn - i, d + at(0, i + 1, d_t), d_t, d + at(0, i, d_t), d_t);
						d[at(0, Mn - 1, d_t)] = tem;
						tem = z[at(0, i, z_t)];
						s_mc(1, Mn - i, z + at(0, i + 1, z_t), z_t, z + at(0, i, z_t), z_t);
						z[at(0, Mn - 1, z_t)] = tem;
						tem = p[at(0, i, d_t)];
						s_mc(1, Mn - i, p + at(0, i + 1, p_t), p_t, p + at(0, i, p_t), p_t);
						p[at(0, Mn - 1, p_t)] = tem;
						--Mn;

						std::cout << "-----------------------------------" << std::endl;
						std::cout << "deflation 2" << std::endl;
						dsp(1, n, mu, mu_t);
						dsp(1, n, d, d_t);
						dsp(1, n, z, z_t);
						dsp(1, n, p, p_t);
						std::cout << "-----------------------------------" << std::endl;

						continue;
					}
					else if(std::abs(d[at(0, i, d_t)])< epsilon_coarse){
						mu[at(0, Mn, mu_t)] = -1;
						auto z1 = std::sqrt(z[at(0, 0, z_t)] * z[at(0, 0, z_t)] + z[at(0, i, z_t)] * z[at(0, i, z_t)]);
						auto zi = std::atan2(z[at(0, i, z_t)], z[at(0, 0, z_t)]);
						z[at(0, 0, z_t)] = z1;
						z[at(0, i, z_t)] = zi;
						auto tem = d[at(0, i, d_t)];
						s_mc(1, Mn - i, d + at(0, i + 1, d_t), d_t, d + at(0, i, d_t), d_t);
						d[at(0, Mn - 1, d_t)] = tem;
						tem = z[at(0, i, z_t)];
						s_mc(1, Mn - i, z + at(0, i + 1, z_t), z_t, z + at(0, i, z_t), z_t);
						z[at(0, Mn - 1, z_t)] = tem;
						tem = p[at(0, i, d_t)];
						s_mc(1, Mn - i, p + at(0, i + 1, p_t), p_t, p + at(0, i, p_t), p_t);
						p[at(0, Mn - 1, p_t)] = tem;
						--Mn;

						std::cout << "-----------------------------------" << std::endl;
						std::cout << "deflation 3" << std::endl;
						dsp(1, n, mu, mu_t);
						dsp(1, n, d, d_t);
						dsp(1, n, z, z_t);
						dsp(1, n, p, p_t);
						std::cout << "-----------------------------------" << std::endl;

						continue;
					}
					else if (d[at(0, i, d_t)] - d[at(0, i - 1, d_t)] < epsilon_strict) {
						mu[at(0, Mn, mu_t)] = i-1;
						auto z_im1 = std::sqrt(z[at(0, i, z_t)] * z[at(0, i, z_t)] + z[at(0, i - 1, z_t)] * z[at(0, i - 1, z_t)]);
						auto z_i = std::atan2(z[at(0, i, z_t)], z[at(0, i - 1, z_t)]);
						z[at(0, z_t)] = z_im1;
						z[at(i, z_t)] = z_i;
						auto tem = d[at(0, i, d_t)];
						s_mc(1, Mn - i, d + at(0, i + 1, d_t), d_t, d + at(0, i, d_t), d_t);
						d[at(0, Mn - 1, d_t)] = tem;
						tem = z[at(0, i, z_t)];
						s_mc(1, Mn - i, z + at(0, i + 1, z_t), z_t, z + at(0, i, z_t), z_t);
						z[at(0, Mn - 1, z_t)] = tem;
						tem = p[at(0, i, d_t)];
						s_mc(1, Mn - i, p + at(0, i + 1, p_t), p_t, p + at(0, i, p_t), p_t);
						p[at(0, Mn - 1, p_t)] = tem;

						std::cout << "-----------------------------------" << std::endl;
						std::cout << "deflation 4" << std::endl;
						dsp(1, n, mu, mu_t);
						dsp(1, n, d, d_t);
						dsp(1, n, z, z_t);
						dsp(1, n, p, p_t);
						std::cout << "-----------------------------------" << std::endl;

						--Mn;
					}
					else
					{
						++i;
					}
				}

				if (Mn == 1){
					mu[at(0, Mn - 1, mu_t)] = -2;
					Mn = 0;
					d[at(0,0, d_t)] = z[at(0, 0, z_t)];
					d[at(0,n, d_t)] = z[at(0,0, z_t)];

					std::cout << "-----------------------------------" << std::endl;
					std::cout << "deflation 0" << std::endl;
					dsp(1, n, mu, mu_t);
					dsp(1, n, d, d_t);
					dsp(1, n, z, z_t);
					dsp(1, n, p, p_t);
					std::cout << "-----------------------------------" << std::endl;
				}
				else {
					if (z[at(0,0, z_t)] <= epsilon_coarse)
						z[at(0,0, z_t)] = epsilon_coarse;
					
					d[at(0,n, d_t)] = d[at(0, Mn, d_t)] + s_vv(Mn, z, z_t, z, z_t);

					std::cout << "-----------------------------------" << std::endl;
					std::cout << "deflation 1" << std::endl;
					dsp(1, n, mu, mu_t);
					dsp(1, n, d, d_t);
					dsp(1, n, z, z_t);
					dsp(1, n, p, p_t);
					std::cout << "-----------------------------------" << std::endl;
				}

				//step 4：计算奇异值
				for (Size i{ 0 }, di{ 0 }; i < Mn; ++i, di = next_c(di, d_t)) {
					auto left = d[di];
					auto right = i == Mn-1 ? d[at(0, n, d_t)] : d[next_c(di, d_t)];
					auto mid = 0.5*(left + right);
					auto v = 0.0;
					for (Size j = 0; j < Mn; ++j) {
						v += z[at(0, j, z_t)] * z[at(0, j, z_t)] / (d[at(0, j, d_t)] - mid) / (d[at(0, j, d_t)] + mid);
					}

					auto base = v < -1 ? right : left;
					auto lower = v < -1 ? left - right : 0.0;
					auto upper = v < -1 ? 0.0 : right - left;

					while (std::abs(lower - upper) >(std::max(std::abs(lower) ,std::abs(upper))) * 2 * std::numeric_limits<double>::epsilon()) {
						mid = (lower + upper) / 2;
						auto v = 0.0;
						for (Size j = 0; j < Mn; ++j) {
							v += z[at(0, j, z_t)] * z[at(0, j, z_t)] / (d[at(0, j, d_t)] - base - mid) / (d[at(0, j, d_t)] + base + mid);
						}
						v < -1 ? lower = mid : upper = mid;
					}
					mu[at(0, i, mu_t)] = (lower + upper) / 2;
				}

				dsp(1, n, mu, mu_t);
				dsp(1, n, d, d_t);
				dsp(1, n, z, z_t);
				dsp(1, n, p, p_t);

				//step 5：重新计算 z
				for (Size i = 0, d_i = 0, z_i = 0; i < Mn; ++i, d_i = next_c(d_i, d_t), z_i = next_c(z_i, z_t)) {
					auto base = mu[at(0, Mn - 1, mu_t)] < 0.0 ? d[at(0, n, d_t)] : d[at(0, Mn - 1, d_t)];
					auto zi = (base + d[at(0, i, d_t)] + mu[at(0, Mn - 1, mu_t)])*(base - d[at(0, i, d_t)] + mu[at(0, Mn - 1, mu_t)]);
					
					for (Size k = 0, mu_k = 0, d_k = 0, d_kp1 = next_c(d_k,d_t); k < i; ++k, mu_k = next_c(mu_k, mu_t), d_k = d_kp1, d_kp1 = next_c(d_k, d_t)){
						base = mu[mu_k] < 0 ? d[d_kp1] : d[d_k];
						zi *= (base - d[d_i] + mu[mu_k]) * (base + d[d_i] + mu[mu_k]) / (d[d_k] - d[d_i]) / (d[d_k] + d[d_i]);
					}

					for (Size k = i, mu_k = 0, d_k = 0, d_kp1 = next_c(d_k, d_t); k < Mn - 1; ++k, mu_k = next_c(mu_k, mu_t), d_k = d_kp1, d_kp1 = next_c(d_k, d_t)) {
						base = mu[mu_k] < 0 ? d[d_kp1] : d[d_k];
						zi *= (base - d[d_i] + mu[mu_k]) * (base + d[d_i] + mu[mu_k]) / (d[d_kp1] - d[d_i]) / (d[d_kp1] + d[d_i]);
					}

					z[z_i] = s_sgn2(z[z_i]) * std::sqrt(zi);
				}

				//dsp(1, n, mu, mu_t);
				//dsp(1, n, d, d_t);
				//dsp(1, n, z, z_t);
				//dsp(1, n, p, p_t);

				//step 6：计算V
				//auto vi = V;
				//auto vi_t = T(v_t);
				double vi[10];
				auto vi_t = T(10);
				for (Size i = 0; i < n; ++i) {
					if (i < Mn) {
						auto base = mu[at(0, i, mu_t)] < 0.0 ? (i == Mn - 1 ? d[at(0, n, d_t)] : d[at(0, i + 1, d_t)]) : d[at(0, i, d_t)];
						
						for (Size j = 1; j < n; ++j) {
							vi[at(j, 0, vi_t)] = j < Mn ? d[at(0, j, d_t)] * z[at(0, j, z_t)] / (d[at(0, j, d_t)] - base - mu[at(0, i, mu_t)]) / (d[at(0, j, d_t)] + base + mu[at(0, i, mu_t)]) : 0.0;
						}
					}
					else {
						s_fill(n, 1, 0.0, vi, vi_t);
						vi[at(i, 0, vi_t)] = 1.0;
					}
					
					// apply deflation
					for (Size j = Mn; j < n; ++j) {
						
						if (mu[at(0, j, mu_t)] == -2) {
						}
						else if (mu[at(0, j, mu_t)] == -1) {
							// type 3
						}
						else {
							// type 4
							auto via = std::cos(z[at(0, j, z_t)]) * vi[static_cast<Size>(mu[at(0, j, mu_t)])] - std::sin(z[at(0, j, z_t)]) * vi[static_cast<Size>(mu[at(0, j, mu_t)])];
							auto vib = std::sin(z[at(0, j, z_t)]) * vi[static_cast<Size>(mu[at(0, j, mu_t)])] + std::cos(z[at(0, j, z_t)]) * vi[static_cast<Size>(mu[at(0, j, mu_t)])];
							vi[at(static_cast<Size>(mu[at(0, j, mu_t)]), 0, vi_t)] = via;
							vi[at(j, 0, vi_t)] = vib;
						}
					}

					// apply permutation
					s_permutate_inv(n, 1, RowIterator<UType>(p, p_t), vi, vi_t);
					
					if (i < Mn) {
						vi[at(0, 0, vi_t)] = -1.0;
						s_nm(n, 1, 1.0 / s_norm(n, vi, vi_t), vi, vi_t);
					}
					
					//std::cout << "vi:" << std::endl;
					//dsp(1, n, vi, T(vi_t));
					//dsp(1, n, p, p_t);
					//dsp(n, n, V, v_t);


					//V(1  :h, i) = V1 * vi(2:h + 1);
					//V(h + 1, i) = vi(1);
					//V(h + 2:n, i) = V2 * vi(h + 2:n);
					s_mm(h, 1, h, V1, v1t, vi + at(1, 0, vi_t), vi_t, V + at(0, i, v_t), v_t);
					V[at(h, i, v_t)] = vi[at(0, 0, vi_t)];
					s_mm(n - h - 1, 1, n - h - 1, V2, v2t, vi + at(h + 1, 0, vi_t), vi_t, V + at(h + 1, i, v_t), v_t);
				}
				std::cout << "V:" << std::endl;
				dsp(n, n, V, v_t);

				//step 7: 移动内存置其他位置
				//s_mc(4, n, U, u_t, S, s_t);
				//d = S;
				//auto d_t2 = s_t;
				//z = S + at(1, 0, s_t);
				//auto z_t2 = s_t;
				//mu = S + at(2, 0, s_t);
				//auto mu_t2 = s_t;
				//p = U + at(3, 0, u_t);
				//auto p_t2 = s_t;
				//p = &*p_data_;
				//auto p_t2 = p_t_;


				double data_dzmup[100];
				auto dzmup_t = 20;
				s_mc(4, n, U, u_t, data_dzmup, dzmup_t);

				//dsp(4, n, U, u_t);
				//dsp(4, n, data_dzmup, dzmup_t);

				d = data_dzmup;
				auto d_t2 = dzmup_t;
				z = data_dzmup + at(1, 0, dzmup_t);
				auto z_t2 = dzmup_t;
				mu = data_dzmup + at(2, 0, dzmup_t);
				auto mu_t2 = dzmup_t;
				p = data_dzmup + at(3, 0, dzmup_t);
				auto p_t2 = dzmup_t;





				//step 8：计算U
				double ui[10];
				auto ui_t = T(10);
				for (Size i = 0; i < n; ++i) {
					if (i < Mn) {
						auto base = mu[at(0, i, mu_t2)] < 0.0 ? (i == Mn - 1 ? d[at(0, n, d_t2)] : d[at(0, i + 1, d_t2)]) : d[at(0, i, d_t2)];
						
						for (Size j = 0; j < n; ++j) {
							ui[at(j, 0, ui_t)] = j < Mn ? z[at(0, j, z_t2)] / (d[at(0, j, d_t2)] - base - mu[at(0, i, mu_t2)]) / (d[at(0, j, d_t2)] + base + mu[at(0, i, mu_t2)]) : 0.0;
						}
					}
					else {
						s_fill(n, 1, 0.0, ui, ui_t);
						ui[at(i, 0, ui_t)] = 1.0;
					}

#ifdef ARIS_DEBUG
					std::cout << "ui1111:" << std::endl;
					dsp(1, n, ui, T(ui_t));
#endif
					// apply deflation
					for (Size j = Mn; j < n; ++j) {

						if (mu[at(0, j, mu_t2)] == -2) {
						}
						else if (mu[at(0, j, mu_t2)] == -1) {
							// type 3
							auto uia = std::cos(z[at(0, j, z_t2)]) * ui[at(0, 0, ui_t)] - sin(z[at(0, j, z_t2)]) * ui[at(j, 0, ui_t)];
							auto uib = std::sin(z[at(0, j, z_t2)]) * ui[at(0, 0, ui_t)] + cos(z[at(0, j, z_t2)]) * ui[at(j, 0, ui_t)];
							ui[at(0, 0, ui_t)] = uia;
							ui[at(j, 0, ui_t)] = uib;
						}
						else {
							// type 4
							auto idx = static_cast<Size>(mu[at(0, j, mu_t2)]);
							
							auto uia = std::cos(z[at(0, j, z_t2)]) * ui[at(idx, 0, ui_t)] - sin(z[at(0, j, z_t2)]) * ui[at(j, 0, ui_t)];
							auto uib = std::sin(z[at(0, j, z_t2)]) * ui[at(idx, 0, ui_t)] + cos(z[at(0, j, z_t2)]) * ui[at(j, 0, ui_t)];
							ui[at(idx, 0, ui_t)] = uia;
							ui[at(j, 0, ui_t)] = uib;
						}
					}


#ifdef ARIS_DEBUG
					std::cout << "ui2222:" << std::endl;
					dsp(1, n, ui, T(ui_t));
#endif


					// apply permutation
					s_permutate_inv(n, 1, RowIterator<UType>(p, p_t2), ui, ui_t);




					if (i < Mn) {
						s_nm(n, 1, 1.0 / s_norm(n, ui, ui_t), ui, ui_t);
					}
#ifdef ARIS_DEBUG
					std::cout << "ui3333:" << std::endl;
					dsp(1, n, ui, T(ui_t));

					dsp(h+1, h+1, U1, u1_t);
					dsp(n-h, n-h, U2, u2_t);
#endif
					//U(1  :h + 1, i) = c0 * U1(:, h + 1)*ui(1) + U1(:, 1 : h)*ui(2   : h + 1);
					//U(h + 2:n + 1, i) = s0 * U2(:, n - h)*ui(1) + U2(:, 1 : n - h - 1)*ui(h + 2 : n);

					s_mc(h + 1, 1, c0 * ui[at(0, 0, ui_t)], U1 + at(0, h, u1_t), u1_t, U + at(0, i, u_t), u_t);
					s_mma(h + 1, 1, h, U1 + at(0, 0, u1_t), u1_t, ui + at(1, 0, ui_t), ui_t, U + at(0, i, u_t), u_t);
					
					
					s_mc(n - h, 1, s0 * ui[at(0, 0, ui_t)], U2 + at(0, n - h - 1, u2_t), u2_t, U + at(h + 1, i, u_t), u_t);
					//dsp(n + 1, n + 1, U, u_t);

					s_mma(n - h, 1, n - h - 1, U2 + at(0, 0, u2_t), u2_t, ui + at(h + 1, 0, ui_t), ui_t, U + at(h + 1, i, u_t), u_t);

					//dsp(n + 1, n + 1, U, u_t);
				}
				s_mc(h + 1, 1, -s0, U1 + at(0, h, u1_t), u1_t, U + at(0, n, u_t), u_t);
				s_mc(n - h, 1, c0, U2 + at(0, n - h - 1, u2_t), u2_t, U + at(h + 1, n, u_t), u_t);


#ifdef ARIS_DEBUG
				std::cout << "U:" << std::endl;
				dsp(n + 1, n + 1, U, u_t);
#endif



				//step 8：计算S
				for (Size i = 0; i < n; ++i) {
					if (i < Mn){
						auto base = mu[at(0, i, mu_t)] < 0.0 ? (i == Mn - 1 ? d[at(0, n, d_t2)] : d[at(0, i + 1, d_t2)]) : d[at(0, i, d_t2)];
						S[at(0, i, s_t)] = base + mu[at(0, i, mu_t2)];
					}
					else{
						S[at(0, i, s_t)] = d[at(0, i, d_t2)];
					}

				}

				//std::cout << "final s:" << std::endl;
				//dsp(1, n, S, s_t);

				//s_mm(h, 1, h, U1, u1_t, vi + at(1, 0, vi_t), vi_t);
				//s_mm(h, 1, h, U1, u1_t, vi + at(1, 0, vi_t), vi_t);

				//std::cout << "vi:" << std::endl;
				//dsp(1, n, vi, T(vi_t));
				//dsp(1, n, p, T(p_t));

				//V(1  :h, i) = V1 * vi(2:h + 1);
				//V(h + 1, i) = vi(1);
				//V(h + 2:n, i) = V2 * vi(h + 2:n);
			}


			

#ifdef ARIS_DEBUG

			std::cout << "global usv return:" << local_num << std::endl;
			dsp(9, 9, global_U);
			dsp(9, 8, global_S);
			dsp(8, 8, global_V);


			std::cout << "return usv:" << std::endl;
			dsp(n + 1, n + 1, U, u_t);
			dsp(n + 1, n   , U, u_t);
			dsp(n    , n   , U, u_t);
#endif

			
		};
		
		
		
		for (int i = 0; i < n; ++i)
		{
			S[at(0, i, s_t)] = A[at(i, i, a_t)];
			S[at(1, i, s_t)] = A[at(i + 1, i, a_t)];
		}
		

		dvc(n, U, u_t, S, s_t, V, v_t, dvc);
		
		
		/*
		u1 3*3 
		v1 2*2
		u2 2*2
		v2 1*1






		[ u1 u1 u1      ]
		| u1 u1 u1 
		  u1 u1 u1 u2
		        u2 u2 
		
		
		*/
		
		
		
		
		
		
/*
		// init u //
		s_mc(m, n, A, a_t, U, u_t);

		// make u as bidiagnal //
		for (Size i(-1), uii{ 0 }, ti{ 0 }, t2i{ 0 }; ++i < std::min(m,n); uii = next_d(uii, u_t), ti = next_r(ti, tau_t), t2i = next_r(t2i, tau2_t))
		{
			///////////////////////////////////////////第一次变换////////////////////////////////////////////////////
			{
				Size m1 = n;
				Size n1 = m;
				auto u_t1 = T(u_t);

				// 若已经到达最后一行，因为最后一行不需要householder化 //
				if (i + 1 < m1)
				{
					// compute householder vector //
					auto U_i1_i = U + next_r(uii, u_t1);

					double rho = -std::sqrt(s_vv(m1 - i, U + uii, u_t1, U + uii, u_t1)) * s_sgn2(U[uii]);
					tau[ti] = U[uii] / rho - 1.0;
					s_nm(m1 - 1 - i, 1, 1.0 / (U[uii] - rho), U_i1_i, u_t1);
					U[uii] = rho;

					// update matrix //
					for (Size j(i), uij(next_c(uii, u_t1)), tj{ next_r(ti, tau_t) }; ++j < n1; uij = next_c(uij, u_t1), tj = next_r(tj, tau_t))
					{
						auto U_i1_j = U + next_r(uij, u_t1);

						double k = tau[ti] * (s_vv(m1 - i - 1, U_i1_i, u_t1, U_i1_j, u_t1) + U[uij]);
						U[uij] += k;
						s_va(m1 - i - 1, k, U_i1_i, u_t1, U_i1_j, u_t1);
					}
				}
			}
			
			///////////////////////////////////////////第二次变换////////////////////////////////////////////////////
			{
				if (i + 2 < m)
				{
					auto ui1i = next_r(uii, u_t);

					// compute householder vector //
					auto U_i2_i = U + next_r(ui1i, u_t);
					double rho = -std::sqrt(s_vv(m - i - 1, U + ui1i, u_t, U + ui1i, u_t)) * s_sgn2(U[ui1i]);
					tau2[t2i] = U[ui1i] / rho - 1.0;
					s_nm(m - 2 - i, 1, 1.0 / (U[ui1i] - rho), U_i2_i, u_t);
					U[ui1i] = rho;

					// update matrix //
					for (Size j(i), ui1j(next_c(ui1i, u_t)), t2j{ next_r(t2i, tau2_t) }; ++j < n; ui1j = next_c(ui1j, u_t), t2j = next_r(t2j, tau2_t))
					{
						auto U_i2_j = U + next_r(ui1j, u_t);

						double k = tau2[t2i] * (s_vv(m - i - 2, U_i2_i, u_t, U_i2_j, u_t) + U[ui1j]);
						U[ui1j] += k;
						s_va(m - i - 2, k, U_i2_i, u_t, U_i2_j, u_t);
					}
				}
			}
		}
*/
		// solve singular values //
		//std::function<void()> dvc;

		//auto dvc = [](Size m, Size n, double f)
		//{





		//};




	}
	//auto inline s_svd(Size m, Size n, Size rank, const double *U, const double *tau, const Size *p, double *x, double *tau2, double zero_check = 1e-10)noexcept->void { s_svd(m, n, rank, U, n, tau, 1, p, x, m, tau2, 1, zero_check); }

	// find plane using point clouds //
	//    n : point number
	//    x : x
	//    y : y
	//    z : z
	//    p : 4 维向量， p1 x + p2 y + p3 z + p4 = 0，其中 p1 p2 p3 为单位向量
	// tau2 : max(n,m) x 1
	auto inline s_interp_plane(Size n, const double *x, const double *y, const double *z, double *plane_func)->void {
		double avg_x{ 0.0 }, avg_y{ 0.0 }, avg_z{ 0.0 };

		for (Size i = 0; i < n; ++i) {
			avg_x += x[i];
			avg_y += y[i];
			avg_z += z[i];
		}
		avg_x /= n;
		avg_y /= n;
		avg_z /= n;

		double k1{ 0.0 }, k2{ 0.0 }, k3{ 0.0 }, k4{ 0.0 }, k5{ 0.0 }, k6{ 0.0 };
		for (Size i = 0; i < n; ++i) {
			auto dx = (x[i] - avg_x);
			auto dy = (y[i] - avg_y);
			auto dz = (z[i] - avg_z);


			k1 += dx * dx;
			k2 += dy * dy;
			k3 += dz * dz;
			k4 += dx * dy;
			k5 += dx * dz;
			k6 += dy * dz;
		}

		double A[9]{ k1,k4,k5,
					 k4,k2,k6,
					 k5,k6,k3 };

		double U[9], tau[3], Q[9], R[9];
		Size p[3], rank;

		s_householder_utp(3, 3, A, U, tau, p, rank, 1e-10);
		s_householder_ut2qr(3, 3, U, tau, Q, R);
		
		
		plane_func[0] = Q[2];
		plane_func[1] = Q[5];
		plane_func[2] = Q[8];
		plane_func[3] = -(plane_func[0] * avg_x + plane_func[1] * avg_y + plane_func[2] * avg_z);
	}
	auto inline s_interp_plane_error(Size n, const double *x, const double *y, const double *z, const double *plane_func)->double {
		double error{ 0.0 };
		for (Size i = 0; i < n; ++i) {
			auto e = plane_func[0] * x[i] + plane_func[1] * y[i] + plane_func[2] * z[i] + plane_func[3];
			error += e * e;
		}
		error /= n;

		return std::sqrt(error);
	}

	// check if point in Parallelepiped //
	//    p0 : origin
	//    p1 : first line
	//    p2 : second line
	//    p3 : third line
	//    p  : point need to check
	//    
	//       p3 ---- p6
	//      /       /|
	//     /       / |
	//    p0 ---- p2 p5
	//    |       |  /
	//    |       | /
	//    p1 ---- p4
	//     
	//    
	auto inline s_is_in_parallelepiped(const double *p0, const double *p1, const double *p2, const double *p3, const double *p, double zero_check = 1e-10)->bool {
		// p0-p1-p4-p2  :  plane A
		// p3-p7-p5-p6  :  plane B
		// p0-p2-p6-p3  :  plane C
		// p1-p4-p5-p7  :  plane D
		// p0-p3-p7-p1  :  plane E
		// p2-p6-p5-p4  :  plane F

		// v1           :  p1 - p0
		// v2           :  p2 - p0
		// v3           :  p3 - p0

		double v1[3]{ p1[0] - p0[0],p1[1] - p0[1], p1[2] - p0[2] },
			v2[3]{ p2[0] - p0[0],p2[1] - p0[1], p2[2] - p0[2] },
			v3[3]{ p3[0] - p0[0],p3[1] - p0[1], p3[2] - p0[2] };

		double A[4], B[4], C[4], D[4], E[4], F[4];

		auto s_c3 = [](const double *a, const double *b, double *c_out) {
			c_out[0] = -a[2] * b[1] + a[1] * b[2];
			c_out[1] = a[2] * b[0] - a[0] * b[2];
			c_out[2] = -a[1] * b[0] + a[0] * b[1];
		};

		// A & B //
		s_c3(v1, v2, A);
		if (s_norm(3, A) < zero_check)return false;
		s_nv(3, 1.0 / s_norm(3, A), A);
		A[3] = -s_vv(3, p0, A);
		s_vc(3, A, B);
		B[3] = -s_vv(3, p3, B);

		// C & D //
		s_c3(v2, v3, C);
		if (s_norm(3, C) < zero_check)return false;
		s_nv(3, 1.0 / s_norm(3, C), C);
		C[3] = -s_vv(3, p0, C);
		s_vc(3, C, D);
		D[3] = -s_vv(3, p1, D);

		// E & F //
		s_c3(v3, v1, E);
		if (s_norm(3, E) < zero_check)return false;
		s_nv(3, 1.0 / s_norm(3, E), E);
		E[3] = -s_vv(3, p0, E);
		s_vc(3, E, F);
		F[3] = -s_vv(3, p2, F);

		if ((s_vv(3, A, p) + A[3]) * (s_vv(3, B, p) + B[3]) > 0.0) return false;
		if ((s_vv(3, C, p) + C[3]) * (s_vv(3, D, p) + D[3]) > 0.0) return false;
		if ((s_vv(3, E, p) + E[3]) * (s_vv(3, F, p) + F[3]) > 0.0) return false;

		return true;
	}

	// check if point in Cylinder //
	//    p0  : cylinder origin, center of circle
	//    dir : direction
	//    r   : radius
	//    l   : length
	//    p  : point need to check
	auto inline s_is_in_cylinder(const double *p0, const double *dir, double r, double l, const double *p, double zero_check = 1e-10)->bool {
		double v[3]{ p[0] - p0[0],p[1] - p0[1], p[2] - p0[2] };
		double dir_norm[3]{ dir[0],dir[1],dir[2] };
		if (s_norm(3, dir_norm) < zero_check)return false;
		s_nv(3, 1.0 / s_norm(3, dir_norm), dir_norm);



		auto dis = s_vv(3, v, dir_norm);
		if (dis > std::max(0.0, l) || dis < std::min(0.0, l)) return false;

		auto rad = std::sqrt(std::max(0.0, s_vv(3, v, v) - dis * dis));
		if (rad > r)return false;


		return true;
	}
}

#endif
