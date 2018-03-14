#include "graphe2.h"
#define MAXCURVES 15

#ifndef WITHOUT_QT
#include "ui_graphe2.h"
QColor Graphe2::baseList(int i){
    switch(i){
    case 0: {return QColor(Qt::darkGreen);}
    case 1: {return QColor(Qt::red);}
    case 2: {return QColor(Qt::cyan);}
    case 3: {return QColor(Qt::darkMagenta);}
    case 4: {return QColor(Qt::darkBlue);}
    case 5: {return QColor(Qt::gray);}
    case 6: {return QColor(Qt::green);}
    case 7: {return QColor(Qt::darkRed);}
    case 8: {return QColor(Qt::darkYellow);}
    case 9: {return QColor(Qt::magenta);}
    case 10: {return QColor(Qt::blue);}
    case 11: {return QColor(Qt::darkGray);}
    case 12: {return QColor(Qt::lightGray);}
    case 13: {return QColor(Qt::yellow);}
    case 14: {return QColor(Qt::darkCyan);}

    }
    return QColor(Qt::black);
}

Graphe2::Graphe2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphe2)
{
    ui->setupUi(this);
    BigPlot = new QwtPlot(ui->widgetGraphe);
    BigPlot->resize(ui->widgetGraphe->width(), ui->widgetGraphe->height());
    BigPlot->axisWidget(QwtPlot::yLeft)->setFont(QFont(QString("Arial"),7));
    BigPlot->axisWidget(QwtPlot::xBottom)->setFont(QFont(QString("Arial"),7));
    // don't do setNbCurves or vlear before creation of a new BigPlot
    setNbCurves(1);
}

Graphe2::~Graphe2()
{
    delete ui;
}

void Graphe2::clear(){
    setNbCurves(nbCurves);
}

void Graphe2::logarithmic(bool newState){
    if(newState) BigPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine ());
    else BigPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine ());
}

void Graphe2::setNbCurves(int _nbCurves){
    if((_nbCurves < 1) || (_nbCurves > MAXCURVES)) {cerr << "ERR: Graphe::setNbCurves(" << _nbCurves << ", not an authorized value\n"; return;}
    for(int i = 0; i < (int) curveData.size(); ++i){
        if(curveData[i]){
            curveData[i]->detach();
            delete curveData[i];
        }
    }
    for(int i = 0; i < (int) Data.size(); ++i){
        if(Data[i]) delete Data[i];
    }
    for(int i = 0; i < (int) xpoints.size(); ++i){
        if(xpoints[i]) delete xpoints[i];
    }
    nbCurves = _nbCurves;
    curveData.clear(); Data.clear(); xpoints.clear();
    curveData.resize(nbCurves, NULL);
    Data.resize(nbCurves);
    xpoints.resize(nbCurves);
    for(int i = 0; i < nbCurves; ++i){
        curveData[i] = new QwtPlotCurve("Essai Titre");
        Data[i] = new vector<double>();
        xpoints[i] = new vector<double>();
    }
    //BigPlot->replot();
}

void Graphe2::Plot(int IDCurve, vector<double> y_to_plot, vector<double> x_to_plot, QString _titre, QColor _color, Qt::PenStyle ps){
    if((IDCurve < 0) || (IDCurve >= nbCurves)) {cerr << "ERR Graphe::Plot(IDCurve=" << IDCurve <<",...), curve ID not valid (only " << nbCurves << " curves defined. Indices start at 0.\n"; return;}
    int size = y_to_plot.size();
    if((int) x_to_plot.size() < size) {cerr << "ERR: Graphe1::Plot , error, the vector of x points is smaller than the vector of y points\n "; return;}
    x_to_plot.resize(size);

    // cerr << "Size of data plotted : " << size << endl;
    /*for(int i = 0; i < size; ++i){
        cerr << i << "\t" << x_to_plot[i] << "\t" << y_to_plot[i] << endl;
    }*/
    int i = IDCurve;
    Data[i]->clear();
    delete Data[i];
    Data[i] = new vector<double>(y_to_plot);
    xpoints[i]->clear();
    delete xpoints[i];
    xpoints[i] = new vector<double>(x_to_plot);
    if(curveData[i]){
        curveData[i]->detach();
        delete curveData[i];
    }
        curveData[i] = new QwtPlotCurve(_titre.toStdString().c_str());
        curveData[i]->setPen(QColor(Qt::black), 1.0, Qt::DashLine);

        curveData[i]->setPen(QPen(Qt::DashDotLine));
        curveData[i]->setPen(_color);
        curveData[i]->setPen(QPen(QBrush(_color), 1.3, ps));

        //curveData[i]->setStyle(QwtPlotCurve::Dots);
        curveData[i]->setRenderHint(QwtPlotItem::RenderAntialiased);

        curveData[i]->setSamples(&(*xpoints[IDCurve])[0],&(*Data[IDCurve])[0], size);    // I hope
        curveData[i]->attach(BigPlot);

        //BigPlot->setAxisScale(2, xpoints[0], xpoints[xpoints.size()-1]);
        //BigPlot->setAxisScale(3, xpoints[0], xpoints[xpoints.size()-1]);
        BigPlot->replot();

}

void Graphe2::setTitle(QString _titre){
    BigPlot->setTitle(_titre);
}

void Graphe2::exportToPng(QString _file){
    cerr << "ERR : export to PNG is not implanted in Graphe2. Please use QCustom plot by using #define USE_QCP in common.h" << endl;
}

void Graphe2::Example(){
    double p1[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5};
    double p2[] = {0, 0.5, 0.8, 0.3, 0.2, 0.1};
    vector<double> xs = vector<double>(p1, &p1[5]);
    vector<double> ys = vector<double>(p2, &p2[5]);
    setNbCurves(1);
    Plot(0, ys, xs, QString("Example of plot"), QColor(Qt::darkBlue));
}

#endif
