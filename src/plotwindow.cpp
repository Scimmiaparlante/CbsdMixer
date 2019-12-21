#include "plotwindow.h"
#include "ui_plotwindow.h"
#include "ui_filterwindow.h"
#include <qwt_scale_engine.h>

#define MAX_Y 1000000000

#define MIN_AMPL -10
#define MAX_AMPL 5

#define NUM_SLIDERS 7


PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow),
    filter_window(static_cast<double>(SAMPLE_RATE)/NUM_SAMPLES, COMP_SAMPLES, this)
{
    ui->setupUi(this);

    //set up the plots and curves
    curve_pre = new QwtPlotCurve();
    curve_post = new QwtPlotCurve();
    init_logFreqPlot(ui->preFrequencyPlot, curve_pre);
    init_logFreqPlot(ui->postFrequencyPlot, curve_post);

    //decide the mixer's frequencies
    std::vector<double> freq = {60, 170, 500, 1000, 5000, 12000};

    //initialize the sliders' panel
    init_slider_panel(freq);

    //mixer startup (+ start reproduction)
    myMixer = new Mixer(freq, COSINE_FILTERING, OVERLAP_WINDOWING);
    myMixer->start();

    //set timer to trigger the update function
    plotTimer = new QTimer(nullptr);
    connect(plotTimer, SIGNAL(timeout()), this, SLOT(replot()));
    plotTimer->start(1000*NUM_SAMPLES/SAMPLE_RATE);

    //---------------------- SETUP OF FILTER WINDOW ----------------------------------
    //connect the menu entry to a function
    connect(ui->actionShow_filter, SIGNAL(triggered(bool)), this, SLOT(show_filter_window()));
}


void PlotWindow::init_logFreqPlot(QwtPlot* plot, QwtPlotCurve* curve)
{
    //set up the first plot
    plot->setAxisAutoScale(QwtPlot::yLeft, false);
    plot->setAxisScale(QwtPlot::yLeft, 1, MAX_Y);
    plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
    plot->setAxisScale(QwtPlot::xBottom, SAMPLE_RATE/NUM_SAMPLES, SAMPLE_RATE/2);
    plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);

    curve->setPen(Qt::blue, 1);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(plot);
}

void PlotWindow::init_slider_panel(std::vector<double> freq)
{
    for(int i = 1; i < NUM_SLIDERS; ++i)
    {
        //initialize labels
        unsigned long index = static_cast<unsigned long>(i-1);
        QLayoutItem* item = ui->Sliders_pane->itemAtPosition(1, i);
        if(item == nullptr)
            break;

        QwtTextLabel* label = reinterpret_cast<QwtTextLabel*>(item->widget());
        if (label != nullptr)
            label->setText( (std::to_string(static_cast<int>(freq[index])) + " Hz (db)").c_str() );

        //initialize sliders
        item = ui->Sliders_pane->itemAtPosition(0, i);
        if(item == nullptr)
            break;

        QwtSlider* slider = reinterpret_cast<QwtSlider*>(item->widget());
        if (slider != nullptr)
            slider->setScale(MIN_AMPL, MAX_AMPL);

        connect(slider, SIGNAL(valueChanged(double)), this, SLOT(update_filter(double)));

        //init volume slider event
        connect(ui->VolumeSlider, SIGNAL(valueChanged(double)), this, SLOT(update_filter(double)));
    }
}

PlotWindow::~PlotWindow()
{
    delete ui;
    delete myMixer;
    delete plotTimer;
}


void PlotWindow::replot()
{
    //-------------pre processing frequencies plot------------------------
    double* data = myMixer->get_rawFrequencies();
    QPolygonF points;
    for (long int i = 0; i < COMP_SAMPLES; ++i)
        points << QPointF( static_cast<double>(SAMPLE_RATE)/NUM_SAMPLES*i, data[i] );

    curve_pre->setSamples( points );

    ui->preFrequencyPlot->replot();

    //-------------post processing frequencies plot----------------------
    data = myMixer->get_processedFrequencies();
    QPolygonF points2;
    for (long int i = 0; i < COMP_SAMPLES; ++i)
        points2 << QPointF( static_cast<double>(SAMPLE_RATE)/NUM_SAMPLES*i, data[i] );

    curve_post->setSamples( points2 );

    ui->postFrequencyPlot->replot();

    //-------------if filter_window is open: filter plot----------------------
    if(filter_window.isVisible())
        filter_window.replot(myMixer->get_filter());
}



void PlotWindow::update_filter(double val)
{
    QwtSlider* sender = reinterpret_cast<QwtSlider*>(QObject::sender());

    int i;
    for(i = 0; i <= NUM_SLIDERS; ++i)
    {
        //search which slider has been updated
        QLayoutItem* item = ui->Sliders_pane->itemAtPosition(0, i);
        if(item == nullptr)
            return;

        QwtSlider* slider = reinterpret_cast<QwtSlider*>(item->widget());
        if (slider == nullptr)
            return;

        if(slider == sender)
            break;
    }

    if(i == 0)
        myMixer->set_volume(val/100);
    else
        myMixer->set_filterValue(i-1, pow(10, val));
}

