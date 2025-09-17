#for dir in Debug RelWithDebInfo; do
for dir in Debug; do
    rm -rf build/main/$dir/Calculators/*
    mkdir -p build/main/$dir/Calculators
    cp build/Calculators/*/*/$dir/*.dll build/main/$dir/Calculators
    cp build/Calculators/*/*/$dir/*.pdb build/main/$dir/Calculators
done    
