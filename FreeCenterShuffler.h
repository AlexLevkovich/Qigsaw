#ifndef _FREECENTERSHUFFLER_H_
#define _FREECENTERSHUFFLER_H_

#include "JigShuffler.h"

class FreeCenterShuffler: public JigShuffler {
public:
    FreeCenterShuffler(const QList<Piece *> & pieces,int panel_width,int panel_height,int image_width);
    void shuffle();
private:
    int panel_width;
    int panel_height;
    int image_width;
};

#endif // _FREECENTERSHUFFLER_H_
