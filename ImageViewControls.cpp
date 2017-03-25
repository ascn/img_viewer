#include <QLabel>
#include <QKeyEvent>
#include <QDebug>
#include "ImageViewControls.h"

ImageViewControls::ImageViewControls(QWidget *parent) :
	QLabel(parent) {

	this->setFocusPolicy(Qt::ClickFocus);

}

ImageViewControls::~ImageViewControls() {

}

void ImageViewControls::keyPressEvent(QKeyEvent *ev) {
	if (ev->key() == Qt::Key_Left) {

		emit rotateLeft();
	} else if (ev->key() == Qt::Key_Right) {
		emit rotateRight();
	} else if (ev->key() == Qt::Key_Up) {
		emit rotateUp();
	} else if (ev->key() == Qt::Key_Down) {
		emit rotateDown();
	} else if (ev->key() == Qt::Key_1) {
		emit increaseFOV();
	} else if (ev->key() == Qt::Key_2) {
		emit decreaseFOV();
	} else if (ev->key() == Qt::Key_D) {
		emit translateRight();
	} else if (ev->key() == Qt::Key_A) {
		emit translateLeft();
	} else if (ev->key() == Qt::Key_Q) {
		emit translateUp();
	} else if (ev->key() == Qt::Key_E) {
		emit translateDown();
	} else if (ev->key() == Qt::Key_W) {
		emit zoomIn();
	} else if (ev->key() == Qt::Key_S) {
		emit zoomOut();
	} else {
		QLabel::keyPressEvent(ev);
	}
}