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
                        
                        //Identità algebrica per somme
                        if(BO->getOpcode() == Instruction::Add) {
                            errs() << "op1: " << *op1 << " op2: " << *op2 << '\n';
                            errs() << "Op code: " << BO->getOpcode() << '\n';

                            //Senza l'else/if, il codice entra solo se è il primo operando a essere la variabile :(
                            if(auto *CI = dyn_cast<ConstantInt>(op2)) {
                                if (CI->isZero()) {
                                    BO->replaceAllUsesWith(op1);
                                    BO->eraseFromParent();
                                    changed = true;
                                    continue;
                                }
                            }

                            else if(auto *CI = dyn_cast<ConstantInt>(op1)) {
                                if (CI->isZero()) {
                                    BO->replaceAllUsesWith(op2);
                                    BO->eraseFromParent();
                                    changed = true;
                                    continue;
                                }
                            }


                        }


                        //Identità algebrica per moltiplicazioni
                        if(BO->getOpcode() == Instruction::Mul) {
                            errs() << "op1: " << *op1 << " op2: " << *op2 << '\n';
                            errs() << "Op code: " << BO->getOpcode() << '\n';

                            //Senza l'else/if, il codice entra solo se è il primo operando a essere la variabile :(
                            if(auto *CI = dyn_cast<ConstantInt>(op2)) {
                                if (CI->isOne()) {
                                    BO->replaceAllUsesWith(op1);
                                    BO->eraseFromParent();
                                    changed = true;
                                    continue;
                                }
                            }

                            else if(auto *CI = dyn_cast<ConstantInt>(op1)) {
                                if (CI->isOne()) {
                                    BO->replaceAllUsesWith(op2);
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
    
    struct StrengthReductionPass : PassInfoMixin<StrengthReductionPass> {
    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be).
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        bool changed = false;

        for (auto &B : F) { // Iterate over basic blocks
            for (auto &Inst : B) { // Iterate over instructions
                ConstantInt *C;
                Value *Param;

                // Check if the instruction has a constant operand
                if (getConstantFromInstruction(Inst, C, Param)) {
                    unsigned int instructionOpcode = Inst.getOpcode();

                    switch (instructionOpcode) {
                        case Instruction::Mul: {
                            if (C->getValue().isPowerOf2()) {
                                Constant *shiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                                Instruction *shift_left = BinaryOperator::Create(BinaryOperator::Shl, Param, shiftCount);
                                shift_left->insertAfter(&Inst);
                                Inst.replaceAllUsesWith(shift_left);
                                changed = true;
                            } else if ((C->getValue() - 1).isPowerOf2()) {
                                Constant *shiftCount = ConstantInt::get(C->getType(), (C->getValue() - 1).exactLogBase2());
                                Instruction *shift_left = BinaryOperator::Create(BinaryOperator::Shl, Param, shiftCount);
                                shift_left->insertAfter(&Inst);

                                Instruction *new_add = BinaryOperator::Create(BinaryOperator::Add, shift_left, Param);
                                new_add->insertAfter(shift_left);
                                Inst.replaceAllUsesWith(new_add);
                                changed = true;
                            } else if ((C->getValue() + 1).isPowerOf2()) {
                                Constant *shiftCount = ConstantInt::get(C->getType(), (C->getValue() + 1).exactLogBase2());
                                Instruction *shift_left = BinaryOperator::Create(BinaryOperator::Shl, Param, shiftCount);
                                shift_left->insertAfter(&Inst);

                                Instruction *new_sub = BinaryOperator::Create(BinaryOperator::Sub, shift_left, Param);
                                new_sub->insertAfter(shift_left);
                                Inst.replaceAllUsesWith(new_sub);
                                changed = true;
                            }
                            break;
                        }
                        case Instruction::SDiv: {
                            if (C->getValue().isPowerOf2()) {
                                Constant *shiftCount = ConstantInt::get(C->getType(), C->getValue().exactLogBase2());
                                Instruction *shift_right = BinaryOperator::Create(BinaryOperator::LShr, Param, shiftCount);
                                shift_right->insertAfter(&Inst);
                                Inst.replaceAllUsesWith(shift_right);
                                changed = true;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }

        return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }
};
    
    // New PM implementation
    struct MultiInstructionPass: PassInfoMixin<MultiInstructionPass> {
        // Main entry point, takes IR unit to run the pass on (&F) and the
        // corresponding pass manager (to be queried if need be).
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
            errs() << "TBI MULTI-INSTRUCTION\n";
            
            // return true;
            return PreservedAnalyses::all();
    
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
llvm::PassPluginLibraryInfo getFirstPassPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "FirstPass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                    ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "algebraic-identity") {
                            FPM.addPass(AlgebraicIdentityPass());
                            return true;
                        }

                        if (Name == "strength-reduction") {
                            FPM.addPass(StrengthReductionPass());
                            return true;
                        }
                        
                        if (Name == "multi-instruction") {
                            FPM.addPass(MultiInstructionPass());
                            return true;
                        }

                        return false;
                    });
                }};
            }


// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize AlgebraicIdentityPass when added to the pass pipeline on the
// command line, i.e. via '-passes=algebraic-identity'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
return getFirstPassPluginInfo();
}
