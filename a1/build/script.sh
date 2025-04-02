# ESEGUIRE QUESTO SCRIPT DALLA CARTELLA build/

rel_path='../test/'

clang -Xclang -disable-O0-optnone -emit-llvm -S -c -O0 ${rel_path}StrengthReductionTester.cpp -o ${rel_path}StrengthReductionTester.ll

opt -p mem2reg -S ${rel_path}StrengthReductionTester.ll  -o ${rel_path}StrengthReductionTester.m2r.ll

# Utilizzare quest'ultima riga solo in caso di opt -p mem2reg... -o ${rel_path}StrengthReductionTester.m2r.bc senza il flag -S.
# Il flag -S serve a generare un file leggibile non in bitcode.
# L'ultima riga serve a generare un file human-readable a partire da bitcode. 

#llvm-dis ${rel_path}StrengthReductionTester.m2r.bc -o ../test/StrengthReductionTester.m2r.ll

make
opt -load-pass-plugin ./libFirstPass.so -passes=strength-reduction ${rel_path}StrengthReductionTester.m2r.ll -disable-output
