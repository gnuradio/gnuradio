#include <cuComplex.h>

template <typename T>
__global__ void kernel_fft_shift(const T* in, T* out, int n);

template <>
__global__ void kernel_fft_shift(const cuFloatComplex* in, cuFloatComplex* out, int n)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < n) {
        float a = 1 - 2 * (i & 1);
        out[i].x = in[i].x * a;
        out[i].y = in[i].y * a;
    }
}

template <>
__global__ void kernel_fft_shift(const cuDoubleComplex* in, cuDoubleComplex* out, int n)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < n) {
        double a = 1 - 2 * (i & 1);
        out[i].x = in[i].x * a;
        out[i].y = in[i].y * a;
    }
}


void exec_fft_shift(const cuFloatComplex* in,
                    cuFloatComplex* out,
                    int n,
                    int grid_size,
                    int block_size,
                    cudaStream_t stream)
{
    kernel_fft_shift<<<grid_size, block_size, 0, stream>>>(in, out, n);
}
