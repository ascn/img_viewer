#ifndef IMG_VIEWER_H
#define IMG_VIEWER_H

#include <QMainWindow>
#include <QCheckBox>
#include <QLabel>
#include <QPixmap>
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>

#include "rasterize.h"

// ":" is just like "extends" in Java
class ImageViewer : public QMainWindow {
  Q_OBJECT // no ; required.  Put this inside any Qt GUI class

public:
  // enums are a convenient way of defining constants
  // By default they map to 0, 1, 2, ..., unless you specify otherwise
  // (This is different from Java, where enums are not numeric.)
  //
  // RED, GREEN, BLUE are all powers of two so we can | the values
  // together to specify multiple color channels in a single int
  enum { RED = 1, GREEN = 2, BLUE = 4 };

  
  // "explicit" means only use this constructor when you instantiate
  // an object, don't use it to cast from one type to another
  explicit ImageViewer(QWidget *parent = 0); 

  // make destructors virtual if you ever want to
  // subclass this, or C++ will could up calling
  // the wrong destructor!
  virtual ~ImageViewer();

public slots:
  /* Signals/Slots are a Qt thing, not a C++ thing.  They only work with Qt
   * and Qt's build process, which includes a "meta-object compiler" that
   * injects messy C++ code into your class to implement them.
   *
   * The idea is that some classes can "signal" certain events (e.g. a
   * checkbox was just clicked) and include information about the even
   * (e.g. whether the box is now checked or unchecked).  Signals look
   * just like function calls, except the function itself is never
   * defined.  Other classes can provide "slots" to receive these
   * events.  These look just like regular class methods, except they
   * are defined under a "slots" label in the header.  Finally, anyone
   * else (e.g. your main function) can connect specific signals to
   * specific slots.  So, for example, your main function can decree
   * that when a certain checkbox is clicked, a slot in your main
   * window class should get called.  Qt keeps track of which slots
   * get called whenever a certain signal is emitted, and takes care
   * of making the calls.
   */

  void shadingOptionChanged(int index);
  void rasterizeButtonClicked();
  void cameraOptionsChanged();

private:
  QGroupBox *cameraDockContents;
  QDoubleSpinBox *cam_left_box;
  QDoubleSpinBox *cam_right_box;
  QDoubleSpinBox *cam_bottom_box;
  QDoubleSpinBox *cam_top_box;
  QDoubleSpinBox *cam_near_box;
  QDoubleSpinBox *cam_far_box;
  QDoubleSpinBox *cam_eye_x_box;
  QDoubleSpinBox *cam_eye_y_box;
  QDoubleSpinBox *cam_eye_z_box;
  QDoubleSpinBox *cam_cen_x_box;
  QDoubleSpinBox *cam_cen_y_box;
  QDoubleSpinBox *cam_cen_z_box;
  QDoubleSpinBox *cam_up_x_box;
  QDoubleSpinBox *cam_up_y_box;
  QDoubleSpinBox *cam_up_z_box;
  QLabel *cam_left_label;
  QLabel *cam_right_label;
  QLabel *cam_bottom_label;
  QLabel *cam_top_label;
  QLabel *cam_near_label;
  QLabel *cam_far_label;
  QLabel *cam_eye_x_label;
  QLabel *cam_eye_y_label;
  QLabel *cam_eye_z_label;
  QLabel *cam_cen_x_label;
  QLabel *cam_cen_y_label;
  QLabel *cam_cen_z_label;
  QLabel *cam_up_x_label;
  QLabel *cam_up_y_label;
  QLabel *cam_up_z_label;

  QLabel *imgLabel;
  QPixmap pixmap;
  QImage img;
  QLabel *objFileLabel;
  QGroupBox *shadingGroup;
  QComboBox *shadingOptionBox;
  QPushButton *rasterizeButton;

  QDockWidget *cameraDock;

  QString obj_file;
  camera_mat_t *camera;
  e_shader shadingOption;

  void createActions();
  void createMenus();
  QMenu *fileMenu;
  QAction *openObjAct;
  QAction *openCamAct;
  QAction *openImgAct;
  QAction *saveImgAct;
  QAction *rasterizeAct;

private slots:
  void open_obj();
  void open_cam();
  void open_img();
  void save();
  void rasterize_wrapper();
};

#endif /* IMG_VIEWER_H */
