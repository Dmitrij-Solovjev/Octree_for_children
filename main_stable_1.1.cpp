/**
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


#define IMMERSING_LIMIT 8
#define MAX_X 256
#define MAX_Y 256
#define MAX_Z 256
#define NUM 5                              //ограничение на количество мячей
#define MAX_R 10



using namespace std;

int identificator = 0;

struct Octree;
struct Ball;

queue<pair<Ball*, Ball*>> Arr_of_collusion();

struct Ball{
    int R;                                  //радиус

    double x, y, z;                         //координаты
    double Ux, Uy, Uz;
    int id;                                 //можно использовать для генерации цвета
    Octree *cubic_mesh;

    Ball (int _R = 1, double x0 = 1, double y0 = 1, double z0 = 1, double Ux0 = 1, double Uy0 = 1, double Uz0 = 1, int _id = identificator){
        ++identificator;
        R = _R;
        x = x0; y = y0; z = z0;
        Ux = Ux0; Uy = Uy0; Uz = Uz0;
        id = _id;
        cout << "object "<< id <<" created" << endl;
    }

    void display() {
        cout << "<"+string(25, '-')+">"<< endl;
        cout << "id: "<< id << endl;
        cout << "x:  "<< x  << "    y: " << y  << "    z: " << z  <<  endl;
        cout << "Ux: "<< Ux << "   Uy: " << Uy << "   Uz: " << Uz <<  endl;
        cout << "<"+string(25, '-')+">"<< endl;
    }

    void move(){
        x+=Ux;
        y+=Uy;
        z+=Uz;
    }

    void draw(){
        //какая-то магия



    }

    void about_me();
    bool am_I_inside();
    bool am_I_outside();
    void spin_me();
    void reallocate();
    void collusion_detect(Ball *ball);

    ~Ball() {
        cout << "Goodbye, " + to_string(id) + " =(" << endl;
    }
};

/*    2-------3    y
     /|      /|    |
    6-+-----7 |    |
    | 0-----+-1    0---x
    |/      |/    /
    4-------5    z
*/


struct Octree {
    //bool are_you_full_son = false;
    //bool are_you_ready_for_recalculation_son = false;
    bool to_recalculate = false;
    vector <Ball *> List_of_items;
    vector <Ball *> List_of_border;
    vector <Ball *> Finita_la_commedia;
    Octree *next[8];
    Octree *prev;

    int my_immersing;   //iteration
    int my_x0, my_y0, my_z0;
    int my_x1, my_y1, my_z1;
    Ball *buffer = NULL;

    Octree(int _my_x0, int _my_y0, int _my_z0, int _my_x1, int _my_y1, int _my_z1, int _my_immersing, Octree *_prev) {
        my_x0 = _my_x0;
        my_y0 = _my_y0;
        my_z0 = _my_z0;
        my_x1 = _my_x1;
        my_y1 = _my_y1;
        my_z1 = _my_z1;
        my_immersing = _my_immersing;
        prev = _prev;
        for (int i = 0; i < 8; ++i) next[i] = NULL;
    }

    ~Octree(){
        cout <<"Плак-плак меня удалили(" << my_x1-my_x0 << ")"<<endl;
    }

    void add(Ball *ball) {                                                  //Сасно расписал(нет) ಠ_ಠ
        if (my_immersing < IMMERSING_LIMIT and List_of_items.size()) {
            bool z1 = ((ball->z + ball->R) >= (double) ((my_z0 + my_z1) / 2));         //правее/выше/ближе
            bool y1 = ((ball->y + ball->R) >= (double) ((my_y0 + my_y1) / 2));
            bool x1 = ((ball->x + ball->R) >= (double) ((my_x0 + my_x1) / 2));

            bool z2 = ((ball->z - ball->R) <= (double) ((my_z0 + my_z1) / 2));         //левее/ниже/дальше
            bool y2 = ((ball->y - ball->R) <= (double) ((my_y0 + my_y1) / 2));
            bool x2 = ((ball->x - ball->R) <= (double) ((my_x0 + my_x1) / 2));

            if (buffer != nullptr) {
                cout << "перемещаю " + to_string(buffer->id) + " вниз по дереву" << endl;
                Ball *p = buffer;
                buffer = NULL;
                this->add(p);
            }

            if (z1 && z2 || y1 && y2 || x1 && x2) {                         //ПЕРЕСЕЧЕНИЕ
                cout << "Моя молодец - моя положить " << ball->id << " на прямые квадрантов" << endl;
                List_of_items.push_back(ball);                              //положили в список внутренних объектов
                List_of_border.push_back(ball);                             //положили в список объектов на границе
                ball->cubic_mesh = this;
            } else {                                                        //ДЛЯ НЕПУСТОГО ДЕРЕВА
                int next_x0 = my_x0 + (my_x1 - my_x0) / 2 * x1;
                int next_y0 = my_y0 + (my_y1 - my_y0) / 2 * y1;
                int next_z0 = my_z0 + (my_z1 - my_z0) / 2 * z1;

                int next_x1 = my_x1 - (my_x1 - my_x0) / 2 * (1 - x1);
                int next_y1 = my_y1 - (my_y1 - my_y0) / 2 * (1 - y1);
                int next_z1 = my_z1 - (my_z1 - my_z0) / 2 * (1 - z1);

                cout << "Моя молодец - моя положить " + to_string(ball->id) + " в ячейку с координатами: " <<
                     next_x0 << ':' << next_y0 << ':' << next_z0 << "   " <<
                     next_x1 << ':' << next_y1 << ':' << next_z1 << endl;

                if (next[z1 * 4 + y1 * 2 + x1 * 1] == nullptr) {          //Если нужная ячейка не создана то создать
                    next[z1 * 4 + y1 * 2 + x1 * 1] = new Octree(next_x0, next_y0, next_z0,
                                                                next_x1, next_y1, next_z1,
                                                                my_immersing + 1, this);
                }

                List_of_items.push_back(ball);                              //положили в список внутренних объектов
                next[z1 * 4 + y1 * 2 + x1 * 1]->add(ball);
            }
        } else if (!List_of_items.size() and my_immersing < IMMERSING_LIMIT) {      //если нет пересечений и объект пуст
            cout << "Моя молодец - моя положить " << ball->id << " в буфер" << endl;
            buffer = ball;
            List_of_items.push_back(ball);
            ball->cubic_mesh = this;
        } else {
            cout << "Моя молодец - моя положить " << ball->id << " в массив" << endl;
            Finita_la_commedia.push_back(ball);
            List_of_items.push_back(ball);
            ball->cubic_mesh = this;
        }

        check_recalculate();
    }

    void check_recalculate() {
        for (int i = 0; i < 8 && !to_recalculate; ++i)
            if (next[i] != nullptr)
                if (next[i]->to_recalculate) {      //если дочернему нужен пересчет -> предку нужен пересчет
                    to_recalculate = true;
                    cout << "Ячейке с координатами: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                            my_x1 << ':' << my_y1 << ':' << my_z1 << " нужен пересчет коллизий ибо нужно дочернему"
                            << endl;
                        return;
                }
        if (List_of_border.size() and List_of_items.size() > 1) {
            to_recalculate = true;
//            for (int i=0; i<List_of_border.size(); ++i)
//                for (int j=0; j<List_of_items.size(); ++j){
//                    Arr_of_collusion(make_pair(List_of_border[i], List_of_items[j]));
//                }

                    cout << "Ячейке с координатами: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1
                 << " нужен пересчет коллизий ибо есть элементы на пересечении" << endl;
        } else if (Finita_la_commedia.size() > 1) {
            to_recalculate = true;
            cout << "Ячейке с координатами: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1 << " нужен пересчет коллизий ибо конечка, n>=2" << endl;
        } else {
            to_recalculate = false;
            cout << "Ячейке с координатами: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1 << " больше не нужен пересчет" << endl;

        }
    }

    void remove(Ball *ball) {
        if (find(List_of_items.begin(), List_of_items.end(), ball) == List_of_items.end()) {
            cout << "FATAL ERROR! DED IDI PIT' TABLETKI#######################################################" << endl;
            while (true);
            return;
        }


        int pos1 = find(List_of_items.begin(), List_of_items.end(), ball) - List_of_items.begin();
        List_of_items[pos1] = nullptr;
        List_of_items.erase(List_of_items.begin() + pos1);


        if (buffer != nullptr and buffer == ball) {
            cout << "Удаляю из буфера р.:" << my_x1 - my_x0 << endl;
            buffer = nullptr;
        } else if (find(List_of_border.begin(), List_of_border.end(), ball) != List_of_border.end()) {
            cout << "Удаляю из массива пересечений р.:" << my_x1 - my_x0 << endl;
            int pos = find(List_of_border.begin(), List_of_border.end(), ball) - List_of_border.begin();
            List_of_border[pos] = nullptr;
            List_of_border.erase(List_of_border.begin() + pos);

        } else if (find(Finita_la_commedia.begin(), Finita_la_commedia.end(), ball) != Finita_la_commedia.end()) {
            cout << "Удаляю из конечного массива р.:" << my_x1 - my_x0 << endl;
            int pos = find(Finita_la_commedia.begin(), Finita_la_commedia.end(), ball) - Finita_la_commedia.begin();
            Finita_la_commedia[pos] = nullptr;
            Finita_la_commedia.erase(Finita_la_commedia.begin() + pos);
        }
        if (!List_of_items.size()){
            prev->remove_next(this);        //если элементов не осталось -> удалить ветвь
        } else check_recalculate();
    }
    void remove_next(Octree *tree){
        for (int i=0; i<8; ++i){
            if (next[i] != nullptr and next[i] == tree){
                cout << "Удалили ветвь " << tree->my_x1 - tree->my_x0 << endl;
                delete next[i];
                next[i]= nullptr;
                return;
            }
        }
    }
};

bool Ball::am_I_outside() {
    return  ((x+R) < (cubic_mesh->my_x1+cubic_mesh->my_x0)/2 || (x-R) > (cubic_mesh->my_x1+cubic_mesh->my_x0)/2) &&
            ((y+R) < (cubic_mesh->my_y1+cubic_mesh->my_y0)/2 || (y-R) > (cubic_mesh->my_y1+cubic_mesh->my_y0)/2) &&
            ((z+R) < (cubic_mesh->my_z1+cubic_mesh->my_z0)/2 || (z-R) > (cubic_mesh->my_z1+cubic_mesh->my_z0)/2);
}

bool Ball::am_I_inside() {
    return ((cubic_mesh->my_x0 < (x - R)) && (cubic_mesh->my_x1 > (x + R)) &&       //x не выходит за пределы
            (cubic_mesh->my_y0 < (y - R)) && (cubic_mesh->my_y1 > (y + R)) &&       //y не выходит за пределы
            (cubic_mesh->my_z0 < (z - R)) && (cubic_mesh->my_z1 > (z + R)) ||       //z не выходит за пределы
            ((x - R) == 0 ) || ((x + R) == MAX_X) ||                                //исключение в случае 0
            ((y - R) == 0 ) || ((y + R) == MAX_Y) ||
            ((z - R) == 0 ) || ((z + R) == MAX_Z));
}


void Ball::reallocate() {
    if (!am_I_inside()) {
        about_me();
        while(!am_I_inside() and cubic_mesh != cubic_mesh->prev) {
            cubic_mesh->remove(this);
            cubic_mesh = cubic_mesh->prev;
        }
//        while (am_I_outside()){
//            cubic_mesh = cubic_mesh->prev;
//        }
        cout << "Поднялись на " << cubic_mesh->my_x1 - cubic_mesh->my_x0 << endl;
        cubic_mesh->add(this);
        about_me();
    }
    if (am_I_outside()){
        cubic_mesh->remove(this);
        cubic_mesh = cubic_mesh->prev;
        cout << "Спускаюсь ..." << endl;
        cubic_mesh->add(this);
        about_me();
    }
};

void Ball::spin_me() {
    if ((x - R) < 0){
        Ux = abs(Ux);
    } else if ((x + R) > MAX_X){
        Ux = -abs(Ux);
    }

    if ((y - R) < 0){
        Uy = abs(Uy);
    } else if ((y + R) > MAX_X){
        Uy = -abs(Uy);
    }

    if ((z - R) < 0){
        Uz = abs(Uz);
    } else if ((z + R) > MAX_X){
        Uz = -abs(Uz);
    }

    x -= ((x - R) < 0)*2*(x - R) + ((x + R) > MAX_X)*2*(x + R - MAX_X);
    y -= ((y - R) < 0)*2*(y - R) + ((y + R) > MAX_Y)*2*(y + R - MAX_Y);
    z -= ((z - R) < 0)*2*(z - R) + ((z + R) > MAX_Z)*2*(z + R - MAX_Z);
}


void Ball::about_me() {
    cout << "Мяч " << id << " (x: "<<x <<"  y: "<< y  <<"  z: "<< z  <<"  R: "<< R <<"  Ux: "
    <<Ux <<"  Uy: "<< Uy  <<"  Uz: "<< Uz  <<") лежит в ячейке координатами: "
         << cubic_mesh->my_x0 << ':' << cubic_mesh->my_y0 << ':' << cubic_mesh->my_z0 << "  Размерностью: "
         << cubic_mesh->my_x1 - cubic_mesh->my_x0 << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    // (´,,•ω•,,)♡
    Octree Little_space_for_little_princess(0, 0, 0, MAX_X, MAX_Y, MAX_Z, 0, nullptr);
    Little_space_for_little_princess.prev = &Little_space_for_little_princess;
    //♡♡(♡>ω<) ♡♡♡♡

    Ball *my_balls[NUM];
    //грамотное создание мячиков, невыходящих за пределы мира
    for (int i=0; i<NUM; ++i){
        my_balls[i] = new Ball (rand()%MAX_R, rand()%MAX_X, rand()%MAX_Y, rand()%MAX_Z);
        Little_space_for_little_princess.add(my_balls[i]);
        my_balls[i]->about_me();

        my_balls[i]->spin_me(); //пересчитываем выход за пределы мира
        my_balls[i]->about_me();

        my_balls[i]->reallocate();//перемещаем в актуальную ячейку
        my_balls[i]->about_me();
    }
    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;

    //for (int j=0; j<5; ++j) {
    for (int i=2; i< 100; --i){
        for (int i=0; i<NUM; ++i) {
//            my_balls[i]->about_me();

            my_balls[i]->move();    //передвигаем
//            my_balls[i]->about_me();

            my_balls[i]->spin_me(); //пересчитываем выход за пределы мира
//            my_balls[i]->about_me();

            my_balls[i]->reallocate();//перемещаем в актуальную ячейку
//            my_balls[i]->about_me();

            my_balls[i]->draw();        //отрисовка

            cout << endl;
            cout << endl;
        }
        cout << endl;
        cout << endl;
        cout << endl;
        sleep(1); // ждать 5 секунд, время эта функция считает в секундах
//        while (true) ;
    }
}
/**Мяч 0 (x: 15  y: 13  z: 10  R: 8  Ux: 1  Uy: 1  Uz: 1) лежит в ячейке координатами: 0:0:0  16:16:16
 *
 *
 */

/** TO-DO LIST
 * 1. реализовать пересет на наличие столкновений
 * 2. реализовать функцию столкновений
 *
 *
 *
 *
 *  1.запретить создание мячей с окружностью, вылезающей за пределы Yes, but...
 *  2.грамотное объявление пределов для разных параметров мячиков
 *  3.Перемещение ಠ_ಠ
 *  4.при удалении объекта отменять пересчет
*/
