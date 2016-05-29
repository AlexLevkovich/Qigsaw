#ifndef _IGAP_H_
#define _IGAP_H_

#include <QPoint>

class IGap: public QPoint {
public:
    IGap(int start, int end);
    IGap();

    int belong(int value);
    int getStart();
    int getEnd();

    IGap & operator= ( const IGap & gap );
};

#endif // _IGAP_H_
