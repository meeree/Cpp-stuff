class turtle {
public:
   turtle(double const (&position)[3], double const (&orientation)[3]);
   double * Rx(double const &theta);
   double * Ry(double const &theta);
   double * Rz(double const &theta);
   double * Mv(double const &distance);
private:
   double mOrientation[3], mPosition[3];
};
