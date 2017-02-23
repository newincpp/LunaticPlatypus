#pragma once
template<typename T>
T lerp(T a, T b, float f) {
    return a + f * (b - a);
}

template<typename T>
T CosineInterpolate(T y1, T y2, float mu) {
    return learp<T>(y1, y2, (1-cos(mu*PI))/2);
}

//template<typename T>
//double CubicInterpolate(
//	double y0,double y1,
//	double y2,double y3,
//	double mu)
//{
//    double a0,a1,a2,a3,mu2;
//
//    mu2 = mu*mu;
//    a0 = y3 - y2 - y0 + y1;
//    a1 = y0 - y1 - a0;
//    a2 = y2 - y0;
//    a3 = y1;
//
//    return(a0*mu*mu2+a1*mu2+a2*mu+a3);
//}
