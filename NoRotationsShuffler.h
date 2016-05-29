#ifndef _NOROTATIONSSHUFFLER_H_
#define _NOROTATIONSSHUFFLER_H_

#include "JigShuffler.h"

class NoRotationsShuffler: public JigShuffler {
public:
    NoRotationsShuffler(const QList<Piece *> & pieces);
    void shuffle();
};

#endif // _NOROTATIONSSHUFFLER_H_
