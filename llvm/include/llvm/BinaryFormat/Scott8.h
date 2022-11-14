#ifndef SCOTT8_H
#define SCOTT8_H


namespace llvm {
  namespace scott8 {

    // Scott8 CPU loads at 0.
    const unsigned int ScottLoaderOffset = 0;

    unsigned long calculateAddress(unsigned long offset) {
      return offset + ScottLoaderOffset;
    }
  }
}

#endif // SCOTT8_H
