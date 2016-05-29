#ifndef _JIGSHUFFLER_H_
#define _JIGSHUFFLER_H_

#include <QList>
#include "Piece.h"

class JigShuffler {
public:
    JigShuffler(const QList<Piece *> & pieces) {
        zOrder = pieces;
    }

    virtual void shuffle() = 0;
    QList<Piece *> & getOrder() { return zOrder; }

protected:
    QList<Piece *> zOrder;
};

#endif // _JIGSHUFFLER_H_
