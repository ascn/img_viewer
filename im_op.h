#include <QImage>
#include <QProgressBar>

void grayscale(QImage *in, QProgressBar *qpb);

void flip(QImage *in, QProgressBar *qpb);

void flop(QImage *in, QProgressBar *qpb);

QImage transpose(QImage *in, QProgressBar *qpb);

QImage boxBlur(QImage *in, int radius, QProgressBar *qpb);

QImage medianFilter(QImage *in, int radius, QProgressBar *qpb);

QImage gaussianBlur(QImage *in, int radius, float sigma, QProgressBar *qpb);

QImage sobel(QImage *in, QProgressBar *qpb);