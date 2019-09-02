#pragma once

#include "common.h"

#include <iostream>
#include <vector>
#include <complex>
#include <future>

constexpr int kNumberOfMandelbrotTasks = 4;
constexpr vector2 kDefaultCenter = vector2(-1.2461, -0.0765);

template <typename TFloat>
class Mandelbrot {
    
    static_assert(std::is_floating_point<TFloat>(), "Type must be float, double or long double");
    
public:
    
    Mandelbrot(int width, int height, int maxIterations = 80)
        : _width(width), _height(height), _maxIterations(maxIterations), _buffer(width * height, vector3(0, 0, 0))
    {}
    
    void generate() {
        
        _ready = false;
        
        std::vector<std::future<void>> futures;
        
        int divWidth = _width / kNumberOfMandelbrotTasks;
        int divHeight = _height / kNumberOfMandelbrotTasks;
        for (int i=0; i < _width/divWidth; i++) {
            for (int j=0; j < _height/divHeight; j++) {
                
                auto f = std::async([=]() { generateSub(
                                                        i * divWidth,
                                                        i * divWidth + divWidth,
                                                        j * divHeight,
                                                        j * divHeight + divHeight);
                });
                
                futures.push_back(std::move(f));
            }
        }
        
        // Wait for all tasks to finish
        for (const auto& f : futures) {
            f.wait();
        }
        
        _scale *= 1.5;
        
        if (_autoAdjust) {
            adjustMaxIterations();
        }
        
        _ready = true;
    }
    
public: // Getters
    bool isReady() const {
        return _ready;
    }
    
    const vector3* getBuffer() const {
        return _buffer.data();
    }
    
    const int getWidth() const {
        return _width;
    }
    
    const int getHeight() const {
        return _height;
    }
    
    
public: // Setters
    
    void setColor(vector3 rgb) {
        _rgb = rgb;
    }
    
private:
    
    void adjustMaxIterations() {
        
        bool adjust = false;
        
        if (_iterationStep == 0 && _scale > 20.0) {
            adjust = true;
        }
        else if (_iterationStep == 1 && _scale > 100.0) {
            adjust = true;
        }
        else if (_iterationStep == 2 && _scale > 1500.0) {
            adjust = true;
        }
        else if (_iterationStep == 3 && _scale > 5000.0) {
            adjust = true;
        }
        
        if (adjust) {
            _maxIterations *= 2.0;
            ++_iterationStep;
            std::cout << "Adjusted max iterations to " << _maxIterations << std::endl;
        }
    }
    
    void generateSub(int x0, int x1, int y0, int y1) {
        
        constexpr auto map = [](TFloat X, TFloat iMin, TFloat iMax, TFloat oMin, TFloat oMax) {
            return ((X - iMin) / (iMax - iMin)) * (oMax - oMin) + oMin;
        };
        
        for(int y = y0; y < y1; ++y){
            for(int x = x0; x < x1; ++x){
                
                TFloat x0 = _center.x + map(x, 0, _width, -2.0 / _scale, 1.0 / _scale);
                TFloat y0 = _center.y + map(y, 0, _height, -1.0 / _scale, 1.0 / _scale);
                
                std::complex<TFloat> z;
                std::complex<TFloat> c = { x0, y0 };
                
                int iteration = 0;
                while(abs(z) < 2 && ++iteration < _maxIterations) {
                    z = std::pow(z, 2.0f) + c;
                }
                
                if (iteration == _maxIterations) {
                    _buffer[y * _height + x] = vector3(0, 0, 0);
                }
                else {
                    
                    TFloat s = iteration - std::log(std::log(std::abs(z))) + 4.0;
                    
                    auto col = vector3(0, 0, 0);
                    col.r = 0.5 + 0.5 * std::cos( 3.0 + s*0.15 * _rgb.r);
                    col.g = 0.5 + 0.5 * std::cos( 3.0 + s*0.15 * _rgb.g);
                    col.b = 0.5 + 0.5 * std::cos( 3.0 + s*0.15 * _rgb.b);
                    
                    _buffer[y * _height + x] = col;
                }
            }
        }
    }
    
private:
    int _width, _height;
    int _maxIterations;
    
    vector2 _center = kDefaultCenter;
    TFloat _scale = 1.0;
    
    std::vector<vector3> _buffer;
    vector3 _rgb {0, 0, 0};
    
    int _iterationStep = 0;
    
    bool _ready = true;
    bool _autoAdjust = true;
};
