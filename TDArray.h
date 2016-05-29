#ifndef TDARRAY_H
#define TDARRAY_H

template <class T> class TDArray {
public:
    TDArray(int rows,int columns) {
        value = new T[rows*columns];
        this->rows = rows;
        this->columns = columns;
    }

    ~TDArray() {
        delete[] value;
    }

    T * operator[] (int row) {
       return value + (columns*row);
    }

private:
    T * value;
    int rows;
    int columns;
};

#endif // TDARRAY_H
