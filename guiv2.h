#ifndef GUIV2_H
#define GUIV2_H

#include <QMainWindow>

namespace Ui {
class GUIv2;
}

class GUIv2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUIv2(QWidget *parent = 0);
    ~GUIv2();

private:
    Ui::GUIv2 *ui;
};

#endif // GUIV2_H
