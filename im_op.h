#include <QImage>
#include <QProgressBar>

void grayscale(QImage *in);

void flip(QImage *in);

void flop(QImage *in);

QImage transpose(QImage *in);

QImage boxBlur(QImage *in, int radius, QProgressBar *qpb);

QImage medianFilter(QImage *in, int radius, QProgressBar *qpb);

QImage gaussianBlur(QImage *in, int radius, float sigma);

QImage sobel(QImage *in);