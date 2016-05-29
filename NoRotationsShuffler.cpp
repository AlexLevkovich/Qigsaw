#include "NoRotationsShuffler.h"

NoRotationsShuffler::NoRotationsShuffler(const QList<Piece *> & pieces):JigShuffler(pieces) {}

void NoRotationsShuffler::shuffle() {
   for (int i=0;i<zOrder.count();i++) {
       zOrder.at(i)->setRotation(0);
   }
}
