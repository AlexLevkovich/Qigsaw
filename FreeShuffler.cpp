#include "FreeShuffler.h"
#include <math.h>
#include "JigUtil.h"

FreeShuffler::FreeShuffler(const QList<Piece *> & pieces,int panel_width,int panel_height):JigShuffler(pieces) {
    this->panel_width = panel_width;
    this->panel_height = panel_height;
}

void FreeShuffler::shuffle() { 
    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);

		int x = (int) (_rand() * (panel_width  - piece->getCurrentWidth ()));
        int y = (int) (_rand() * (panel_height - piece->getCurrentHeight()));

        piece->moveTo(x,y);
    }
}
