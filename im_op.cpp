#include <QImage>
#include <QColor>
#include <cmath>
#include <algorithm>
#include <vector>

#include "im_op.h"

void grayscale(QImage *in, QProgressBar *qpb) {
	qpb->setRange(0, in->height() * in->width());
	int curr_pix = 0;
	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			QRgb tmp = in->pixel(j, i);
			int val = .299 * qRed(tmp) + \
					  .587 * qGreen(tmp) + \
					  .114 * qBlue(tmp);
			in->setPixel(j, i, qRgb(val, val, val));
		}
		curr_pix += in->width();
		qpb->setValue(curr_pix);
	}
}

void flip(QImage *in, QProgressBar *qpb) {
	qpb->setRange(0, in->height() * in->width() / 2);
	int curr_pix = 0;
	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width() / 2; ++j) {
			QRgb tmp = in->pixel(j, i);
			in->setPixel(j, i, in->pixel(in->width() - j - 1, i));
			in->setPixel(in->width() - j - 1, i, tmp);
		}
		curr_pix += in->width() / 2;
		qpb->setValue(curr_pix);
	}
}

void flop(QImage *in, QProgressBar *qpb) {
	qpb->setRange(0, in->height() * in->width() / 2);
	int curr_pix = 0;
	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height() / 2; ++j) {
			QRgb tmp = in->pixel(i, j);
			in->setPixel(i, j, in->pixel(i, in->height() - j - 1));
			in->setPixel(i, in->height() - j - 1, tmp);
		}
		curr_pix += in->width() / 2;
		qpb->setValue(curr_pix);
	}
}

QImage transpose(QImage *in, QProgressBar *qpb) {
	qpb->setRange(0, in->height() * in->width());
	int curr_pix = 0;
	QImage out(in->height(), in->width(), in->format());
	for (int i = 0; i < in->width(); ++i) {
		for (int j = 0; j < in->height(); ++j) {
			out.setPixel(j, i, in->pixel(i, j));
		}
		curr_pix += in->height();
		qpb->setValue(curr_pix);
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
		}
		curr_pix += in->height();
		qpb->setValue(curr_pix);
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
		}
		curr_pix += in->height();
		qpb->setValue(curr_pix);
	}
	qpb->setValue(qpb->maximum());
	return out;
}

double weight(int dist, float sigma) {
	return exp((-dist * dist) / (2.f * pow(sigma, 2)));
}

QImage gaussianBlur(QImage *in, int radius, float sigma, QProgressBar *qpb) {
	QImage row(in->width(), in->height(), in->format());
	QImage out(in->width(), in->height(), in->format());

	qpb->setRange(0, 2 * in->width() * in->height());
	int pix_count = 0;

	float weight_sum = 0;
	for (int i = -radius; i <= radius; ++i) {
		weight_sum += weight(abs(i), sigma);
	}

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			if (j < radius || j + radius >= in->width()) { continue; }
			float rsum, gsum, bsum;
			rsum = gsum = bsum = 0;
			for (int k = j - radius; k <= j + radius; ++k) {
				QRgb curr = in->pixel(k, i);
				int x = abs(j - k);
				rsum += qRed(curr) * weight(x, sigma);
				gsum += qGreen(curr) * weight(x, sigma);
				bsum += qBlue(curr) * weight(x, sigma);
			}
			row.setPixel(j, i, qRgb(rsum / weight_sum,
									 gsum / weight_sum,
									 bsum / weight_sum));
		}
		pix_count += in->width();
		qpb->setValue(pix_count);
	}

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			if (i < radius || i + radius >= in->height() ||
					j < radius || j + radius >= in->width()) {
				out.setPixel(j, i, qRgb(0, 255, 0));
				continue;
			}
			float rsum, gsum, bsum;
			rsum = gsum = bsum = 0;
			for (int k = i - radius; k <= i + radius; ++k) {
				int x = abs(i - k);
				rsum += qRed(row.pixel(j, k)) * weight(x, sigma);
				gsum += qGreen(row.pixel(j, k)) * weight(x, sigma);
				bsum += qBlue(row.pixel(j, k)) * weight(x, sigma);
			}
			out.setPixel(j, i, qRgb(rsum / weight_sum,
									 gsum / weight_sum,
									 bsum / weight_sum));
		}
		pix_count += in->width();
		qpb->setValue(pix_count);
	}

	return out;
}

QImage sobel(QImage *in, QProgressBar *qpb) {
	grayscale(in, qpb);
	QImage out(in->width(), in->height(), in->format());

	qpb->setRange(0, 6 * in->width() * in->height());
	int pix_count = 0;

	float *Gx_int = (float *) malloc(in->width() * in->height() * sizeof(*Gx_int));
    float *Gx = (float *) malloc(in->width() * in->height() * sizeof(*Gx));
    float *Gy_int = (float *) malloc(in->width() * in->height() * sizeof(*Gy_int));
    float *Gy = (float *) malloc(in->width() * in->height() * sizeof(*Gy));

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			if (i < 1 || i + 1 >= in->height()) { continue; }
			Gx_int[i * in->width() + j] = \
				qRed(in->pixel(j, i - 1)) + \
				qRed(in->pixel(j, i)) * 2 + \
				qRed(in->pixel(j, i + 1));
		}
	}

	pix_count += in->width() * in->height();
	qpb->setValue(pix_count);

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			if (j < 1 || j + 1 >= in->height()) { continue; }
			Gx[i * in->width() + j] = \
				Gx_int[i * in->width() + (j - 1)] * -1 + \
				Gx_int[i * in->width() + (j + 1)];
		}
	}

	pix_count += in->width() * in->height();
	qpb->setValue(pix_count);

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			if (i < 1 || i + 1 >= in->height()) { continue; }
			Gy_int[i * in->width() + j] = \
				qRed(in->pixel(j, i - 1)) + \
				qRed(in->pixel(j, i + 1)) * -1;
		}
	}

	pix_count += in->width() * in->height();
	qpb->setValue(pix_count);

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			if (j < 1 || j + 1 >= in->height()) { continue; }
			Gy[i * in->width() + j] = \
				Gy_int[i * in->width() + (j - 1)] + \
				Gy_int[i * in->width() + j] * 2 + \
				Gy_int[i * in->width() + (j + 1)];
		}
	}

	pix_count += in->width() * in->height();
	qpb->setValue(pix_count);

	free(Gx_int);
    free(Gy_int);

    float *G = (float *) malloc(in->width() * in->height() * sizeof(*G));

	int max = 0;

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			int ind = i * in->width() + j;
			G[ind] = sqrt(Gx[ind] * Gx[ind] + Gy[ind] * Gy[ind]);
			if (G[ind] > max) { max = G[ind]; }
		}
	}

	pix_count += in->width() * in->height();
	qpb->setValue(pix_count);

	for (int i = 0; i < in->height(); ++i) {
		for (int j = 0; j < in->width(); ++j) {
			if (i < 1 || j < 1 || i + 1 >= in->height() || j + 1 >= in->width()) {
				out.setPixel(j, i, qRgb(0, 255, 0));
				continue;
			}
			int ind = i * in->width() + j;
			G[ind] = (G[ind] / max) * 255;
			out.setPixel(j, i, qRgb(G[ind], G[ind], G[ind]));
		}
	}

	pix_count += in->width() * in->height();
	qpb->setValue(pix_count);

	free(Gx);
    free(Gy);
    free(G);

	return out;
}