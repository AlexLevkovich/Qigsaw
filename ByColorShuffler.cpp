#include "ByColorShuffler.h"
#include "IGap.h"

const int ByColorShuffler::colorGaps = 10000;

ByColorShuffler::ByColorShuffler(const QList<Piece *> & pieces,int panel_width,int panel_height):JigShuffler(pieces) {
    this->panel_width = panel_width;
    this->panel_height = panel_height;
}

int ByColorShuffler::iqpiece_compare(const void * item1, const void * item2) {
    IqPiece * piece1 = (IqPiece *)item1;
    IqPiece * piece2 = (IqPiece *)item2;
    return piece1->getIQ() - piece2->getIQ();
}

void ByColorShuffler::shuffle() {
    int gapLen = 0xFFFFFF/colorGaps;
    IGap * gaps = new IGap[colorGaps];
    for (int i=0;i<(colorGaps-1);i++) {
        int start = (i==0)?0:(gaps[i-1].getEnd()+1);
        gaps[i] = IGap(start,start+gapLen-1);
    }
    gaps[colorGaps-1] = IGap(gaps[colorGaps-2].getEnd()+1,0xFFFFFF);

    int count = 0;
    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);
        if (piece->instanceOf() != MPiece) {
            count++;
        }
    }
    IqPiece* piecesIQ = new IqPiece[count];

    int maxWidth = 0;
    int ii = 0;
    for (int i=0;i<zOrder.count();i++) {
        Piece * piece = zOrder.at(i);
        if (piece->instanceOf() == MPiece) continue;

        if (piece->getCurrentWidth() > maxWidth) {
            maxWidth = piece->getCurrentWidth();
        }
        piecesIQ[ii++] = IqPiece(piece,gaps,colorGaps);
    }

    qsort(piecesIQ,count,sizeof(IqPiece),iqpiece_compare);


    int colCount = panel_width/(maxWidth+2);
    int i=0;

    int rowCount = count/colCount;
    int reminder = count - (colCount*rowCount);
    for (int col=0;col<colCount;col++) {
        rowCount = count/colCount;
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
           Piece * piece = piecesIQ[j++].getPiece();

           realHeight += piece->getCurrentHeight()+2;
        }

        int hDelta = (rowCount <= 1)?0:(panel_height - realHeight)/(rowCount - 1);

        for (int row=0;row<rowCount;row++) {
            if (i >= count) break;
            Piece * piece = piecesIQ[i++].getPiece();

            int currY = (row > 0)?yByPrev+hDelta:0;
            piece->setPuzzlePosition(xPos,currY);
            yByPrev = currY + piece->getCurrentHeight() + 2;
        }
    }

    delete[] gaps;
    delete[] piecesIQ;
}
