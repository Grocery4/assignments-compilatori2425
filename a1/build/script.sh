#!/bin/bash
# ===================================================================== #
# Eseguire lo script dalla directory 'build/' e specificare
# $1: il nome del file sorgente senza estensione
# $2: nome del passo
# esempio: bash script.sh MultiInstructionTester 'multi-instruction'
# ===================================================================== #

TEST_PATH='../test/'

# ===================================================================== #
# Rimuovi operazioni di load/store non necessarie.
# ===================================================================== #

clang -Xclang -disable-O0-optnone -emit-llvm -S -c -O0 ${TEST_PATH}${1}.cpp -o ${TEST_PATH}$1.ll

# Utilizzare l'ottimizzazione mem2reg trasforma la variabile 'a'
# in un valore costante, non permettendo la corretta dimostrazione
# del passo definito.
# opt -p mem2reg ${TEST_PATH}$1.ll -S -o ${TEST_PATH}$1.m2r.ll

# Utilizzare quest'ultima riga solo in caso di opt -p mem2reg... -o ${TEST_PATH}$1.m2r.bc senza il flag -S.
# Il flag -S serve a generare un file leggibile non in bitcode.
# L'ultima riga serve a generare un file human-readable a partire da bitcode. 
# llvm-dis ${TEST_PATH}$1.m2r.bc -o ${TEST_PATH}$1.m2r.ll

# Genera la libreria a partire dal passo definito in 'FirstPass.cpp'
make

# CON mem2reg
# opt -load-pass-plugin ./libFirstPass.so -passes=${2} -S ${TEST_PATH}${1}.m2r.ll -o ${TEST_PATH}output.ll
# diff -u ${TEST_PATH}${1}.m2r.ll ${TEST_PATH}output.ll

# SENZA mem2reg
opt -load-pass-plugin ./libFirstPass.so -passes=${2} -S ${TEST_PATH}${1}.ll -o ${TEST_PATH}output.ll
diff -u ${TEST_PATH}${1}.ll ${TEST_PATH}output.ll
