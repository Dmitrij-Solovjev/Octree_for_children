/**
 * сугубо for test
 * v:0.1.1
 * **/
#include <iostream>     //базовый ввод вывод
#include <vector>       //хранение элементов/добавление/удаление
#include <queue>        //очередь пересеченных
#include <algorithm>    //find()
#include <cstdlib>      // для функций rand()
#include <ctime>        // для функции time()
#include <unistd.h>     // sleep() (осторожно! только для linux)
//#include "windows.h"  // для винды;
//Sleep(5000);

/**
 * для отрисовки существует функция draw() внутри struct Ball
 *
 *
 *
 **/


#define IMMERSING_LIMIT 4
#define MAX_X 64
#define MAX_Y 64
#define MAX_Z 64
#define NUM 5                              //ограничение на количество мячей
#define MAX_R 10



using namespace std;

int identificator = 0;

struct Octree;
struct Ball;

queue<pair<Ball*, Ball*>> Arr_of_collusion;

struct Ball{
    int R;                                  //радиус

    double x, y, z;                         //координаты
    double Ux, Uy, Uz;
    int id;                                 //можно использовать для генерации цвета

    Ball (int _R = 1, double x0 = 1, double y0 = 1, double z0 = 1, double Ux0 = 1, double Uy0 = 1, double Uz0 = 1, int _id = identificator){
        ++identificator;
        R = _R;
        x = x0; y = y0; z = z0;
        Ux = Ux0; Uy = Uy0; Uz = Uz0;
        id = _id;
        cout << "object "<< id <<" created" << endl;
    }

    void move(){
        if (x>MAX_X) Ux=-Ux;
        if (y>MAX_Y) Uy=-Uy;
        if (z>MAX_Z) Uz=-Uz;

        x+=Ux;
        y+=Uy;
        z+=Uz;
    }

    void draw(){
        //какая-то магия



    }

//    void about_me();
  //  bool am_I_inside();
//    bool am_I_outside();
  //  void spin_me();
  //  void reallocate();
//    void collusion_detect(Ball *ball);
};



void Ball::about_me() {
    cout << "Мяч " << id << " (x: "<<x <<"  y: "<< y  <<"  z: "<< z  <<"  R: "<< R <<"  Ux: "
    <<Ux <<"  Uy: "<< Uy  <<"  Uz: "<< Uz  <<") лежит в ячейке координатами: "
         << cubic_mesh->my_x0 << ':' << cubic_mesh->my_y0 << ':' << cubic_mesh->my_z0 << "  Размерностью: "
         << cubic_mesh->my_x1 - cubic_mesh->my_x0 << endl;
}

int main() {

    Ball *my_balls[NUM];

    for (int i=0; i<NUM; ++i){
        my_balls[i] = new Ball (rand()%MAX_R, rand()%MAX_X, rand()%MAX_Y, rand()%MAX_Z);

        my_balls[i]->about_me();
    }
    cout << endl;

    for (int i=2; i< 100; ++i){
        for (int i=0; i<NUM; ++i){
            my_balls[i]->move();
    }
}
