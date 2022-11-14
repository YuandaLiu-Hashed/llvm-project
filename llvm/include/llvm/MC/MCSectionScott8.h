#ifndef MCSECTIONSCOTT8_H
#define MCSECTIONSCOTT8_H

#include "llvm/MC/MCSection.h"

namespace llvm {

class MCSymbol;
class StringRef;
class raw_ostream;

class MCSectionScott8 final : public MCSection {
  SmallString<8> SectionName;

private:
  friend class MCContext;
  MCSectionScott8(StringRef Name, SectionKind K, MCSymbol *Begin);

public:
  ~MCSectionScott8();

  StringRef getSectionName() const { return SectionName; }

  void printSwitchToSection(const MCAsmInfo &MAI, const Triple &T,
                            raw_ostream &OS,
                            const MCExpr *Subsection) const override;
  
  bool useCodeAlign() const override { return false; }
  bool isVirtualSection() const override { return false; }

  static bool classof(const MCSection *S) {
    return S->getVariant() == SV_Scott8;
  }
};

} // end namespace llvm

#endif // MCSECTIONSCOTT8_H
