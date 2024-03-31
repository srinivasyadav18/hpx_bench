backends=("seq" "simd" "hpx" "hpx_simd" "hpx_static_cs" "omp" "omp_simd")

for b in "${backends[@]}"; do
    grep -E 'thread|Average|backend' $b".out" | tee $b"_clean.out"
done
