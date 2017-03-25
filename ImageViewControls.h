#ifndef __IMAGE_VIEW_CONTROLS_H__
#define __IMAGE_VIEW_CONTROLS_H__

#include <QLabel>
#include <QKeyEvent>

class ImageViewControls : public QLabel {
	Q_OBJECT

public:
	explicit ImageViewControls(QWidget *parent = 0);

	virtual ~ImageViewControls();

signals:
	void rotateLeft();
	void rotateRight();
	void rotateUp();
	void rotateDown();
	void increaseFOV();
	void decreaseFOV();
	void translateRight();
	void translateLeft();
	void translateUp();
	void translateDown();
	void zoomIn();
	void zoomOut();

protected:
	virtual void keyPressEvent(QKeyEvent *ev);

};

#endif // __IMAGE_VIEW_CONTROLS_H__