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

int main();

class FxAdmin; 

const int screenWidth = 800;
const int screenHeight = 600;
Vector2 initPlayerPosition;

bool paused = false;
bool dSystem = false;
bool dead = false;

//MARK: BACKGROUND

class DarkVoid {
public:

    int size = 1;
    int growValue = 8;
    bool black = true;

    bool growVoid() {

        if (size < 800){

            if (black){
                DrawCircle(200,200,size,BLACK);
            }
            else {
                DrawCircle(200,200,size,WHITE);
            }

            size += growValue;
        }
        else {
            size = 1;
            black = !black;
        }
        return black;
    }


};


//MARK: EXPLOSION


class Explosion {

public:

    Vector2 explosionLocation;
    float explosionSize;
    int growspeed;
    bool ended;

    Explosion(Vector2 location,float size){

        explosionLocation = location;
        explosionSize = size;
        growspeed = 50;
        ended = false;

        initCout();

    }

    void initCout(){

        cout << "expllosion initialized" << endl;
    }

    void grow(){

        if (explosionSize < 1000){
            explosionSize += growspeed;
        }
        else {

            ended = true;
            cout << "explosion ended";
        }

    }

    void draw() {

        DrawCircle(explosionLocation.x,explosionLocation.y,explosionSize,YELLOW);

    }

    void update() {

        if (!ended){
        grow();
        draw();
        }
    }


};


class FxAdmin {
public:
    std::vector <Explosion> explosions;


    void addExplosion(Vector2 location, int size){

        Explosion newexplosion = Explosion(location,size);
        explosions.push_back(newexplosion);
        cout << "numero de explosiones: " << explosions.size() << endl;

    }

    void updateExplosions(){
        //cout << "probando probando fxadmin" << endl;
        //cout << "numero de explosiones: " << explosions.size() << endl;
        for (auto& explosion : explosions){

            explosion.update();
            
            

        }

    } 


};

//CREAR SINGLETON DE FXADMIN MEJOR
FxAdmin fxadminmain = FxAdmin();


//MARK: ENEMY

class Enemy {
public:

    Vector2 location;
    Vector2 initLoc;
    float speed;
    Rectangle enemyRect;
    Color enemyColor;
    string colorName;
    float size;

    bool active;
    int enemylevel;
    FxAdmin fxadmin;
    

    Enemy(Vector2 initlocation, float speedInit, int level){

        initLoc = initlocation;
        speed = speedInit;
        enemyRect = {initLoc.x,initLoc.y,50,50};
        enemylevel = level;
        active = true;
   
        enemyLevels(enemylevel);

    }

    void enemyLevels(int enemylevel){

        switch (enemylevel){

            case 1:
                enemyColor = ORANGE;
                colorName = "Orange";
                size = 50.0f;
                speed = 5;
                break;

            case 2:
                enemyColor = LIGHTGRAY;
                colorName = "Lightgray";
                size = 80.0f;
                speed = 7;
                break;

            case 3:
                enemyColor = DARKGREEN;
                colorName = "Darkgreen";
                size = 90.0f;
                speed = 10;
                break;

            case 4:
                enemyColor = DARKBLUE;
                colorName = "Darkblue";
                size = 95.0f;
                speed = 7;
                break;

            case 5:
                enemyColor = RED;
                colorName = "Red";
                size = 100.0f;
                speed = 8;
                break;

        }

    }

    void explode(){

        active = false;
        cout << "created explosion BOOM" << endl;
        fxadminmain.addExplosion(initLoc, size);
        

    }

    void moveEnemy(){

        if (!paused){
        initLoc.y += speed; 
        }

    }

    void drawEnemy(){

        if (active){
        DrawRectangleV(initLoc,{size,size},enemyColor);
        }
    }

    void check_outside(){

        if (initLoc.y > screenHeight + size && active){

            active = false;

        }

    }


    void debug(){

        if (active){
        DrawText("active", initLoc.x,initLoc.y,20,BLACK);
        }
        else{
        DrawText("inactive",initLoc.x,initLoc.y,20,BLACK);
        }

    }

    void update(){

        enemyRect = {initLoc.x,initLoc.y,size,size};
        moveEnemy();
        drawEnemy();
        check_outside();

        if (dSystem) {
        debug();
        }

    }

};


//MARK: ENEMYGEN

class EnemyGen {
public:

    std::vector <Enemy> enemies;
    int timer = 50;

    EnemyGen();

    void funcTimer(){

        if (timer > 0){

            timer -= 1;

        }

        else if (timer <= 0) {

            generateEnemy();
            timer = 50;

        }

    }

    void generateEnemy(){

        random_device rd; 
        mt19937 gen(rd()); 
        uniform_real_distribution<> dis(0.0f, 800.0f); // Rango de 0 a 800 // Generar un número float aleatorio en el rango especificado float numeroRandom =

        float numeroRandom = dis(gen);
        Vector2 newLocation = {numeroRandom,-100.0f};

        std::uniform_int_distribution<> int_dis(1, 5); 
        int random_int = int_dis(gen);
        
        Enemy newEnemy = Enemy(newLocation ,5.0f ,random_int);
        enemies.push_back(newEnemy);

    } 


    private: 
    
    void updateEnemies() {

        //cout << "enemigos en array: " << enemies.size() << endl;

        for (auto& enemy : enemies) {

            enemy.update();
        }

    }

    void delete_inactive(){


            for (auto it = enemies.begin(); it != enemies.end(); ){

                if (!it -> active){

                    cout << "se eliminó: " << it -> colorName << endl;
                    it = enemies.erase(it);


                }
                else {
                    it++;
                }

            }

        }
        

    


    public: void update(){

        funcTimer();
        delete_inactive();
        updateEnemies();

    }

};

//MARK: PLAYER

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
                DrawEllipse(location.x+50,location.y,10.0f,15.0f,turbinaColor);
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

        if (IsKeyPressed(KEY_P)){

            paused = !paused;

        }

        if (IsKeyPressed(KEY_O)){

            dSystem = !dSystem;

        }

        if (IsKeyPressed(KEY_K)){

            vida -= 90;
            cout << "KILLED HIMSELF" << endl;

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

    void checkCollision(auto& group) { // Asegúrate de pasar el grupo por referencia

        playerRect = { location.x, location.y, 50, 50 };

        // Usamos un iterador explícito para recorrer la lista de enemigos
        for (auto it = group.enemies.begin(); it != group.enemies.end(); ++it) {
            // Accedemos al objeto enemigo a través del iterador
            auto& enemy = *it;

            if (CheckCollisionRecs(playerRect, enemy.enemyRect) && !damaged) {
                if (enemy.active) {
                    vida -= enemy.enemylevel;
                    damaged = true;
                    enemy.explode();
                }
            }
        }
    }
    public: void updatePlayer(auto& group) {

        runTimer();
        //cout << damageTimer << endl;
        checkCollision(group);
        input();
        dibujarTurbina();
        dibujarPlayer();

    }

};

//MARK: GAME OVER

void drawGameOver(){

    //Color gameoverColor = Color(0,0,0,255);

    DrawText("Game Over",screenWidth/3,screenHeight/2,40,RED);

    DrawText("Press (R) to restart",screenWidth/3-50,screenHeight/2 + 100,45,RED);


    if (IsKeyPressed(KEY_R)){

        dead = false;
        main();

    }

}

//MARK: MAIN PROGRAM

int main() {

	const string playerName = "Player 1";
	int playerPunteo = 200;
	

    InitWindow(screenWidth, screenHeight, "Ship Dodger");

    SetTargetFPS(60);

    initPlayerPosition = {screenWidth/2,400};

    Player player = Player();
    player.vida = 100;
    player.location = initPlayerPosition;
    player.speed = 5;
    player.initspeed = 5;
    string valor;
    string enemiesString;
    string explosionString;
    
    DarkVoid background_void = DarkVoid();
    EnemyGen enemygenerator = EnemyGen(fxadminmain);


    while (!WindowShouldClose()) {

        BeginDrawing();
        
        if (background_void.growVoid()){
            ClearBackground(WHITE);
        }
        else {
            ClearBackground(BLACK);
        }

        if (dSystem){

            enemiesString = "Active Enemies: " + to_string(enemygenerator.enemies.size());
            explosionString = "Active Explosions: " + to_string(fxadminmain.explosions.size());

            DrawText(enemiesString.c_str(),5,30,20,RED);
            DrawText(explosionString.c_str(),5,60,20,RED);

        }

        valor = to_string(player.vida);

        enemygenerator.update();

        if (player.vida > 0){
            player.updatePlayer(enemygenerator);
        }
        else{

            if (!dead) {
            fxadmin1.addExplosion(player.location,50);
            dead = true;
            }

            drawGameOver();
        }

									fxadminmain.updateExplosions();
        
		DrawText("LIFE POINTS: ", 30, 100, 30, MAROON);
        DrawText("SHIP DODGER 9000",5,5,40,BLUE);
		DrawText(valor.c_str(), 30, 150, 40, LIME);
		
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
