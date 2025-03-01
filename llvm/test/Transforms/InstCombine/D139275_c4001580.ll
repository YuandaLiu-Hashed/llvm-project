; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -passes=instcombine -S < %s | FileCheck %s

; Based on reproduced from https://reviews.llvm.org/D139275#4001580
; FIXME: this is a miscompile.
define float @D139275_c4001580(float %arg) {
; CHECK-LABEL: @D139275_c4001580(
; CHECK-NEXT:    [[I:%.*]] = fcmp ugt float [[ARG:%.*]], 0.000000e+00
; CHECK-NEXT:    [[I1:%.*]] = fcmp ult float [[ARG]], 1.000000e+00
; CHECK-NEXT:    [[I2:%.*]] = and i1 [[I]], [[I1]]
; CHECK-NEXT:    [[I3:%.*]] = fcmp uge float [[ARG]], 0x3FB99999A0000000
; CHECK-NEXT:    [[I7:%.*]] = fadd float [[ARG]], 0xBFB99999A0000000
; CHECK-NEXT:    [[I5:%.*]] = select i1 [[I3]], float [[I7]], float 0x3FB99999A0000000
; CHECK-NEXT:    [[I8:%.*]] = select i1 [[I2]], float [[I5]], float 0.000000e+00
; CHECK-NEXT:    ret float [[I8]]
;
  %i = fcmp ugt float %arg, 0.000000e+00
  %i1 = fcmp ult float %arg, 1.000000e+00
  %i2 = and i1 %i, %i1
  %i3 = fcmp uge float %arg, 0x3FB99999A0000000
  %i4 = xor i1 %i, %i2
  %i5 = select i1 %i4, float 0x3FB99999A0000000, float 0.000000e+00
  %i6 = and i1 %i3, %i2
  %i7 = fadd float %arg, 0xBFB99999A0000000
  %i8 = select i1 %i6, float %i7, float %i5
  ret float %i8
}
