#include "guiv2.h"
#include "ui_guiv2.h"

GUIv2::GUIv2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUIv2)
{
    ui->setupUi(this);
}

GUIv2::~GUIv2()
{
    delete ui;
}
