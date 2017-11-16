//
//  main.cpp
//  shape_new
//
//  Created by 贾云龙 on 11/15/17.
//  Copyright © 2017 贾云龙. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;

class Buffer{
private:
    char* p;
    static int count;
public:
    char buffer[5192000];
    Buffer(){p = buffer; }
    void writehead(char h[]){
        int a = 0;
        for(int i = 0; h[i] != '\0'; i++){
            a++;
            *p = h[i];
            p++;
        }
        for(; a < 80; a++ ){
            buffer[a] = 0;
        }
        p = buffer + 80;
        count = 80;
    }
    void writeNum(uint32_t num){
        *p++ = num & 0xff;
        *p++ = (num >> 8) & 0xff;
        *p++ = (num >> 16) & 0xff;
        *p++ = (num >> 24) & 0xff;
        count += sizeof(uint32_t);
    }
    void writePoint(float x, float y, float z){
        float* p1 = (float*) p;
        *p1 = x; p1++;
        *p1 = y; p1++;
        *p1 = z; p1++;
        p += 3*sizeof(float);
        count += 3*sizeof(float);
    }
    void writeart(){
        *p++ = 0;
        *p++ = 0;
        count += sizeof(uint16_t);
    }
    int size(){
        return count;
    }
};

int Buffer::count = 0;

class Shape{
public:
    float x, y, z;
    Shape(float x, float y, float z):x(x), y(y), z(z){}
    virtual vector<float> getpoint() =0;
};

class Cube : public Shape{
private:
    float size;
public:
    Cube(float x, float y, float z, float size):Shape(x, y ,z), size(size){}
    vector<float> getpoint(){
        vector<float> p;
        //float x = 0, y = 0, z = 0, size = 10;
        for(float i =x; i< size+1+x; i += size){
            for(float j =y; j < size+1+y; j += size){
                for(float m =z; m < size+1+z; m += size){
                    p.push_back(i);
                    p.push_back(j);
                    p.push_back(m);
                }
            }
        }
        return p;
    }
};

class Cylinder : public Shape{
private:
    float r, h;
    int facets;
public:
    Cylinder(float x, float y, float z, float r, float h, int facets):Shape(x,y,z), r(r), h(h), facets(facets){}
    vector<float> getpoint(){
        vector<float> p;
        const double Pi = 3.1415926;
        double thea = (Pi*2) / facets;
        //float r = 3, h =10, x = 0, y = 0, z = 0;
        for(int j = z; j < z + h +1; j+=h){
            for(int i = 0; i < facets; i++){
                double n = r*cos(thea*i) + x;
                double m = r*sin(thea*i) + y;
                p.push_back(n);
                p.push_back(m);
                p.push_back(j);
            }
        }
        return p;
    }
};

class CAD {
private:
    vector<Shape*> shapes;
public:
    CAD(){}
    ~CAD(){
        for(int i = 0; i < shapes.size(); i++)
            delete shapes[i];
    }
    void add(Shape* p){
        shapes.push_back(p);
    }
    void write(string filename){
        ofstream stl;
        stl.open(filename, std::ofstream::binary);
        Buffer a;
        a.writehead("Shape3D");
        a.writeNum(20*20*20+8*8*8);
        for(int i = 0; i < shapes.size(); i++){
            vector<float> p = (shapes[i] -> getpoint());
            for(int i =0; i < p.size(); i+= 3){
                for(int j = 0; j < p.size(); j+= 3){
                    for(int n = 0;n < p.size(); n += 3){
                        a.writePoint(0.0, 0.0, 1.0);
                        a.writePoint(p[i], p[i+1], p[i+2]);
                        a.writePoint(p[j], p[j+1], p[j+2]);
                        a.writePoint(p[n], p[n+1], p[n+2]);
                        a.writeart();
                    }
                }
            }
        }
        stl.write(a.buffer, a.size());
        stl.close();
        
    }
};



int main(){
    CAD c;
    c.add(new Cube(0,0,0,   5));
    c.add(new Cylinder(10,0,0,    3, 10, 10));
    c.write("/Users/joe/Desktop/shape.stl");
}
