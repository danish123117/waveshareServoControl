
#include <cmath>

#define L1 0.17
#define L2 0.17
#define L3 0.17
#define L4 0.17
#define L5 0.1

void forwardKinematics ( double enc1_data , double enc2_data , double &x3 , double &y3) {
double t1 = ((3072-enc1_data)*3.1416)/2048 ;
double t5 = ((3072- enc2_data)*3.1416)/2048;
double x2 = L1 * cos (t1);
double y2 = L1 * sin (t1);
double x4 = (L4 * cos (t5)) - L5;
double y4 = L4 * sin (t5);
double P4P2 = sqrt (( pow (x4 - x2 , 2)) + (pow (y4 - y2 , 2)));
double PHP2 = (( pow (L2 , 2)) - ( pow (L3 , 2)) + ( pow (P4P2 , 2))) / (2 * P4P2 );
double PHP3 = sqrt (( pow (L2 , 2)) - ( pow (PHP2 , 2)));
double xH = x2 + (( PHP2 / P4P2 ) * (x4 - x2));
double yH = y2 + (( PHP2 / P4P2 ) * (y4 - y2));
x3 = xH - (( PHP3 / P4P2 ) * (y4 - y2));
y3 = yH + (( PHP3 / P4P2 ) * (x4 - x2));
}



bool inverseKinematics(double x3, double y3, int &enc1, int &enc2) {
    double L1 = 0.17;
    double L2 = 0.17;
    double L3 = 0.17;
    double L4 = 0.17;
    double L5 = 0.1;  
  
  // Distances for circle intersections
    double d_left  = hypot(x3, y3);
    double d_right = hypot(x3 + L5, y3);

    // Reachability check
    if (d_left > (L1 + L2) || d_left < fabs(L1 - L2)) return false;
    if (d_right > (L3 + L4) || d_right < fabs(L4 - L3)) return false;

    // --- Left side (joint 2) ---
    double a1 = (L1*L1 - L2*L2 + d_left*d_left) / (2 * d_left);
    double h1 = sqrt(L1*L1 - a1*a1);
    double x2 = (a1/d_left) * x3 + (h1/d_left) * y3;
    double y2 = (a1/d_left) * y3 - (h1/d_left) * x3;
    double t1 = atan2(y2, x2);

    // --- Right side (joint 4) ---
    double dx = x3 + L5;
    double dy = y3;
    double d  = hypot(dx, dy);
    double a2 = (L4*L4 - L3*L3 + d*d) / (2 * d);
    double h2 = sqrt(L4*L4 - a2*a2);
    double x4 = (a2/d) * dx - (h2/d) * dy - L5;
    double y4 = (a2/d) * dy + (h2/d) * dx;
    double t5 = atan2(y4, x4 + L5);

    // Convert radians back to encoder counts
    enc1 = (int)lround(3072.0 - (t1 * 2048.0 / M_PI));
    enc2 = (int)lround(3072.0 - (t5 * 2048.0 / M_PI));

    return true;   // success
}