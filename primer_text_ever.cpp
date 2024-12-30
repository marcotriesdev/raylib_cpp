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

    Enemy(Vector2 initlocation, float speedInit){

        initLoc = initlocation;
        speed = speedInit;
        enemyRect = {initLoc.x,initLoc.y,50,50};

    }

    void moveEnemy(){

        initLoc.y += speed; 

    }

    void drawEnemy(){

        DrawRectangleV(initLoc,{50,50},RED);

    }

    void update(){

        enemyRect = {initLoc.x,initLoc.y,50,50};
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
        Vector2 newLocation = {numeroRandom,-50.0f};
        
        Enemy newEnemy = Enemy(newLocation,5);
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

    

    private: void dibujarPlayer() {
        DrawRectangleV(location,{50,50},GREEN);
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

        if (IsKeyDown(KEY_A)){
            movement.x -= 1;
            speed = initspeed;
        }
        if (IsKeyDown(KEY_D)){
            movement.x += 1;
            speed = initspeed;
        }
        if (IsKeyDown(KEY_W)){
            movement.y -= 1;
            speed = initspeed;
        }
        if (IsKeyDown(KEY_S)){
            movement.y += 1;
            speed = initspeed;
        }

        calculateFric();
        normalizeLocation();

        location += movement *speed;

    }

    private: void normalizeLocation(){

        movement = Vector2Normalize(movement);
    }


    private: void checkCollision(auto group){

        playerRect = {location.x,location.y,50,50};

        for (Enemy enemy : group.enemies) {
        if (CheckCollisionRecs(playerRect,enemy.enemyRect)){

            vida -= 1;
        }
        }
    }

    public: void updatePlayer(auto group) {

        input();
        checkCollision(group);
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
        DrawText("Hola, raylib en C++!", 190, 200, 20, MAROON);

        global_counter += 1;

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

		DrawText("Eres ", 190, 220, 20, MAROON);
		DrawText(valor.c_str(), 190, 240, 20, MAROON);
		
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
