#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <thread>
#include <QMainWindow>
#include <QTimer>

#include "qwt_plot.h"
#include "qwt_plot_curve.h"

#include "mixer.h"



namespace Ui {
class PlotWindow;
class FilterWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::PlotWindow* ui;
    Ui::FilterWindow* uif;
    QMainWindow filter_window;
    QwtPlotCurve* curve_pre;
    QwtPlotCurve* curve_post;
    QwtPlotCurve* curve_filter;
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
    void show_filter_window();


};


#endif // PLOTWINDOW_H
