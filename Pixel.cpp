#include "Pixel.h"

// Instantiate common pixel formats here

template class Pixel16<0x01, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0>;
template class Pixel16<0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0>;

template class Pixel24<0, 1, 2>;
template class Pixel24<2, 1, 0>;

template class Pixel32<0, 1, 2, 3>;
template class Pixel32<3, 0, 1, 2>;
template class Pixel32<3, 2, 1, 0>;
template class Pixel32<0, 3, 2, 1>;
