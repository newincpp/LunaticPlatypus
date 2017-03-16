#!/bin/sh
dmd nelo.d -fPIC -ofnelo.so -shared -defaultlib=libphobos2.so 
