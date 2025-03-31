//=============================================================================
// FILE:
//    FirstPass.cpp
//    JRK: This file is the custom PassManager templates supplied by the professor
//    on Moodle.
//
// DESCRIPTION:
//    Visits all functions in a module and prints their names. Strictly speaking, 
//    this is an analysis pass (i.e. //    the functions are not modified). However, 
//    in order to keep things simple there's no 'print' method here (every analysis 
//    pass should implement it).
//
// USAGE:
//    New PM
//      opt -load-pass-plugin=<path-to>libFirstPass.so -passes="<pass-defined-here>" `\`
//        -disable-output <input-llvm-file>
//
//
// License: MIT
//=============================================================================
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) {
    
    // Preleviamo le prime due istruzioni del BB
    Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());
    
    // L'indirizzo della prima istruzione deve essere uguale a quello del 
    // primo operando della seconda istruzione (per costruzione dell'esempio)
    assert(&Inst1st == Inst2nd.getOperand(0));
    
    // Stampa la prima istruzione
    outs() << "PRIMA ISTRUZIONE: " << Inst1st << "\n";
    // Stampa la prima istruzione come operando
    outs() << "COME OPERANDO: ";
    Inst1st.printAsOperand(outs(), false);
    outs() << "\n";
    
    // User-->Use-->Value
    outs() << "I MIEI OPERANDI SONO:\n";
    for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
        Value *Operand = *Iter;
        
        if (Argument *Arg = dyn_cast<Argument>(Operand)) {
            outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo() 
            <<" DELLA FUNZIONE " << Arg->getParent()->getName()
            << "\n";
        }
        if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
            outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue()
            << "\n";
        }
    }
    
    outs() << "LA LISTA DEI MIEI USERS:\n";
    for (auto Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) {
        outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
    }
    
    outs() << "E DEI MIEI USI (CHE E' LA STESSA):\n";
    for (auto Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) {
        outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
    }
    
    // Manipolazione delle istruzioni
    Instruction *NewInst = BinaryOperator::Create(
        Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));
        
        NewInst->insertAfter(&Inst1st);
        // Si possono aggiornare le singole references separatamente?
        // Controlla la documentazione e prova a rispondere.
        Inst1st.replaceAllUsesWith(NewInst);
        
        return true;
    }
    
    //TODO For each pass, create struct and implement get<PassName>PluginLibraryInfo
    //-----------------------------------------------------------------------------
    // FirstPass implementation
    //-----------------------------------------------------------------------------
    // No need to expose the internals of the pass to the outside world - keep
    // everything in an anonymous namespace.
    namespace {
        
        // New PM implementation
        struct AlgebraicIdentityPass: PassInfoMixin<AlgebraicIdentityPass> {
            // Main entry point, takes IR unit to run the pass on (&F) and the
            // corresponding pass manager (to be queried if need be).
            PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
                bool changed = false;

                // Analizzo i BB della funzione
                for (auto &BB : F) {
                    //Analizzo le Instruction dei BB
                    for (auto I = BB.begin(), E = BB.end(); I != E;) {                        
                        Instruction *current_I = &*I;
                        ++I;
                        //Controllo il tipo di operazione (add mult)
                        if (auto *BO = dyn_cast<BinaryOperator>(current_I)) {
                            Value *op1 = BO->getOperand(0);
                            Value *op2 = BO->getOperand(1);
                            
                            if(BO->getOpcode() == Instruction::Add) {
                                errs() << "op1: " << *op1 << "op2: " << *op2 << '\n';
                                errs() << "Op code: " << BO->getOpcode() << '\n';

                                if(auto *CI = dyn_cast<ConstantInt>(op2)) {
                                    if (CI->isZero()) {
                                        BO->replaceAllUsesWith(op1);
                                        BO->eraseFromParent();
                                        changed = true;
                                        continue;
                                    }
                                }

                            }
                        
                        }
                    }
                
                }

            // return true;
            return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
            }
        
        };
        // Without isRequired returning true, this pass will be skipped for functions
        // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
        // all functions with optnone.
        static bool isRequired() { return true; }
}; // namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getAlgebraicIdentityPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "AlgebraicIdentityPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "algebraic-identity") {
                            FPM.addPass(AlgebraicIdentityPass());
                            return true;
                        }
                        return false;
                    });
                }};
            }
            
            // This is the core interface for pass plugins. It guarantees that 'opt' will
            // be able to recognize AlgebraicIdentityPass when added to the pass pipeline on the
            // command line, i.e. via '-passes=local-opts'
            extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
            llvmGetPassPluginInfo() {
                return getAlgebraicIdentityPassPluginInfo();
            }
            