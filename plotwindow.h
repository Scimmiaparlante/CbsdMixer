#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "qwt_plot.h"
#include "qwt_plot_curve.h"

#include "mixer.h"

#include <thread>

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();

public slots:
    void replot();
    void update_filter(double val);

private:
    Ui::PlotWindow *ui;
    QwtPlotCurve *curve_pre;
    QwtPlotCurve *curve_post;
    QTimer *plotTimer;
    Mixer *myMixer;
    std::thread* mixerThread;
};

#endif // PLOTWINDOW_H
