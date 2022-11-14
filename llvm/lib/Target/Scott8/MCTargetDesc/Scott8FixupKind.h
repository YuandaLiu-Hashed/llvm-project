

#ifndef LLVM_LIB_TARGET_Scott8_FIXUPKINDS_H
#define LLVM_LIB_TARGET_Scott8_FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
    namespace Scott8 {
        enum Fixups {
            Scott8FixupNone = FirstTargetFixupKind,
            Scott8OrdinaryFixup,
            LastTargetFixupKind,
            NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
        };
    }; /* namespace Scott8 */
}; /* namespace llvm */

#endif /* LLVM_LIB_TARGET_Scott8_FIXUPKINDS_H */