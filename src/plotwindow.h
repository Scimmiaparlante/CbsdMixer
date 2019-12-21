#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <thread>
#include <QMainWindow>
#include <QTimer>

#include "qwt_plot.h"
#include "qwt_plot_curve.h"

#include "mixer.h"
#include "filterwindow.h"



namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::PlotWindow* ui;
    FilterWindow filter_window;
    QwtPlotCurve* curve_pre;
    QwtPlotCurve* curve_post;
    QTimer *plotTimer;

    Mixer *myMixer;

    std::thread* mixerThread;
    std::thread* mixerThread2;

    void init_logFreqPlot(QwtPlot* p, QwtPlotCurve* c);
    void init_slider_panel(std::vector<double> freq);

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();

public slots:
    void replot();
    void update_filter(double val);
    void show_filter_window() { filter_window.show(); }
};


#endif // PLOTWINDOW_H
