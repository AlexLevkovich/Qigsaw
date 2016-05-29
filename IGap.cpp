#include "IGap.h"

IGap::IGap(int start, int end): QPoint(start,end) {}

IGap::IGap(): QPoint() {}

int IGap::belong(int value) {
    if (y() >= value && x() <= value) return 0;
    if (y() < value) return 1;
    if (x() > value) return -1;

    return 0;
}

int IGap::getStart() {
    return (int)x();
}

int IGap::getEnd() {
    return (int)y();
}

IGap & IGap::operator= (const IGap & gap) {
    *((QPoint *)this) = *((QPoint *)&gap);
    return *this;
}
