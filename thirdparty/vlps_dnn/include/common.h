#ifndef BAIDU_FCR_MODEL_ABACUS_DNN_COMMON_H
#define BAIDU_FCR_MODEL_ABACUS_DNN_COMMON_H

#include <mkl.h>
#include <mkl_cblas.h>
#include <Eigen/Eigen>
#include "vlps/common/failure.h"
#include "vlps/common/macros.h"
namespace abacus_dnn {

template<class A, class B>
bool matrix_size_equals(const Eigen::DenseBase<A>& a, const Eigen::DenseBase<B>& b) {
    return a.rows() == b.rows() && a.cols() == b.cols();
}

template<class A>
bool matrix_size_equals(const Eigen::DenseBase<A>& a, Eigen::DenseIndex rows,
        Eigen::DenseIndex cols) {
    return a.rows() == rows && a.cols() == cols;
}

template<class A, class B, class C>
void matrix_multiply_with_cpu(bool transa, bool transb, bool transc, const A& a, const B& b, C& c) {
    if (transc) {
        return matrix_multiply_with_cpu(!transb, !transa, false, b, a, c);
    }
    if (!transa) {
        if (!transb) {
            abacus::FailureExit on_check_size([&a, &b]() {
                MIO_FAILURE_LOG << "check matrix size failed, "
                    << "a["<< a.rows() << " * " << a.cols() << "]"
                    << "b["<< b.rows() << " * " << b.cols() << "]";
            });
            MIO_CHECK(a.cols() == b.rows());
            c.noalias() = a * b;
        } else {
            abacus::FailureExit on_check_size([&a, &b]() {
                MIO_FAILURE_LOG << "check matrix size failed, "
                    << "a["<< a.rows() << " * " << a.cols() << "]"
                    << "b["<< b.rows() << " * " << b.cols() << "]";
            });
            MIO_CHECK(a.cols() == b.cols());
            c.noalias() = a * b.transpose();
        }
    } else {
        if (!transb) {
            abacus::FailureExit on_check_size([&a, &b]() {
                MIO_FAILURE_LOG << "check matrix size failed, "
                    << "a["<< a.rows() << " * " << a.cols() << "]"
                    << "b["<< b.rows() << " * " << b.cols() << "]";
            });
            MIO_CHECK(a.rows() == b.rows());
            c.noalias() = a.transpose() * b;
        } else {
            abacus::FailureExit on_check_size([&a, &b]() {
                MIO_FAILURE_LOG << "check matrix size failed, "
                    << "a["<< a.rows() << " * " << a.cols() << "]"
                    << "b["<< b.rows() << " * " << b.cols() << "]";
            });
            MIO_CHECK(a.rows() == b.cols());
            c.noalias() = a.transpose() * b.transpose();
        }
    }
}

template<class A, class B, class C>
void matrix_multiply(bool transa, bool transb, bool transc, const A& a, const B& b, C& c) {
    if (transc) {
        return matrix_multiply(!transb, !transa, false, b, a, c);
    }
    
    matrix_multiply_with_cpu(transa, transb, transc, a, b, c);
}

template<class A, class B, class C>
void matrix_multiply_add(bool transa, bool transb, bool transc, const A& a, const B& b, C& c) {
    if (transc) {
        return matrix_multiply_add(!transb, !transa, false, b, a, c);
    }

    if (!transa) {
        if (!transb) {
            CHECK(a.cols() == b.rows());
            CHECK(matrix_size_equals(c, a.rows(), b.cols())) << " a.rows[" << a.rows() << "]"
                                                             << " b.cols[" << b.cols() << "]";
            c.noalias() += a * b;
        } else {
            CHECK(a.cols() == b.cols());
            CHECK(matrix_size_equals(c, a.rows(), b.rows())) << " a.rows[" << a.rows() << "]"
                                                             << " b.rows[" << b.rows() << "]";
            c.noalias() += a * b.transpose();
        }
    } else {
        if (!transb) {
            CHECK(a.rows() == b.rows());
            CHECK(matrix_size_equals(c, a.cols(), b.cols())) << " a.cols[" << a.cols() << "]"
                                                             << " b.cols[" << b.cols() << "]";
            c.noalias() += a.transpose() * b;
        } else {
            CHECK(a.rows() == b.cols());
            CHECK(matrix_size_equals(c, a.cols(), b.rows())) << " a.cols[" << a.cols() << "]"
                                                             << " b.rows[" << b.rows() << "]";
            c.noalias() += a.transpose() * b.transpose();
        }
    }
}

}
#endif //BAIDU_FCR_MODEL_ABACUS_DNN_COMMON_H
