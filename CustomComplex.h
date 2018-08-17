#ifndef __CustomComplex
#define __CustomComplex

#include <iostream>
#include <cstdlib>
#include <memory>
#include <iomanip>
#include <cmath>
#include <omp.h>
#include <ctime>
#include <stdio.h>
#include <sys/time.h>
#include <chrono>
#include <vector_types.h>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <chrono>

using namespace std;

#define nstart 0
#define nend 3

#define CUDAVER 1

#define CudaSafeCall( err ) __cudaSafeCall( err, __FILE__, __LINE__ )
#define CudaCheckError()    __cudaCheckError( __FILE__, __LINE__ )

inline void __cudaSafeCall( cudaError err, const char *file, const int line )
{
#ifdef CUDA_ERROR_CHECK
    if ( cudaSuccess != err )
    {
        fprintf( stderr, "cudaSafeCall() failed at %s:%i : %s\n",
        file, line, cudaGetErrorString( err ) );
        exit( -1 );
    }
#endif

    return;
}

inline void __cudaCheckError( const char *file, const int line )
{
#ifdef CUDA_ERROR_CHECK
    cudaError err = cudaGetLastError();
    if ( cudaSuccess != err )
    {
        fprintf( stderr, "cudaCheckError() failed at %s:%i : %s\n",
        file, line, cudaGetErrorString( err ) );
        exit( -1 );
    }

    // More careful checking. However, this will affect performance.
    // Comment away if needed. - Rahul - commented the below deviceSynchronize
//    err = cudaDeviceSynchronize();
    if( cudaSuccess != err )
    {
        fprintf( stderr, "cudaCheckError() with sync failed at %s:%i : %s\n",file, line, cudaGetErrorString( err ) );
        exit( -1 );
    }
#endif
    return;
}


//template<class re, class im>

class CustomComplex : public double2{
//#pragma omp declare target

    private : 
//    re x;
//    im y;

    public:
    explicit CustomComplex () {
        x = 0.00;
        y = 0.00;
    }


    explicit CustomComplex(const double& a, const double& b) {
        x = a;
        y = b;
    }

    CustomComplex(const CustomComplex& src) {
        x = src.x;
        y = src.y;
    }

    CustomComplex& operator =(const CustomComplex& src) {
        x = src.x;
        y = src.y;

        return *this;
    }

    CustomComplex& operator +=(const CustomComplex& src) {
        x = src.x + this->x;
        y = src.y + this->y;

        return *this;
    }

    CustomComplex& operator -=(const CustomComplex& src) {
        x = src.x - this->x;
        y = src.y - this->y;

        return *this;
    }

    CustomComplex& operator -() {
        x = -this->x;
        y = -this->y;

        return *this;
    }

    CustomComplex& operator ~() {
        return *this;
    }

    void print() const {
        printf("( %f, %f) ", this->x, this->y);
        printf("\n");
    }

    double get_real() const
    {
        return this->x;
    }

    double get_imag() const
    {
        return this->y;
    }

    void set_real(double val)
    {
        this->x = val;
    }

    void set_imag(double val) 
    {
        this->y = val;
    }

// 6 flops
//    template<class real, class imag>
    friend inline CustomComplex operator *(const CustomComplex &a, const CustomComplex &b) {
        double x_this = a.x * b.x - a.y*b.y ;
        double y_this = a.x * b.y + a.y*b.x ;
        CustomComplex result(x_this, y_this);
        return (result);
    }

//2 flops
//    template<class real, class imag>
    friend inline CustomComplex operator *(const CustomComplex &a, const double &b) {
       CustomComplex result(a.x*b, a.y*b);
       return result;
    }

//    template<class real, class imag>
    friend inline CustomComplex operator -(CustomComplex a, CustomComplex b) {
        CustomComplex result(a.x - b.x, a.y - b.y);
        return result;
    }

//2 flops
//    template<class real, class imag>
    friend inline CustomComplex operator -(const double &a, CustomComplex& src) {
        CustomComplex result(a - src.x, 0 - src.y);
        return result;
    }

    //template<class real, class imag>
    friend inline CustomComplex operator +(const double &a, CustomComplex& src) {
        CustomComplex result(a + src.x, src.y);
        return result;
    }

    //template<class real, class imag>
    friend inline CustomComplex operator +(CustomComplex a, CustomComplex b) {
        CustomComplex result(a.x + b.x, a.y+b.y);
        return result;
    }

    //template<class real, class imag>
    friend inline CustomComplex operator /(CustomComplex a, CustomComplex b) {

        CustomComplex b_conj = CustomComplex_conj(b);
        CustomComplex numerator = a * b_conj;
        CustomComplex denominator = b * b_conj;

        double re_this = numerator.x / denominator.x;
        double im_this = numerator.y / denominator.x;

        CustomComplex result(re_this, im_this);
        return result;
    }

    //template<class real, class imag>
    friend inline CustomComplex operator /(CustomComplex a, double b) {
       CustomComplex result(a.x/b, a.y/b);
       return result;
    }

    //template<class real, class imag>
    friend inline CustomComplex CustomComplex_conj(const CustomComplex& src) ;

    //template<class real, class imag>
    friend inline double CustomComplex_abs(const CustomComplex& src) ;

    //template<class real, class imag>
    friend inline double CustomComplex_real( const CustomComplex& src) ;

    //template<class real, class imag>
    friend inline double CustomComplex_imag( const CustomComplex& src) ;
//#pragma omp end declare target
};

//#pragma omp declare target
inline CustomComplex CustomComplex_conj(const CustomComplex& src) {

    double re_this = src.x;
    double im_this = -1 * src.y;

    CustomComplex result(re_this, im_this);
    return result;

}

inline double CustomComplex_abs(const CustomComplex& src) {
    double re_this = src.x * src.x;
    double im_this = src.y * src.y;

    double result = sqrt(re_this+im_this);
    return result;
}

inline double CustomComplex_real( const CustomComplex& src) {
    return src.x;
}

inline double CustomComplex_imag( const CustomComplex& src) {
    return src.y;
}

#endif

//Function definition
inline void schDttt_corKernel1(CustomComplex &schDttt_cor, int *inv_igp_index, int *indinv, CustomComplex *I_epsR_array, CustomComplex *I_epsA_array, CustomComplex *aqsmtemp, CustomComplex *aqsntemp, CustomComplex &schDttt, double *vcoul, int ncouls, int ifreq, int ngpown, int n1, double fact1, double fact2);

inline void schDttt_corKernel2(CustomComplex &schDttt_cor, int *inv_igp_index, int *indinv, CustomComplex *I_epsR_array, CustomComplex *I_epsA_array, CustomComplex *aqsmtemp, CustomComplex *aqsntemp, double *vcoul, int ncouls, int ifreq, int ngpown, int n1, double fact1, double fact2);
//#pragma omp end declare target
