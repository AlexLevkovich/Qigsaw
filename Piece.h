#ifndef _PIECE_H_
#define _PIECE_H_

#include <QImage>
#include <QVector>
#include <QList>
#include <QString>
#include <QDataStream>

class QPainter;

enum PieceId {
    BPiece,
    MPiece
};

class MultiPiece;
class IGap;

class Piece {
    friend class MultiPiece;

public:
    /* A Piece must be within this many pixels of "perfect" to be considered
     * close. */
    static const int posClose;
    static const int pieceCenterClose;

    static const int LEFT_FRONT;
    static const int RIGHT_FRONT;
    static const int TOP_FRONT;
    static const int BOTTOM_FRONT;
    static const int LEFT_TOP_FRONT;
    static const int RIGHT_TOP_FRONT;
    static const int LEFT_BOTTOM_FRONT;
    static const int RIGHT_BOTTOM_FRONT;
    static QList<Piece *> all_puzzles;

    static void deleteAllPieces();
    static int countsPieces();
    Piece();
    ~Piece();

    Piece(const QVector<QRgb> & data,
          int imageX, int imageY,
          int imageWidth, int imageHeight,
          int totalWidth, int totalHeight,
          int rotation);

    void setFlashMode(bool flashMode);
    bool isFlashMode();
    bool isFrontierSet(int front);
    virtual int getFrontier();
    void setFrontier(int frontier);

    virtual void writeExternal(QDataStream * out);
    virtual void readExternal(QDataStream * in);

    virtual void setRotation(int rot);
    int getRotation();
    void forceSetRotation();
    int getCurrentHeight();
    int getCurrentWidth();
    int getTotalWidth();
    int getTotalHeight();
    int getImageHeight();
    int getImageWidth();
    int getImageX();
    int getImageY();
    int getRotatedX();
    int getRotatedY();
    int getRotatedX(int rotation);
    int getRotatedY(int rotation);
    int getPuzzleX();
    int getPuzzleY();
    void setPuzzlePosition(int x, int y);
    QRect getPuzzleRect() const;
    QImage getImage() const;
    void addNeighbor(Piece * neighbor);
    void removeNeighbor(Piece * neighbor);
    void moveTo(int x, int y);
    virtual QString toString() const;
    virtual bool contains(int x, int y);
    bool isCloseTo(Piece * piece);
    QList<Piece *> join();
    virtual PieceId instanceOf();
    int getPieceIQ(IGap * gaps,int count);
    bool isOneOfNeighbors(Piece * piece);
    QPoint getClosedToRectNewPosition(QRect rect) const;
    virtual void draw(QPainter * g);
protected:
    QList<Piece *> neighbors;
    int origWidth, origHeight;
    int curWidth, curHeight;

    void init(const QVector<QRgb> & data,
              int imageX, int imageY,
              int imageWidth, int imageHeight,
              int totalWidth, int totalHeight);
    void init();
    void setImagePosition(int x, int y);
    void forceSetRotation(int rot);
    virtual void setImageData(const QVector<QRgb> & data, int width, int height);
    virtual void recomputeImageData();
    void setRotatedPosition();
    int getAlpha(int x, int y);
    virtual void flush();
private:
    static void bevel(QVector<QRgb> & data, int width, int height);
    static int gaps_compare(const void * item1,const void * item2);
    int getGapNum(IGap * gaps,int count,int color);

    // Location in the image.
    int imageX, imageY;
    // Size of the entire image.
    int totalWidth, totalHeight;
    // Location in the image adjusted by current rotation.
    int rotatedX, rotatedY;
    QVector<QRgb> origData;
    QVector<QRgb> curData;
    int puzzleX, puzzleY;
    // Image for this Piece.  We're going to now try making this null for
    // non-atomic pieces.
    QImage image;
    QImage flashImage;
    // This is measured in integer degrees, 0-359.  0 is unrotated.  90 is 90
    // degrees clockwise, etc.
    int rotation;
    int frontier;
    bool flashMode;
};

QDataStream & operator >> (QDataStream & in,Piece * & t);

#endif // _PIECE_H_
