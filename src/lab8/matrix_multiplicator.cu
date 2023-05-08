#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>

#include <cuda.h>
#include <cuda_runtime.h>

#include "matrix.hpp"

using namespace std;

Matrix<double> a, b, result;

/* CUDA error checking macro */
#define gpuErrAssert(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char* file, int line, bool abort = true)
{
    if (code != cudaSuccess) {
        fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
        if (abort) exit(code);
    }
}

__global__ void multiply_matrices(
    const double* a,
    const double* b,
    double* result,
    const int result_width, const int height_b,
    const int width_a, const int width_b
) {
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    int k;
    double tmp = 0;
    for (k = 0; k < height_b; ++k) {
        tmp += a[i * width_a + k] * b[k * width_b + j];
    }
    result[i * result_width + j] = tmp;
}

void load_matrices(const string &path_a, const string &path_b)
{
    ifstream file_a(path_a), file_b(path_b);
    if (!file_a.is_open() || !file_b.is_open()) {
        cerr << "Error: Failed to open matrix files.\n";
        exit(1);
    }
    file_a >> a;
    file_b >> b;
    result.set_size(b.get_width(), a.get_height());

    cout << "Loaded matrices:\n";
    cout << " A (" << quoted(path_a) << "):\n" << a;
    cout << " B (" << quoted(path_b) << "):\n" << b;
    cout << "Allocated multiplication result matrix A * B = C ["
        << result.get_height() << "x" << result.get_width() << "]\n\n";
}

void check_cuda_devices()
{
    int nDevices;
    cudaGetDeviceCount(&nDevices);

    if (nDevices == 0) {
        cerr << "Error: No CUDA-compatible devices found!\n";
        exit(1);
    }

    cout << "Available CUDA devices:\n";
    for (int i = 0; i < nDevices; i++) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);
        printf("(%i)  Device name: %s\n", i, prop.name);
        printf("     Type: %s\n", prop.integrated ? "integrated" : "discrete");
        printf("     Total memory: %d MB\n", prop.totalGlobalMem / 0x100000);
        printf("     Memory Bus Width: %d bits\n", prop.memoryBusWidth);
        printf("     Peak Memory Bandwidth: %f GB/s\n",
            2.0 * prop.memoryClockRate * (prop.memoryBusWidth / 8) / 1.0e6);
    }
    cout << '\n';
}

int main(int argc, char **argv)
{
    check_cuda_devices();
    /* Load two matrices from files */
    if (argc >= 3)
        load_matrices({ argv[1] }, { argv[2] });
    else
        load_matrices("matrix_a.txt", "matrix_b.txt");

    auto size_a = sizeof(double) * a.size();
    auto size_b = sizeof(double) * b.size();
    auto result_size = sizeof(double) * result.size();

    /* Allocate device buffers */
    double* d_a, *d_b, *d_result;
    cudaMalloc(&d_a, size_a);
    cudaMalloc(&d_b, size_b);
    cudaMalloc(&d_result, result_size);

    /* Transfer data from host to device memory */
    cudaMemcpy(d_a, a.data(), size_a, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b.data(), size_b, cudaMemcpyHostToDevice);

    /* Execute kernel */
    cout << "Starting CUDA kernel execution...\n";
    dim3 threads_per_block(result.get_width(), result.get_height());
    multiply_matrices<<<1, threads_per_block>>>(
        d_a, d_b, d_result,
        result.get_width(), b.get_height(),
        a.get_width(), b.get_width()
    );
    gpuErrAssert(cudaPeekAtLastError());
    gpuErrAssert(cudaDeviceSynchronize());
    cout << "Done executing!\n";

    // Transfer data back to host memory
    cudaMemcpy(result.data(), d_result, result_size, cudaMemcpyDeviceToHost);
    cout << "Multiplication result: \n" << result;

    // Deallocate device memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_result);
}