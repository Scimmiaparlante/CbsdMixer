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
    Ui::FilterWindow* ui;
    QwtPlotCurve* curve_filter;

    double resolution;
    unsigned int num_elem;

public:
    explicit FilterWindow(double resolution_, unsigned int num_elem_, QWidget *parent = nullptr);
    ~FilterWindow();

public slots:

    void replot(double* filter);
};

#endif // FILTERWINDOW_H
