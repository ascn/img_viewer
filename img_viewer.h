#ifndef IMG_VIEWER_H
#define IMG_VIEWER_H

#include <QMainWindow>
#include <QCheckBox>
#include <QLabel>
#include <QPixmap>
#include <QAction>
#include <QMenu>

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

  void redStateChanged(int state);
  void greenStateChanged(int state);
  void blueStateChanged(int state);

private:
  void updateLabel(void);

  int channels;
  QCheckBox *rCheck, *gCheck, *bCheck;
  QLabel *imgLabel;
  QPixmap pixmap;
  QImage img;

  QString obj_file;
  camera_mat_t camera;

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
