#ifndef VM_INSN_HPP
#define VM_INSN_HPP
#include <inttypes.h>
#include "objectmodel.hpp"
#include "inlines.hpp"

namespace sanya {

namespace vm_insn {

enum OpCode {
    kHalt = 0,
    kNop,

    kLoadFixnum,
    kLoadNil,
    kLoadBool,
    kLoadConst,
    kLoadCell,
    kLoadGlobal,
    kBuildClosure,

    kMove,
    kStoreCell,
    kStoreGlobal,

    kBranch,
    kBranchIfFalse,
    kCall,
    kTailCall,
    kRet,

    kLast
};

typedef uint16_t Operand;
typedef int32_t Immediate;

inline RawFixnum *PackRType(OpCode op, Operand a,
                            Operand b = 0, Operand c = 0) {
    return (((uint16_t)op) << RawObject::kNonHeapTypeShift) |
        RawObject::kFixnumType | (a << 16) | (b << 32) | (c << 48);
}

inline RawFixnum *PackIType(OpCode op, Operand a, Immediate bx) {
    return (((uint16_t)op) << RawObject::kNonHeapTypeShift) |
        RawObject::kFixnumType | (a << 16) | (bx << 32);
}

inline OpCode UnpackOperator(RawFixnum *insn) {
    return (OpCode)((insn & 0xffff) >> RawObject::kNonHeapTypeShift);
}

inline Operand UnpackOperandA(Insn insn) {
    return (insn >> 16) & 0xffff;
}

inline Operand UnpackOperandB(Insn insn) {
    return (insn >> 32) & 0xffff;
}

inline Operand UnpackOperandC(Insn insn) {
    return (insn >> 48) & 0xffff;
}

inline Immediate UnpackImmediate(Insn insn) {
    return (insn >> 32) & 0xffffffffL;
}

}  // namespace vm_insn

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* VM_INSN_HPP */
