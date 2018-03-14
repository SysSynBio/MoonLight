#ifndef GRAPHE2_H
#define GRAPHE2_H

#include "common.h"
#ifndef WITHOUT_QT

#ifdef QT5
#include <QWidget>
#endif
#ifdef QT4
#include <QtGui/QWidget>
#endif


#include "qwt6.1.0/qwt_plot.h"
#include "qwt6.1.0/qwt_scale_map.h"
#include "qwt6.1.0/qwt_color_map.h"
#include "qwt6.1.0/qwt_symbol.h"
#include "qwt6.1.0/qwt.h"
#include "qwt6.1.0/qwt_plot_curve.h"
#include "qwt6.1.0/qwt_color_map.h"
#include "qwt6.1.0/qwt_scale_widget.h"
#include "qwt6.1.0/qwt_scale_draw.h"
#include "qwt6.1.0/qwt_plot_zoomer.h"
#include "qwt6.1.0/qwt_plot_panner.h"
#include "qwt6.1.0/qwt_plot_layout.h"
#include "qwt6.1.0/qwt_scale_engine.h"

#include <iostream>
#include <vector>
using namespace std;



namespace Ui {
class Graphe2;
}

class Graphe2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit Graphe2(QWidget *parent = 0);
    ~Graphe2();

    QwtPlot* BigPlot;
    vector< vector<double>* > Data;
    vector< vector<double>* > xpoints;
    vector<QwtPlotCurve*> curveData;
    int nbCurves;

    void setTitle(QString _titre);
    void exportToPng(QString _file);
    void setNbCurves(int _nbCurves);
    void Plot(int IDCurve, vector<double> y_to_plot, vector<double> x_to_plot, QString _titre, QColor _color = QColor(Qt::white), Qt::PenStyle ps = Qt::SolidLine);
    void Example();
    void clear();
    void setColorScale(int){}
    void logarithmic(bool newState);
    QColor baseList(int i);
private:
    Ui::Graphe2 *ui;
};
#endif

#endif // GRAPHE2_H
