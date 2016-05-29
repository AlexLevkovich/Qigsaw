#include "FreeCenterShuffler.h"

typedef Piece * p_Piece;

FreeCenterShuffler::FreeCenterShuffler(const QList<Piece *> & pieces,int panel_width,int panel_height,int image_width):JigShuffler(pieces) {
    this->panel_width = panel_width;
    this->panel_height = panel_height;
    this->image_width = image_width;
}

void FreeCenterShuffler::shuffle() {
    int count = 0;
    int maxWidth = 0;
    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);
        if (piece->instanceOf() !=  MPiece) {
            if (piece->getCurrentWidth() > maxWidth) {
                maxWidth = piece->getCurrentWidth();
            }
            count++;
        }
    }

    p_Piece * pieces = new p_Piece[count];

    int pi = 0;
    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);
        if (piece->instanceOf() != MPiece) {
            if (piece->getFrontier() != -1) {
                pieces[pi++] = piece;
            }
        }
    }
    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);
        if (piece->instanceOf() != MPiece) {
            if (piece->getFrontier() == -1) {
                pieces[pi++] = piece;
            }
        }
    }

    int div1 = count/2;
    int div2 = count - div1;

    int rectWidth = (panel_width-image_width)/2;

    int colCount = rectWidth/(maxWidth+2);
    int i=0;
    int rowCount = div1/colCount;
    int reminder = div1 - (colCount*rowCount);
    for (int col=0;col<colCount;col++) {
        rowCount = div1/colCount;
        if (reminder > 0) {
            rowCount++;
            reminder--;
        }

        int xPos = col*(maxWidth+2);
        int yByPrev = 0;

        int j=i;
        int realHeight = 0;
        for (int row=0;row<rowCount;row++) {
           if (j >= count) break;
           Piece * piece = pieces[j++];

           realHeight += piece->getCurrentHeight()+2;
        }

        int hDelta = (rowCount <= 1)?0:(panel_height - realHeight)/(rowCount - 1);

        for (int row=0;row<rowCount;row++) {
            if (i >= count) break;
            Piece * piece = pieces[i++];

            int currY = (row > 0)?yByPrev+hDelta:0;
            piece->setPuzzlePosition(xPos,currY);
            yByPrev = currY + piece->getCurrentHeight() + 2;
        }
    }

    rowCount = div2/colCount;
    reminder = div2 - (colCount*rowCount);
    for (int col=0;col<colCount;col++) {
        rowCount = div2/colCount;
        if (reminder > 0) {
            rowCount++;
            reminder--;
        }

        int xPos = panel_width - maxWidth;
        if (col > 0) {
            xPos = panel_width - maxWidth - (col*(maxWidth+2));
        }
        int yByPrev = 0;

        int j=i;
        int realHeight = 0;
        for (int row=0;row<rowCount;row++) {
           if (j >= count) break;
           Piece * piece = pieces[j++];

           realHeight += piece->getCurrentHeight()+2;
        }

        int hDelta = (rowCount <= 1)?0:(panel_height - realHeight)/(rowCount - 1);
        for (int row=0;row<rowCount;row++) {
            if (i >= count) break;
            Piece * piece = pieces[i++];

            int currY = (row > 0)?yByPrev+hDelta:0;
            piece->setPuzzlePosition(xPos,currY);
            yByPrev = currY + piece->getCurrentHeight() + 2;
        }
    }

    delete[] pieces;
}

