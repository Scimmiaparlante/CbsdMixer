#include "plotwindow.h"
#include "ui_plotwindow.h"
#include <qwt_scale_engine.h>
#include <signal.h>

#define MAX_Y 1000000000

#define MIN_AMPL -10
#define MAX_AMPL 10

#define NUM_SLIDERS 7


PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
{
    ui->setupUi(this);

    //set up the first plot
    ui->preFrequencyPlot->setAxisAutoScale(QwtPlot::yLeft, false);
    ui->preFrequencyPlot->setAxisScale(QwtPlot::yLeft, 1, MAX_Y);
    ui->preFrequencyPlot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLogScaleEngine );
    ui->preFrequencyPlot->setAxisScale(QwtPlot::xBottom, SAMPLE_RATE/NUM_SAMPLES, SAMPLE_RATE/2);
    ui->preFrequencyPlot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLogScaleEngine );

    //set up the second plot
    ui->postFrequencyPlot->setAxisAutoScale(QwtPlot::yLeft, false);
    ui->postFrequencyPlot->setAxisScale(QwtPlot::yLeft, 1, MAX_Y);
    ui->postFrequencyPlot->setAxisScaleEngine( QwtPlot::yLeft, new QwtLogScaleEngine );
    ui->postFrequencyPlot->setAxisScale(QwtPlot::xBottom, SAMPLE_RATE/NUM_SAMPLES, SAMPLE_RATE/2);
    ui->postFrequencyPlot->setAxisScaleEngine( QwtPlot::xBottom, new QwtLogScaleEngine );

    //create curve for plot 1
    curve_pre = new QwtPlotCurve();
    //curve_pre->setTitle( "Some Points" );
    curve_pre->setPen( Qt::blue, 1 );
    curve_pre->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve_pre->attach( ui->preFrequencyPlot );

    //create curve for plot 2
    curve_post = new QwtPlotCurve();
    curve_post->setPen( Qt::blue, 1 );
    curve_post->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve_post->attach( ui->postFrequencyPlot );

    //decide the mixer's frequencies
    std::vector<double> freq = {60, 170, 500, 1000, 5000, 12000};

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
    }

    //init volume slider event
    connect(ui->VolumeSlider, SIGNAL(valueChanged(double)), this, SLOT(update_filter(double)));

    //mixer startup (+ start reproduction)
    myMixer = new Mixer(freq, COSINE_FILTERING);
    mixerThread = new std::thread(&Mixer::startAcquisition, myMixer);
    mixerThread2 = new std::thread(&Mixer::startReproduction, myMixer);


    //set timer to trigger the update function
    plotTimer = new QTimer(nullptr);
    connect(plotTimer, SIGNAL(timeout()), this, SLOT(replot()));
    plotTimer->start(1000*NUM_SAMPLES/SAMPLE_RATE);
}

PlotWindow::~PlotWindow()
{
    delete ui;
    delete myMixer;
    delete plotTimer;

    pthread_kill(mixerThread->native_handle(), SIGKILL);
    pthread_kill(mixerThread2->native_handle(), SIGKILL);

    delete mixerThread;
    delete mixerThread2;
}


void PlotWindow::replot()
{
    //-------------pre processing frequencies plot------------------------
    double* data = myMixer->get_rawFrequencies();
    QPolygonF points;
    for (long int i = 0; i < COMP_SAMPLES; ++i)
    {
        points << QPointF( static_cast<double>(SAMPLE_RATE)/NUM_SAMPLES*i, data[i] );
    }
    curve_pre->setSamples( points );

    ui->preFrequencyPlot->replot();

    //-------------post processing frequencies plot----------------------
    data = myMixer->get_processedFrequencies();
    QPolygonF points2;
    for (long int i = 0; i < COMP_SAMPLES; ++i)
    {
        points2 << QPointF( static_cast<double>(SAMPLE_RATE)/NUM_SAMPLES*i, data[i] );
    }
    curve_post->setSamples( points2 );

    ui->postFrequencyPlot->replot();
}


void PlotWindow::update_filter(double val)
{
    QwtSlider* sender = reinterpret_cast<QwtSlider*>(QObject::sender());

    int i;
    for(i = 0; i <= NUM_SLIDERS; ++i)
    {
        //search which slider have been updated
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
