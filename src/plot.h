/* plot.h
 */

#ifndef PLOT_H
#define PLOT_H 
#ifdef HAVE_QT

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "options.h"
#include "sim.h"

typedef std::map<std::string,double*> DensityMap;
typedef std::map<std::string,QwtPlotCurve*> CurveMap;

class Plot : public QwtPlot
{
   Q_OBJECT

   public:
      // Constructor
      Plot( Options* initOptions, Sim* initSim, QWidget* parent = 0 );

   public slots:
      void update();

   private:
      Options* o;
      Sim* sim;
      double* iterData;
      DensityMap density;
      CurveMap curves;
};

#endif /* HAVE_QT */
#endif /* PLOT_H */

