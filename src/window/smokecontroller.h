#ifndef SMOKECONTROLLER_H
#define SMOKECONTROLLER_H

#include <QWidget>

namespace Ui {
class SmokeController;
}

class SmokeController : public QWidget
{
    Q_OBJECT

public:
    explicit SmokeController(QWidget *parent = 0);
    ~SmokeController();

private:
    Ui::SmokeController *ui;
};

#endif // SMOKECONTROLLER_H
