//
//  Created by Adrian Phoulady on 8/29/19.
//  © 2019 Adrian Phoulady
//

// dynamic row-major multidimensional arrays for a faster access than vector of vectors

template <class T>
class array1d {

public:

    int const columns;
    T * const data;

    explicit array1d(int columns)
    : columns{columns},
    data{(T*) new char[columns * sizeof(T)]} { // char for not wanting a default constructor
    }

    T &operator()(int column) {
        return data[column];
    }

    ~array1d() {
        delete[] (char *) data; // (char *) for T being a non-POD
    }

};

template <class T>
class array2d {

public:

    int const rows, columns;
    T * const data;

    array2d(int rows, int columns)
    : rows{rows},
    columns{columns},
    data{(T*) new char[rows * columns * sizeof(T)]} {
    }

    T *operator()(int row) {
        return &data[row * columns];
    }

    T &operator()(int row, int column) {
        return data[row * columns + column];
    }

    ~array2d() {
        delete[] (char *) data;
    }

};

template <class T>
class array3d {

public:

    int const aisles, rows, columns;
    T * const data;

    array3d(int aisles, int rows, int columns)
    : aisles{aisles},
    rows{rows},
    columns{columns},
    data{(T*) new char[aisles * rows * columns * sizeof(T)]} {
    }

    T *operator()(int aisle) {
        return &data[aisle * rows * columns];
    }

    T *operator()(int aisle, int row) {
        return &data[(aisle * rows + row) * columns];
    }

    T &operator()(int aisle, int row, int column) {
        return data[(aisle * rows + row) * columns + column];
    }

    ~array3d() {
        delete[] (char *) data;
    }

};
