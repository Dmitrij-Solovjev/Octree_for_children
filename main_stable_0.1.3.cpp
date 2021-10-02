/**
 * v:0.1.3
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


#define IMMERSING_LIMIT 4                   //ограничение глубины дерева
#define MAX_X 64                            //ограничение на координаты
#define MAX_Y 64
#define MAX_Z 64
#define NUM 5                               //ограничение на количество мячей
#define MAX_R 10                            //ограничение на радиус

using namespace std;

int identificator = 0;


struct Octree;
struct Ball;
vector <pair<Ball*, Ball*>> Arr_of_collusion;


//vector <pair<Ball*, Ball*>> Arr_of_collusion;

struct Ball {
    int R;                                    //радиус

    double x, y, z;                             //координаты
    double len_x, len_y, len_z;                 //(x-len_x)

    double Ux, Uy, Uz;
    int id;                                 //можно использовать для генерации цвета
    Octree *cubic_mesh;                     //в какой ветви мы лежим

    Ball (int _R = 1, double x0 = 1, double y0 = 1, double z0 = 1, double Ux0 = 1, double Uy0 = 1, double Uz0 = 1, int _id = identificator){
        ++identificator;
        R = _R;
        x = x0; y = y0; z = z0;
        Ux = Ux0; Uy = Uy0; Uz = Uz0;
        id = _id;
        cout << "object "<< id <<" created" << endl;
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
//    void collusion_detect(Ball *ball);

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
        List_of_items.reserve(0);
        cout << "Octree"<<endl;
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

    void add(Ball *ball) {
        cout << "add"<<endl;
        if (my_immersing < IMMERSING_LIMIT and List_of_items.size()) {//проходит по итерациям и в масссиве есть элементы
            bool z1 = ((ball->z + ball->R) >= (double) ((my_z0 + my_z1) / 2));         //правее/выше/ближе
            bool y1 = ((ball->y + ball->R) >= (double) ((my_y0 + my_y1) / 2));
            bool x1 = ((ball->x + ball->R) >= (double) ((my_x0 + my_x1) / 2));

            bool z2 = ((ball->z - ball->R) <= (double) ((my_z0 + my_z1) / 2));         //левее/ниже/дальше
            bool y2 = ((ball->y - ball->R) <= (double) ((my_y0 + my_y1) / 2));
            bool x2 = ((ball->x - ball->R) <= (double) ((my_x0 + my_x1) / 2));

            if (buffer != nullptr) {                                    //если были в буфере
                cout << "перемещаю " + to_string(buffer->id) + " вниз по дереву" << endl;
                Ball *p = buffer;
                buffer = NULL;
                this->add(p);                                           //положить дочке
            }

            if (z1 && z2 || y1 && y2 || x1 && x2) {                         //ПЕРЕСЕЧЕНИЕ
                cout << "Моя молодец - моя положить " << ball->id << " на прямые квадрантов" << endl;
                bool chekin=false;
//                cout << List_of_items.size() << endl;
                for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                    if (List_of_items[i] == ball) chekin=true;
                }
//                cout << "Ошибка закралась тут" << endl;
                if (not(chekin)) {List_of_items.push_back(ball); cout << "кладем внутрь всего " << ball->id << endl;}                             //положили в список внутренних объектов

                bool chekin2=false;
                for (int i=0; i< List_of_border.size() and not(chekin2); ++i){
                    if (List_of_border[i] == ball) chekin2=true;
                }
                if (not(chekin2)) {List_of_border.push_back(ball); cout << "кладем внутрь пересечения " << ball->id << endl;}                             //положили в список внутренних объектов

//                List_of_border.push_back(ball);                             //положили в список объектов на границе
                ball->cubic_mesh = this;
            } else {                                                        //ДЛЯ НЕПУСТОГО ДЕРЕВА

                bool chekin=false;
                for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                    if (List_of_items[i] == ball) chekin=true;
                }
                if (not(chekin)) {List_of_items.push_back(ball);  cout << "кладем внутрь и в доч. " << ball->id <<endl;}                              //положили в список внутренних объектов



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
                next[z1 * 4 + y1 * 2 + x1 * 1]->add(ball);
            }
        } else if (!List_of_items.size() and my_immersing < IMMERSING_LIMIT) {      //если нет пересечений и объект пуст
            cout << "Моя молодец - моя положить " << ball->id << " в буфер" << endl;
            buffer = ball;
            bool chekin=false;
            for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                if (List_of_items[i] == ball) chekin=true;
            }
            if (not(chekin)) {List_of_items.push_back(ball);  cout << "кладем внутрь буфера " << ball->id << endl;}                              //положили в список внутренних объектов

            //List_of_items.push_back(ball);  cout << "кладем внутрь" << endl;
            ball->cubic_mesh = this;
        } else {
            cout << "Моя молодец - моя положить " << ball->id << " в массив" << endl;
            Finita_la_commedia.push_back(ball);
            bool chekin=false;
            for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                if (List_of_items[i] == ball) chekin=true;
            }
            if (not(chekin)) {List_of_items.push_back(ball);  cout << "кладем внутрь массива " << ball->id << endl;}                              //положили в список внутренних объектов

//            List_of_items.push_back(ball);  cout << "кладем внутрь" << endl;
            ball->cubic_mesh = this;
        }

        //check_recalculate();
    }

    void check_recalculate() {
        cout << "check_recalculate"<<endl;
        for (int i = 0; i < 8; ++i)
            if (next[i] != nullptr) {
                cout << "Пересчет нужен дочернему от: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                     my_x1 << ':' << my_y1 << ':' << my_z1 << endl;
                next[i]->check_recalculate();
            }
        if (List_of_border.size() and List_of_items.size() > 1) {
            cout << "Пересчитываю: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1 << endl;
            cout << "List_of_border: ";
            for (int i = 0; i < List_of_border.size(); ++i) cout << List_of_border[i]->id << ' ';
            cout << endl;
            cout << "List_of_items: ";
            for (int i = 0; i < List_of_items.size(); ++i) cout << List_of_items[i]->id << ' ';
            cout << endl;

            for (int i = 0; i < List_of_border.size(); ++i)
                for (int j = 0; j < List_of_items.size(); ++j) {
                    bool check_again = false;
                    if (List_of_items[j] == List_of_border[i]) continue;
                    for (int ii = 0; ii < i and not(check_again); ++ii) {
                        if (List_of_items[j] == List_of_border[ii]) check_again = true;     // a->b, b->a
                    }
                    if (check_again) continue;
                    Arr_of_collusion.push_back(make_pair(List_of_border[i], List_of_items[j]));

                }

            cout << "Добавление из общих ячеек: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1 << endl;
        } else if (Finita_la_commedia.size() > 1) {
            cout << "Пересчитываю массив: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1 << endl;

            for (int i=0; i<List_of_border.size(); ++i)
                for (int j=0; j<List_of_items.size(); ++j) {
                    Arr_of_collusion.push_back(make_pair(List_of_border[i], List_of_items[j]));
                }
            cout << "Добавление из массива: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1 << endl;
        } else {
            cout << "Пересчет не нужен: " << my_x0 << ':' << my_y0 << ':' << my_z0 << "   " <<
                 my_x1 << ':' << my_y1 << ':' << my_z1  << endl;

        }
        for (int i=0; i<Arr_of_collusion.size(); ++i)
            cout << Arr_of_collusion[i].first->id << "-" <<Arr_of_collusion[i].second->id<< " ";
        cout << endl;
    }

    void remove(Ball *ball) {
        cout << "remove"<<endl;
        if (find(List_of_items.begin(), List_of_items.end(), ball) == List_of_items.end()) {
            cout << "FATAL ERROR! DED IDI PIT' TABLETKI#######################################################" << endl;
            while (true);
        }
        cout << "EEEEEEEEEEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRRAAAAAASSSSSSSSSSSSSSSSEEEEE" << endl;
        cout << ball->id <<endl;
        for (int i=0; i<List_of_items.size(); ++i){
            cout <<List_of_items[i]->id << ' ';
        }
        cout << endl;

        int pos1 = find(List_of_items.begin(), List_of_items.end(), ball) - List_of_items.begin();
        List_of_items[pos1] = nullptr;
        List_of_items.erase(List_of_items.begin() + pos1);

        for (int i=0; i<List_of_items.size(); ++i){
            cout <<List_of_items[i]->id << ' ';
        }
        cout << endl;


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
        } //else check_recalculate();
    }

    void remove_next(Octree *tree){
        cout << "remove_next"<<endl;
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
    cout << "am_I_outside"<<endl;
    return  ((x+R) < (cubic_mesh->my_x1+cubic_mesh->my_x0)/2 || (x-R) > (cubic_mesh->my_x1+cubic_mesh->my_x0)/2) &&
            ((y+R) < (cubic_mesh->my_y1+cubic_mesh->my_y0)/2 || (y-R) > (cubic_mesh->my_y1+cubic_mesh->my_y0)/2) &&
            ((z+R) < (cubic_mesh->my_z1+cubic_mesh->my_z0)/2 || (z-R) > (cubic_mesh->my_z1+cubic_mesh->my_z0)/2);
}

bool Ball::am_I_inside() {
    cout << "am_I_inside"<<endl;
    return ((cubic_mesh->my_x0 < (x - R)) && (cubic_mesh->my_x1 > (x + R)) &&       //x не выходит за пределы
            (cubic_mesh->my_y0 < (y - R)) && (cubic_mesh->my_y1 > (y + R)) &&       //y не выходит за пределы
            (cubic_mesh->my_z0 < (z - R)) && (cubic_mesh->my_z1 > (z + R)) ||       //z не выходит за пределы
            ((x - R) == 0 ) || ((x + R) == MAX_X) ||                                //исключение в случае 0
            ((y - R) == 0 ) || ((y + R) == MAX_Y) ||
            ((z - R) == 0 ) || ((z + R) == MAX_Z));
}


void Ball::reallocate() {
    cout << "reallocate"<<endl;

    if (!am_I_inside()) {
        about_me();
        while(!am_I_inside() and cubic_mesh != cubic_mesh->prev) {
            Octree *prev = cubic_mesh->prev;
            cubic_mesh->remove(this);
            cubic_mesh = prev;
        }

        cout << "Поднялись на " << cubic_mesh->my_x1 - cubic_mesh->my_x0 << endl;
        cubic_mesh->add(this);

        about_me();
    }
    if (am_I_outside() and ((cubic_mesh->buffer != nullptr and cubic_mesh->buffer != this) or   //если мы лежим в буфере
                            (cubic_mesh->buffer == nullptr))){        //то перемещаться итеративно вниз - не лучшая идея
        cout << "Спускаюсь ..." << endl;
        //Octree *tree_p = cubic_mesh->prev;
        //cubic_mesh->remove(this);

        if (find(cubic_mesh->List_of_border.begin(), cubic_mesh->List_of_border.end(), this) != cubic_mesh->List_of_border.end()) {
            cout << "Удаляю из массива пересечений р.:" << cubic_mesh->my_x1 - cubic_mesh->my_x0 << " т.к. мы кладем в дочернюю"<< endl;
            int pos = find(cubic_mesh->List_of_border.begin(), cubic_mesh->List_of_border.end(), this) - cubic_mesh->List_of_border.begin();
            cubic_mesh->List_of_border[pos] = nullptr;
            cubic_mesh->List_of_border.erase(cubic_mesh->List_of_border.begin() + pos);
        }

        cubic_mesh->add(this);

        about_me();
    }
};

void Ball::spin_me() {
    cout << "spin_me"<<endl;
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
    cout << "about_me"<<endl;
    cout << "Мяч " << id << " (x: "<<x <<"  y: "<< y  <<"  z: "<< z  <<"  R: "<< R <<"  Ux: "
    <<Ux <<"  Uy: "<< Uy  <<"  Uz: "<< Uz  <<") лежит в ячейке координатами: "
         << cubic_mesh->my_x0 << ':' << cubic_mesh->my_y0 << ':' << cubic_mesh->my_z0 << "  Размерностью: "
         << cubic_mesh->my_x1 - cubic_mesh->my_x0 << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Octree Tree(0, 0, 0, MAX_X, MAX_Y, MAX_Z, 0, nullptr);
    Tree.prev = &Tree;
    Ball *my_balls[NUM];

    int ARR[] = {8, 39, 48, 53,
                 8, 42, 44, 23,
                 4, 32, 38, 50,
                 7, 34, 16, 56,
                 1, 35, 34, 52};
/*
    my_balls[0] = new Ball (8, 39, 48, 53);
    my_balls[1] = new Ball (8, 42, 44, 23);
    my_balls[2] = new Ball (4, 32, 38, 50);
    my_balls[3] = new Ball (7, 34, 16, 56);
    my_balls[4] = new Ball (1, 35, 34, 52);
*/



    //грамотное создание мячиков, невыходящих за пределы мира
    for (int i=0; i<NUM; ++i){
//        my_balls[i] = new Ball (rand()%MAX_R, rand()%MAX_X, rand()%MAX_Y, rand()%MAX_Z);
        my_balls[i] = new Ball (ARR[i*4], ARR[i*4+1], ARR[i*4+2], ARR[i*4+3], 0, 0, 0);
        Tree.add(my_balls[i]);
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
    for (int i=2; i< 3; ++i) {
        for (int i=0; i<NUM; ++i) {
            my_balls[i]->about_me();

            my_balls[i]->move();    //передвигаем
            my_balls[i]->about_me();

            my_balls[i]->spin_me(); //пересчитываем выход за пределы мира
            my_balls[i]->about_me();

            my_balls[i]->reallocate();//перемещаем в актуальную ячейку
            my_balls[i]->about_me();

            cout << "####################################################################################################################################################################################"<< endl;
            cout << "####################################################################################################################################################################################"<< endl;
        }
        Tree.check_recalculate();

        while(!Arr_of_collusion.empty()){
            pair <Ball*, Ball *> my_pair; my_pair=Arr_of_collusion.front();
            Arr_of_collusion.erase(Arr_of_collusion.begin());
            cout << my_pair.first->id << " должен плюхнуть " << my_pair.second->id << endl;
        }

        cout << endl;
        cout << endl;
        cout << endl;
        sleep(1); // ждать 5 секунд, время эта функция считает в секундах
    }
}
/**Мяч 0 (x: 15  y: 13  z: 10  R: 8  Ux: 1  Uy: 1  Uz: 1) лежит в ячейке координатами: 0:0:0  16:16:16
 *
 *
 */

/** TO-DO LIST
 * 1.реализовать добавление пар для просчета в Octree
 *
 *
 *
 *
 *
 *
 *
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
