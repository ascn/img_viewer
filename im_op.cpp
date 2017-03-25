#include <QImage>
#include <QColor>
#include <cmath>
#include <algorithm>
#include <vector>

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

QImage transpose(QImage *in) {
	QImage out(in->height(), in->width(), in->format());
	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height(); ++j) {
			out.setPixel(j, i, in->pixel(i, j));
		}
	}
	return out;
}

QImage boxBlur(QImage *in, int radius, QProgressBar *qpb) {
	QImage out(in->width(), in->height(), in->format());
	qpb->setRange(0, in->width() * in->height());

	int pix_count = (2 * radius + 1) * (2 * radius + 1);
	int curr_pix = 0;

	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height(); ++j) {
			if (i < radius || i + radius + 1 > in->width() ||
					j < radius || j + radius + 1 > in->height()) {
				out.setPixel(i, j, qRgb(0, 255, 0));
				continue;
			}
			float rSum = 0; float gSum = 0; float bSum = 0;
			for (int k = i - radius; k < i + radius + 1; ++k) {
				for (int l = j - radius; l < j + radius + 1; ++l) {
					rSum += qRed(in->pixel(k, l));
					gSum += qGreen(in->pixel(k, l));
					bSum += qBlue(in->pixel(k, l));
				}
			}
			rSum /= pix_count;
			gSum /= pix_count;
			bSum /= pix_count;
			out.setPixel(i, j, qRgb((int) rSum, (int) gSum, (int) bSum));

			curr_pix++;
			qpb->setValue(curr_pix);
		}
	}

	qpb->setValue(qpb->maximum());

	return out;
}

QImage medianFilter(QImage *in, int radius, QProgressBar *qpb) {
	QImage out(in->width(), in->height(), in->format());
	qpb->setRange(0, in->width() * in->height());

	int curr_pix = 0;

	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height(); ++j) {
			if (i < radius || i + radius + 1 > in->width() ||
					j < radius || j + radius + 1 > in->height()) {
				out.setPixel(i, j, qRgb(0, 255, 0));
				continue;
			}
			std::vector<int> r_vec;
			std::vector<int> g_vec;
			std::vector<int> b_vec;
			for (int k = i - radius; k < i + radius + 1; ++k) {
				for (int l = j - radius; l < j + radius + 1; ++l) {
					r_vec.push_back(qRed(in->pixel(k, l)));
					g_vec.push_back(qGreen(in->pixel(k, l)));
					b_vec.push_back(qBlue(in->pixel(k, l)));
				}
			}
			std::nth_element(r_vec.begin(), r_vec.begin() + r_vec.size() / 2, r_vec.end());
			std::nth_element(g_vec.begin(), g_vec.begin() + g_vec.size() / 2, g_vec.end());
			std::nth_element(b_vec.begin(), b_vec.begin() + b_vec.size() / 2, b_vec.end());
			int r_med = r_vec[r_vec.size() / 2];
			int g_med = g_vec[g_vec.size() / 2];
			int b_med = b_vec[b_vec.size() / 2];
			out.setPixel(i, j, qRgb(r_med, g_med, b_med));

			curr_pix++;
			qpb->setValue(curr_pix);
		}
	}
	qpb->setValue(qpb->maximum());
	return out;
}

QImage gaussianBlur(QImage *in, int radius, float sigma) {
	QImage out(in->width(), in->height(), in->format());

	return out;
}

QImage sobel(QImage *in) {
	grayscale(in);
	QImage out(in->width(), in->height(), in->format());
	QImage Gx(in->width(), in->height(), in->format());
	QImage Gy(in->width(), in->height(), in->format());
	Gx.fill(qRgb(0, 0, 0));
	Gy.fill(qRgb(0, 0, 0));

	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height(); ++j) {
			if (i < 1 || i > in->width() + 1 ||
					j < 1 || j > in->height() + 1) { continue; }
			int p1 = qRed(in->pixel(i - 1, j - 1));
			int p2 = qRed(in->pixel(i - 1, j));
			int p3 = qRed(in->pixel(i - 1, j + 1));
			int p4 = qRed(in->pixel(i + 1, j - 1));
			int p5 = qRed(in->pixel(i + 1, j));
			int p6 = qRed(in->pixel(i + 1, j + 1));
			int val = p1 + 2 * p2 + p3 - p4 - 2 * p5 - p6;
			Gx.setPixel(i, j, qRgb(val, val, val));
		}
	}

	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height(); ++j) {
			if (i < 1 || i > in->width() + 1 ||
					j < 1 || j > in->height() + 1) { continue; }
			int p1 = qRed(in->pixel(i - 1, j - 1));
			int p2 = qRed(in->pixel(i, j - 1));
			int p3 = qRed(in->pixel(i + 1, j - 1));
			int p4 = qRed(in->pixel(i - 1, j + 1));
			int p5 = qRed(in->pixel(i, j + 1));
			int p6 = qRed(in->pixel(i + 1, j + 1));
			int val = p1 + 2 * p2 + p3 - p4 - 2 * p5 - p6;
			Gy.setPixel(i, j, qRgb(val, val, val));
		}
	}

	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height(); ++j) {
			int x = qRed(Gx.pixel(i, j));
			int y = qRed(Gy.pixel(i, j));
			int val = sqrt(x * x + y * y);
			out.setPixel(i, j, qRgb(val, val, val));
		}
	}

	return out;
}