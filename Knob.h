#ifndef _KNOB_H_
#define _KNOB_H_

#include <QPainterPath>

#define DATA_LEN 5

typedef double float_arr[DATA_LEN][DATA_LEN];

class Knob {
public:
    Knob();
    Knob(int x1, int y1, int x2, int y2);
    ~Knob();

    QPainterPath getCurvePath(int x, int y) const;
    QPainterPath getCurvePath() const;
    int getX1();
    int getX2();
    int getY1();
    int getY2();
    QRect getBounds() const;
    QRect getBounds(int x,int y) const;
    QString toString() const;
    Knob & operator= ( const Knob & knob );
private:
    static const double ctl[DATA_LEN][DATA_LEN];
    static const int X;
    static const int Y;
    static const int SLOPE;
    static const int XDB;
    static const int XDF;

    // x,y each varies by + or - itsVar
    static const double XVARY;
    static const double YVARY;
    // b,f each varies by + or - it*itsVar
    static const double XDBVARY;
    static const double XDFVARY;

    int x1, y1, x2, y2;
    QPainterPath cPath, cPathReverse;

    void initPath();
    void curveTo(QPainterPath & path,const float_arr & data,int idx, bool forward);
    void jitter(float_arr & pts, double xVar, double yVar,double bVar, double fVar);
};
#endif // _KNOB_H_
