#include <QImage>
#include <QColor>

#include "im_op.h"

void grayscale(QImage *in) {
	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			QRgb tmp = in->pixel(j, i);
			int val = .299 * qRed(tmp) + \
					  .587 * qGreen(tmp) + \
					  .114 * qBlue(tmp);
			in->setPixel(j, i, qRgb(val, val, val));
		}
	}
}

void flip(QImage *in) {
	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width() / 2; ++j) {
			QRgb tmp = in->pixel(j, i);
			in->setPixel(j, i, in->pixel(in->width() - j - 1, i));
			in->setPixel(in->width() - j - 1, i, tmp);
		}
	}
}

void flop(QImage *in) {
	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height() / 2; ++j) {
			QRgb tmp = in->pixel(i, j);
			in->setPixel(i, j, in->pixel(i, in->height() - j - 1));
			in->setPixel(i, in->height() - j - 1, tmp);
		}
	}
}