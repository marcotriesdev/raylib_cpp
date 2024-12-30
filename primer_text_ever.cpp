#include "raylib.h"
#include <iostream>
#include <string>
#include <sstream>
#include <format>
#include <vector>
#include "raymath.h"
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

int global_counter = 4;
Vector2 initPosition;
Vector2 initPlayerPosition;
float initSize = 10;
float initsize2 = 1;


class Enemy {
public:

    Vector2 location;
    Vector2 initLoc;
    float speed;
    Rectangle enemyRect;
    Color enemyColor;
    float size;

    int enemylevel;

    Enemy(Vector2 initlocation, float speedInit, int level){

        initLoc = initlocation;
        speed = speedInit;
        enemyRect = {initLoc.x,initLoc.y,50,50};
        enemylevel = level;

        enemyLevels(enemylevel);

    }

    void enemyLevels(int enemylevel){

        switch (enemylevel){

            case 1:
                enemyColor = ORANGE;
                size = 50.0f;
                speed = 5;
                cout << "enemy is level 1 and orange"<< endl;
                break;

            case 2:
                enemyColor = LIGHTGRAY;
                size = 80.0f;
                speed = 7;
                cout << "enemy is level 2 and gray"<< endl;
                break;

            case 3:
                enemyColor = DARKGREEN;
                size = 90.0f;
                speed = 10;
                cout << "enemy is level 3 and dark green"<< endl;
                break;

            case 4:
                enemyColor = DARKBLUE;
                size = 95.0f;
                speed = 7;
                cout << "enemy is level 4 and dark blue"<< endl;
                break;

            case 5:
                enemyColor = RED;
                size = 100.0f;
                speed = 8;
                cout << "enemy is level 5 and red" << endl;
                break;



        }

    }

    void moveEnemy(){

        initLoc.y += speed; 

    }

    void drawEnemy(){

        DrawRectangleV(initLoc,{size,size},enemyColor);

    }

    void update(){

        enemyRect = {initLoc.x,initLoc.y,size,size};
        moveEnemy();
        drawEnemy();

    }

};

class EnemyGen {
public:

    std::vector <Enemy> enemies;
    int timer = 50;

    public: void funcTimer(){

        if (timer > 0){

            timer -= 1;

        }

        else if (timer <= 0) {

            generateEnemy();
            timer = 50;

        }

    }

    public: void generateEnemy(){

        random_device rd; 
        mt19937 gen(rd()); 
        uniform_real_distribution<> dis(0.0f, 800.0f); // Rango de 0 a 800 // Generar un número float aleatorio en el rango especificado float numeroRandom =

        float numeroRandom = dis(gen);
        Vector2 newLocation = {numeroRandom,-100.0f};

        std::uniform_int_distribution<> int_dis(1, 5); 
        int random_int = int_dis(gen);
        cout << random_int << endl;
        
        Enemy newEnemy = Enemy(newLocation,5,random_int);
        enemies.push_back(newEnemy);

    } 


    private: void updateEnemies() {

        for (auto& enemy : enemies) {

            enemy.update();
        }

    }

    public: void update(){

        funcTimer();
        updateEnemies();

    }

};


class Player {
public:


    public: int vida;
    public: Vector2 location;
    private: Vector2 movement;
    public: float speed;
    public: float initspeed;
    private: float friction = 0.5f;
    private: Rectangle playerRect = {location.x,location.y,50,50};

    private: int damageTimer = 25;
    private: int initTimer = 25;
    private: bool damaged = false;

    private: int turbinas[5] = {0,1,2,3,4};
    private: int turbinaActiva = turbinas[0];
    private: Color turbinaColor = {255,200,0,255};

    

    private: void dibujarPlayer() {
        DrawRectangleV(location,{50,50},GREEN);
    }

    private: void dibujarTurbina(){

        switch (turbinaActiva)
        {
            case 3: // location: TOP FACE
                DrawEllipse(location.x,location.y,10.0f,15.0f,turbinaColor);
                DrawEllipse(location.x+40,location.y,10.0f,15.0f,turbinaColor);
                cout << "dibujando turbina de arriba"<< endl;
                break;

            case 4: // location: RIGHT FACE
                DrawEllipse(location.x+50,location.y,15.0f,10.0f,turbinaColor);
                DrawEllipse(location.x+50,location.y+50,15.0f,10.0f,turbinaColor);
                break;

            case 1: // location: BOTTOM FACE
                DrawEllipse(location.x,location.y+50,10.0f,15.0f,turbinaColor);
                DrawEllipse(location.x+50,location.y+50,10.0f,15.0f,turbinaColor);
                break;

            case 2: // location: LEFT FACE
                DrawEllipse(location.x,location.y,15.0f,10.0f,turbinaColor);
                DrawEllipse(location.x,location.y+50,15.0f,10.0f,turbinaColor);
                break;

            case 0:
                break;

        }
        


    }

    private: void calculateFric() {

        if (speed > 0){
            speed -= friction;
        }
        else if (speed <= 0){
            speed = 0;
        
        }
    }

    private: void input() {

        turbinaActiva = turbinas[0];

        if (IsKeyDown(KEY_A)){
            movement.x -= 1;
            speed = initspeed;
            turbinaActiva = turbinas[4];
        }
        if (IsKeyDown(KEY_D)){
            movement.x += 1;
            speed = initspeed;
            turbinaActiva = turbinas[2];
        }
        if (IsKeyDown(KEY_W)){
            movement.y -= 1;
            speed = initspeed;
            turbinaActiva = turbinas[1];
        }
        if (IsKeyDown(KEY_S)){
            movement.y += 1;
            speed = initspeed;
            turbinaActiva = turbinas[3];
        }


        calculateFric();
        normalizeLocation();

        location += movement *speed;

    }

    private: void normalizeLocation(){

        movement = Vector2Normalize(movement);
    }

    private: void runTimer (){

        if (damaged && damageTimer > 0){
        
            damageTimer -= 1;

        }
        else if (damaged && damageTimer <=0){

            damaged = false;
            damageTimer = initTimer;

        }
    }

    private: void checkCollision(auto group){

        playerRect = {location.x,location.y,50,50};

        for (Enemy enemy : group.enemies) {
        if (CheckCollisionRecs(playerRect,enemy.enemyRect) && !damaged){

            vida -= enemy.enemylevel;
            damaged = true;

        }
        }
    }

    public: void updatePlayer(auto group) {

        runTimer();
        //cout << damageTimer << endl;
        input();
        checkCollision(group);
        dibujarTurbina();
        dibujarPlayer();

    }

};

void dibujarCirculo(Vector2 location, float size, bool darks){

    if (darks){
        DrawCircleV(location,size,BLACK);
    }
    else{
        DrawCircleV(location,size,WHITE);
    }
    
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
	const string playerName = "puta";
	int playerPunteo = 200;
	

    InitWindow(screenWidth, screenHeight, "Juego con raylib en C++");

    SetTargetFPS(60);

    
    initPosition = {200,200};
    initPlayerPosition = {screenWidth/2,400};

    Player player = Player();
    player.vida = 100;
    player.location = initPlayerPosition;
    player.speed = 5;
    player.initspeed = 5;
    string valor;

    EnemyGen enemygenerator = EnemyGen();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        dibujarCirculo(initPosition,initSize,true);

        if (initSize < 500){
            initSize += 1;
        }
        else {
            initsize2 += 1;
            dibujarCirculo(initPosition,initsize2,false);
        }


        valor = to_string(player.vida);

        enemygenerator.update();
        player.updatePlayer(enemygenerator);
        

		DrawText("LIFE POINTS: ", 190, 220, 30, MAROON);
		DrawText(valor.c_str(), 190, 260, 40, LIME);
		
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
