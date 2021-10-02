/**
 * v:0.1.4
 * **/
#include <iostream>     //базовый ввод вывод
#include <vector>       //хранение элементов/добавление/удаление
#include <algorithm>    //find()
#include <cstdlib>      // для функций rand()
#include <ctime>        // для функции time()
#include <unistd.h>     // sleep()

/**
 * для отрисовки существует функция draw() внутри struct Actor
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
struct Actor;
vector <pair<Actor*, Actor*>> Arr_of_collusion;


//vector <pair<Actor*, Actor*>> Arr_of_collusion;

struct Actor {
    //int R;                                    //радиус

    double x, y, z;                             //координаты
    double len_x, len_y, len_z;                 //(x-len_x)

    double Ux, Uy, Uz;
    int id;                                 //можно использовать для генерации цвета
    Octree *cubic_mesh;                     //в какой ветви мы лежим

    Actor (int _R = 1, double x0 = 1, double y0 = 1, double z0 = 1, double Ux0 = 1, double Uy0 = 1, double Uz0 = 1, int _id = identificator){
        ++identificator;
        len_x = _R;
        len_y = _R;
        len_z = _R;
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
//    void collusion_detect(Actor *actor);

    ~Actor() {
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
    vector <Actor *> List_of_items;
    vector <Actor *> List_of_border;
    vector <Actor *> Finita_la_commedia;
    Octree *next[8];
    Octree *prev;

    int my_immersing;   //iteration
    int my_x0, my_y0, my_z0;
    int my_x1, my_y1, my_z1;
    Actor *buffer = NULL;

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

    void add(Actor *actor) {
        cout << "add"<<endl;
        if (my_immersing < IMMERSING_LIMIT and List_of_items.size()) {//проходит по итерациям и в масссиве есть элементы
            bool z1 = ((actor->z + actor->len_z) >= (double) ((my_z0 + my_z1) / 2));         //правее/выше/ближе
            bool y1 = ((actor->y + actor->len_y) >= (double) ((my_y0 + my_y1) / 2));
            bool x1 = ((actor->x + actor->len_x) >= (double) ((my_x0 + my_x1) / 2));

            bool z2 = ((actor->z - actor->len_z) <= (double) ((my_z0 + my_z1) / 2));         //левее/ниже/дальше
            bool y2 = ((actor->y - actor->len_y) <= (double) ((my_y0 + my_y1) / 2));
            bool x2 = ((actor->x - actor->len_x) <= (double) ((my_x0 + my_x1) / 2));

            if (buffer != nullptr) {                                    //если были в буфере
                cout << "перемещаю " + to_string(buffer->id) + " вниз по дереву" << endl;
                Actor *p = buffer;
                buffer = NULL;
                this->add(p);                                           //положить дочке
            }

            if (z1 && z2 || y1 && y2 || x1 && x2) {                         //ПЕРЕСЕЧЕНИЕ
                cout << "Моя молодец - моя положить " << actor->id << " на прямые квадрантов" << endl;
                bool chekin=false;
//                cout << List_of_items.size() << endl;
                for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                    if (List_of_items[i] == actor) chekin=true;
                }
//                cout << "Ошибка закралась тут" << endl;
                if (not(chekin)) {List_of_items.push_back(actor); cout << "кладем внутрь всего " << actor->id << endl;}                             //положили в список внутренних объектов

                bool chekin2=false;
                for (int i=0; i< List_of_border.size() and not(chekin2); ++i){
                    if (List_of_border[i] == actor) chekin2=true;
                }
                if (not(chekin2)) {List_of_border.push_back(actor); cout << "кладем внутрь пересечения " << actor->id << endl;}                             //положили в список внутренних объектов

//                List_of_border.push_back(actor);                             //положили в список объектов на границе
                actor->cubic_mesh = this;
            } else {                                                        //ДЛЯ НЕПУСТОГО ДЕРЕВА

                bool chekin=false;
                for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                    if (List_of_items[i] == actor) chekin=true;
                }
                if (not(chekin)) {List_of_items.push_back(actor);  cout << "кладем внутрь и в доч. " << actor->id <<endl;}                              //положили в список внутренних объектов



                int next_x0 = my_x0 + (my_x1 - my_x0) / 2 * x1;
                int next_y0 = my_y0 + (my_y1 - my_y0) / 2 * y1;
                int next_z0 = my_z0 + (my_z1 - my_z0) / 2 * z1;

                int next_x1 = my_x1 - (my_x1 - my_x0) / 2 * (1 - x1);
                int next_y1 = my_y1 - (my_y1 - my_y0) / 2 * (1 - y1);
                int next_z1 = my_z1 - (my_z1 - my_z0) / 2 * (1 - z1);

                cout << "Моя молодец - моя положить " + to_string(actor->id) + " в ячейку с координатами: " <<
                     next_x0 << ':' << next_y0 << ':' << next_z0 << "   " <<
                     next_x1 << ':' << next_y1 << ':' << next_z1 << endl;

                if (next[z1 * 4 + y1 * 2 + x1 * 1] == nullptr) {          //Если нужная ячейка не создана то создать
                    next[z1 * 4 + y1 * 2 + x1 * 1] = new Octree(next_x0, next_y0, next_z0,
                                                                next_x1, next_y1, next_z1,
                                                                my_immersing + 1, this);
                }
                next[z1 * 4 + y1 * 2 + x1 * 1]->add(actor);
            }
        } else if (!List_of_items.size() and my_immersing < IMMERSING_LIMIT) {      //если нет пересечений и объект пуст
            cout << "Моя молодец - моя положить " << actor->id << " в буфер" << endl;
            buffer = actor;
            bool chekin=false;
            for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                if (List_of_items[i] == actor) chekin=true;
            }
            if (not(chekin)) {List_of_items.push_back(actor);  cout << "кладем внутрь буфера " << actor->id << endl;}                              //положили в список внутренних объектов

            //List_of_items.push_back(actor);  cout << "кладем внутрь" << endl;
            actor->cubic_mesh = this;
        } else {
            cout << "Моя молодец - моя положить " << actor->id << " в массив" << endl;
            Finita_la_commedia.push_back(actor);
            bool chekin=false;
            for (int i=0; i< List_of_items.size() and not(chekin); ++i){
                if (List_of_items[i] == actor) chekin=true;
            }
            if (not(chekin)) {List_of_items.push_back(actor);  cout << "кладем внутрь массива " << actor->id << endl;}                              //положили в список внутренних объектов

//            List_of_items.push_back(actor);  cout << "кладем внутрь" << endl;
            actor->cubic_mesh = this;
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

    void remove(Actor *actor) {
        cout << "remove"<<endl;
        if (find(List_of_items.begin(), List_of_items.end(), actor) == List_of_items.end()) {
            cout << "FATAL ERROR! DED IDI PIT' TABLETKI#######################################################" << endl;
            while (true);
        }
        cout << "EEEEEEEEEEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRRAAAAAASSSSSSSSSSSSSSSSEEEEE" << endl;
        cout << actor->id <<endl;
        for (int i=0; i<List_of_items.size(); ++i){
            cout <<List_of_items[i]->id << ' ';
        }
        cout << endl;

        int pos1 = find(List_of_items.begin(), List_of_items.end(), actor) - List_of_items.begin();
        List_of_items[pos1] = nullptr;
        List_of_items.erase(List_of_items.begin() + pos1);

        for (int i=0; i<List_of_items.size(); ++i){
            cout <<List_of_items[i]->id << ' ';
        }
        cout << endl;


        if (buffer != nullptr and buffer == actor) {
            cout << "Удаляю из буфера р.:" << my_x1 - my_x0 << endl;
            buffer = nullptr;
        } else if (find(List_of_border.begin(), List_of_border.end(), actor) != List_of_border.end()) {
            cout << "Удаляю из массива пересечений р.:" << my_x1 - my_x0 << endl;
            int pos = find(List_of_border.begin(), List_of_border.end(), actor) - List_of_border.begin();
            List_of_border[pos] = nullptr;
            List_of_border.erase(List_of_border.begin() + pos);

        } else if (find(Finita_la_commedia.begin(), Finita_la_commedia.end(), actor) != Finita_la_commedia.end()) {
            cout << "Удаляю из конечного массива р.:" << my_x1 - my_x0 << endl;
            int pos = find(Finita_la_commedia.begin(), Finita_la_commedia.end(), actor) - Finita_la_commedia.begin();
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

bool Actor::am_I_outside() {
    cout << "am_I_outside"<<endl;
    return  ((x+len_x) < (cubic_mesh->my_x1+cubic_mesh->my_x0)/2 || (x-len_x) > (cubic_mesh->my_x1+cubic_mesh->my_x0)/2) &&
            ((y+len_y) < (cubic_mesh->my_y1+cubic_mesh->my_y0)/2 || (y-len_y) > (cubic_mesh->my_y1+cubic_mesh->my_y0)/2) &&
            ((z+len_z) < (cubic_mesh->my_z1+cubic_mesh->my_z0)/2 || (z-len_z) > (cubic_mesh->my_z1+cubic_mesh->my_z0)/2);
}

bool Actor::am_I_inside() {
//    cout << "am_I_inside"<<endl;
    return ((cubic_mesh->my_x0 < (x - len_x)) && (cubic_mesh->my_x1 > (x + len_x)) &&       //x не выходит за пределы
            (cubic_mesh->my_y0 < (y - len_y)) && (cubic_mesh->my_y1 > (y + len_y)) &&       //y не выходит за пределы
            (cubic_mesh->my_z0 < (z - len_z)) && (cubic_mesh->my_z1 > (z + len_z)) ||       //z не выходит за пределы
            ((x - len_x) == 0 ) || ((x + len_x) == MAX_X) ||                                //исключение в случае 0
            ((y - len_y) == 0 ) || ((y + len_y) == MAX_Y) ||
            ((z - len_z) == 0 ) || ((z + len_z) == MAX_Z));
}


void Actor::reallocate() {
//    cout << "reallocate"<<endl;

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

void Actor::spin_me() {
    cout << "spin_me"<<endl;
    if ((x - len_x) < 0){
        Ux = abs(Ux);
    } else if ((x + len_x) > MAX_X){
        Ux = -abs(Ux);
    }

    if ((y - len_y) < 0){
        Uy = abs(Uy);
    } else if ((y + len_y) > MAX_X){
        Uy = -abs(Uy);
    }

    if ((z - len_z) < 0){
        Uz = abs(Uz);
    } else if ((z + len_z) > MAX_X){
        Uz = -abs(Uz);
    }
    x -= ((x - len_x) < 0)*2*(x - len_x) + ((x + len_x) > MAX_X)*2*(x + len_x - MAX_X);
    y -= ((y - len_y) < 0)*2*(y - len_y) + ((y + len_y) > MAX_Y)*2*(y + len_y - MAX_Y);
    z -= ((z - len_z) < 0)*2*(z - len_z) + ((z + len_z) > MAX_Z)*2*(z + len_z - MAX_Z);
}


void Actor::about_me() {
    cout << "about_me"<<endl;
    cout << "Мяч " << id << " (x: "<<x <<"  y: "<< y  <<"  z: "<< z  <<"  R: "<< len_z <<"  Ux: "
    <<Ux <<"  Uy: "<< Uy  <<"  Uz: "<< Uz  <<") лежит в ячейке координатами: "
         << cubic_mesh->my_x0 << ':' << cubic_mesh->my_y0 << ':' << cubic_mesh->my_z0 << "  Размерностью: "
         << cubic_mesh->my_x1 - cubic_mesh->my_x0 << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    Octree Tree(0, 0, 0, MAX_X, MAX_Y, MAX_Z, 0, nullptr);
    Tree.prev = &Tree;
    Actor *my_actors[NUM];

    int ARR[] = {8, 39, 48, 53,
                 8, 42, 44, 23,
                 4, 32, 38, 50,
                 7, 34, 16, 56,
                 1, 35, 34, 52};
/*
    int ARR[] = {1, 2, 2, 2,
                 1, 34, 34, 34,
                 1, 50, 50, 50,
                 1, 14, 14, 14,
                 1, 35, 2, 2};
    */

/*
    my_actors[0] = new Actor (8, 39, 48, 53);
    my_actors[1] = new Actor (8, 42, 44, 23);
    my_actors[2] = new Actor (4, 32, 38, 50);
    my_actors[3] = new Actor (7, 34, 16, 56);
    my_actors[4] = new Actor (1, 35, 34, 52);
*/



    //грамотное создание мячиков, невыходящих за пределы мира
    for (int i=0; i<NUM; ++i){
//        my_actors[i] = new Actor (rand()%MAX_R, rand()%MAX_X, rand()%MAX_Y, rand()%MAX_Z);
        my_actors[i] = new Actor (ARR[i*4], ARR[i*4+1], ARR[i*4+2], ARR[i*4+3], 0, 0, 0);
        Tree.add(my_actors[i]);
        my_actors[i]->about_me();

        my_actors[i]->spin_me(); //пересчитываем выход за пределы мира
        my_actors[i]->about_me();

        my_actors[i]->reallocate();//перемещаем в актуальную ячейку
        my_actors[i]->about_me();
    }
    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;

    //for (int j=0; j<5; ++j) {
    for (int i=2; i< 3; ++i) {
        for (int i=0; i<NUM; ++i) {
            my_actors[i]->about_me();

            my_actors[i]->move();    //передвигаем
            my_actors[i]->about_me();

            my_actors[i]->spin_me(); //пересчитываем выход за пределы мира
            my_actors[i]->about_me();

            my_actors[i]->reallocate();//перемещаем в актуальную ячейку
            my_actors[i]->about_me();

            cout << "####################################################################################################################################################################################"<< endl;
            cout << "####################################################################################################################################################################################"<< endl;
        }
        Tree.check_recalculate();

        while(!Arr_of_collusion.empty()){
            pair <Actor*, Actor *> my_pair; my_pair=Arr_of_collusion.front();
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
