#ifndef _BYCOLORSHUFFLER_H_
#define _BYCOLORSHUFFLER_H_

#include "JigShuffler.h"

class ByColorShuffler: public JigShuffler {
public:
    ByColorShuffler(const QList<Piece *> & pieces,int panel_width,int panel_height);
    void shuffle();
private:
    int panel_width;
    int panel_height;

    class IqPiece {
      public:
        IqPiece() {
            piece = NULL;
            iq = 0;
        }

        IqPiece(Piece * piece,IGap * gaps,int g_count) {
            this->piece = piece;
            iq = piece->getPieceIQ(gaps,g_count);
        }

        int getIQ() const {
            return iq;
        }

        Piece * getPiece() const {
            return piece;
        }

        IqPiece & operator= ( const IqPiece & gap ) {
            piece = gap.getPiece();
            iq = gap.getIQ();
            return *this;
        }
      private:
        Piece * piece;
        int iq;
    };

    static const int colorGaps;

    static int iqpiece_compare(const void * item1, const void * item2);
};

#endif // _BYCOLORSHUFFLER_H_
