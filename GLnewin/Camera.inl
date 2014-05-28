template<typename __DISP_TYPE> void GLnewin::Camera::syncronizeRez(__DISP_TYPE&& display) {
    typedef decltype(display.getSize()) Size_Type_;
    Size_Type_&& sz = display.getSize();
    _res.x = sz.x;
    _res.y = sz.y;
}

template<typename __DISP_TYPE> void GLnewin::Camera::syncronizeRez(const __DISP_TYPE& display) {
    typedef decltype(__DISP_TYPE::getSize()) Size_Type_;
    Size_Type_&& sz = display.getSize();
    _res.x = sz.x;
    _res.y = sz.y;
}
