# ESEGUIRE QUESTO SCRIPT DALLA CARTELLA build/

rel_path='../test/'

clang -Xclang -disable-O0-optnone -emit-llvm -S -c -O0 ${rel_path}$1.cpp -o ${rel_path}$1.ll

opt -p mem2reg -S ${rel_path}$1.ll  -o ${rel_path}$1.m2r.ll

# Utilizzare quest'ultima riga solo in caso di opt -p mem2reg... -o ${rel_path}$1.m2r.bc senza il flag -S.
# Il flag -S serve a generare un file leggibile non in bitcode.
# L'ultima riga serve a generare un file human-readable a partire da bitcode. 

#llvm-dis ${rel_path}$1.m2r.bc -o ${rel_path}$1.m2r.ll

make
opt -load-pass-plugin ./libFirstPass.so -passes=multi-instruction ${rel_path}$1.m2r.ll -disable-output
