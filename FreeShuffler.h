#ifndef _FREESHUFFLER_H_
#define _FREESHUFFLER_H_

#include "JigShuffler.h"

class FreeShuffler: public JigShuffler {
public:
    FreeShuffler(const QList<Piece *> & pieces,int panel_width,int panel_height);
    void shuffle();
private:
    int panel_width;
    int panel_height;
};


#endif // _FREESHUFFLER_H_
