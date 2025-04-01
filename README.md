**Generazione di codice senza load/store**

```
clang -Xclang -disable-O0-optnone -S -emit-llvm -O0 <src>.cc -o <dest>.ll
opt -p mem2reg <dest>.ll -o <dest-wo-loadstore>.m2r.bc
llvm-dis <dest-wo-loadstore>.m2r.bc -o <dest-wo-loadstore>.m2r.ll
```
**Il file generato sarà poi soggetto a ottimizzazione**

Una volta finito di modificare il `PassManager.cpp` eseguire il comando `make`
e se va a buon fine eseguire
`opt -load-pass-plugin <path/to/build/dir>lib<nome>.so -passes=<pass> <src>.ll -o dest.ll`

**Se la libreria non è ancora stata creata**
```
export LLVM_DIR=<installation/dir/llvm19>
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR <source/dir/testPass.cpp>
make
```
*In fase di testing spesso spammo il comando `make && !opt`*
