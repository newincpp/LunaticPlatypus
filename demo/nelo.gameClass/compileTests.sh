#!/bin/sh
dmd nelo.d -fPIC -ofneloD.so -shared -defaultlib=libphobos2.so 
