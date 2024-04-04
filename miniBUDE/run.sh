
rm -rf *.out

./seq -i 1 2>&1 | tee -a seq.out
./simd -i 1 2>&1 | tee -a simd.out

# threads=(8 16 32 48 64 128)
# threads=(8 16 32 40)
threads=(1 2 4 8 16 32 64 128)

iters=5

for thread in "${threads[@]}"; do
    OMP_NUM_THREADS=$thread ./omp -i $iters  2>&1 | tee -a omp.out
    OMP_NUM_THREADS=$thread ./omp_simd -i $iters 2>&1 | tee -a omp_simd.out

    ./hpx --hpx:threads=$thread -i $iters 2>&1 | tee -a hpx.out
    ./hpx_simd --hpx:threads=$thread -i $iters 2>&1 | tee -a hpx_simd.out
    ./hpx_fj --hpx:threads=$thread -i $iters 2>&1 | tee -a hpx_fj.out
done

backends=("seq" "simd" "hpx" "hpx_simd" "hpx_fj" "omp" "omp_simd")

for b in "${backends[@]}"; do
    grep -E 'thread|Average|backend' $b".out" | tee $b"_clean.out"
done
