
rm -rf *.out

# Run seq once
./seq -i 2 2>&1 | tee -a seq.out

# Define the array
threads=(1 2 4 8 16 32 40)

iters=1
# Loop through each element in the array
for thread in "${threads[@]}"; do
    echo Running benchmark with \# threads : $thread

    if [ threads -lt 4 ]; then
        OMP_NUM_THREADS=$thread ./omp -i 1 2>&1 | tee -a omp.out
        OMP_NUM_THREADS=$thread ./omp_simd -i 1 2>&1 | tee -a omp_simd.out
    fi
    OMP_NUM_THREADS=$thread ./omp -i $iters  2>&1 | tee -a omp.out
    OMP_NUM_THREADS=$thread ./omp_simd -i $iters 2>&1 | tee -a omp_simd.out
    ./hpx --hpx:threads=$thread -i $iters 2>&1 | tee -a hpx.out
    ./hpx_static_cs --hpx:threads=$thread -i $iters 2>&1 | tee -a hpx_static_cs.out
    ./hpx_simd --hpx:threads=$thread -i $iters 2>&1 | tee -a hpx_simd.out
done
