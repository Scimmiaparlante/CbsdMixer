#ifndef FILTERWINDOW_H
#define FILTERWINDOW_H

#include <QMainWindow>

#include "ui_filterwindow.h"

#include <qwt_plot_curve.h>

namespace Ui {
class FilterWindow;
}



class FilterWindow : public QMainWindow
{
    Q_OBJECT

private:




public:
    Ui::FilterWindow* ui;
    QwtPlotCurve* curve_filter;

    explicit FilterWindow(int x_min, int x_max, QWidget *parent = nullptr);
    ~FilterWindow();

signals:

public slots:
};

#endif // FILTERWINDOW_H
