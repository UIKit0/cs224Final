#include "smokecontroller.h"
#include "ui_smokecontroller.h"

SmokeController::SmokeController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SmokeController)
{
    ui->setupUi(this);
}

SmokeController::~SmokeController()
{
    delete ui;
}
