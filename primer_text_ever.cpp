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
#include <cmath>

using namespace std;

int main();

class FxAdmin; 

const int screenWidth = 800;
const int screenHeight = 600;
Vector2 initPlayerPosition;

float level_speed[10] = {1.0f,1.1f,1.2f,1.3f,1.4f,1.5f,1.6f,1.7f,1.8f,2.0f};
float global_speed = level_speed[0];
int speed_int = 1; 
int total_enemies = 0;

Color grisito = {2,2,2,240};

float global_void_offset = 0.0f;

const double math_pi = 3.14159265358979323846;

bool paused = false;
bool dSystem = false;
bool dead = false;
bool startMenuPlayed = false;
bool win = false;
bool startfade = false;

//MARK: Stamina Pickup

class StaminaPickup {
public:
    Vector2 location;
    float size = 15.0f;
    float speed = 5.0f;
    bool active = true;
    Rectangle collisionRect;
    

    StaminaPickup(Vector2 loc){

        location = loc;

    }

    void debugDraw(){

        DrawRectangle(location.x-15,location.y-15,30,30,LIGHTGRAY);

    }

    void pickup(){

        active = false;

    }

    void check_outside(){

         if (location.y > screenHeight + size && active){

            active = false;

        }       

    }

    void draw(){
        DrawCircleLines(location.x,location.y,size,PINK);
        DrawCircleLines(location.x,location.y,size+2,PINK);
        DrawCircleLines(location.x,location.y,size+4,PINK);
        DrawCircleLines(location.x,location.y,size+6,PINK);

    }

    void move(){
        location.y += speed;
    }

    void update(){

        if (dSystem){debugDraw();} 
        collisionRect = {location.x-15,location.y-15,size*2,size*2};
        draw();
        move();

    }


};


//MARK: BACKG void

class DarkVoid {
public:

    int size = 1;
    int growValue = 8;
    bool black = true;
    float offset = global_void_offset;

    bool growVoid() {
        offset = global_void_offset;

        if (size < 800){

            if (black){
                DrawCircle(200,200+offset,size,BLACK);
            }
            else {
                DrawCircle(200,200+offset,size,WHITE);
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


//MARK: DEBRIS

class Debris {
public:

    int debrisSize;
    float debrisSizeF;
    Vector2 initLocation;
    Vector2 directionVector;
    Color debrisColor;
    Rectangle debrisRect;
    int debrisSpeed;
    bool distance; // 3D effect wow
    bool active;

    Debris(int size, Vector2 initLoc, Vector2 direction, Color color,int speed, bool bool3d){

        debrisSize = size;
        debrisSizeF = static_cast<float>(debrisSize);
        initLocation = {initLoc.x,initLoc.y};
        directionVector = {direction.x,direction.y};
        debrisColor = color;
        debrisRect = {initLocation.x,initLocation.y,debrisSizeF,debrisSizeF};
        debrisSpeed = speed;
        distance = bool3d;
        active = true; 

        if (distance){debrisSpeed *= 4.6;}

    }

    private: void moveDebris(){

        initLocation.x += directionVector.x * debrisSpeed;
        initLocation.y += directionVector.y * debrisSpeed;

        debrisRect = {initLocation.x,initLocation.y,debrisSizeF,debrisSizeF};

    }

    //3D effect WOW!
    void moveDistance(){

        if (distance){ //approaches
            
            if (debrisSizeF < 100){
                debrisSizeF += 3; 
            }
            else {
                debrisSizeF = 100;
            }        
        }
        else{ //gets farther away

            if (debrisSizeF > 1){
            debrisSizeF -= 0.5;
            }
            else {
                debrisSizeF = 1;
            }
        }


    }

    void check_outside(){

        if (initLocation.x < 0 || initLocation.x > screenWidth || initLocation.y < 0 || initLocation.y > screenHeight){

            active = false;

        }
    }

    public: void update(){

        check_outside();
        moveDebris();
        moveDistance();
        DrawRectangleRec(debrisRect,debrisColor);
        
    }
};

//MARK: EXPLOSION


class Explosion {

public:

    Vector2 explosionLocation;
    float explosionSize;
    int growspeed;
    bool ended;
    Color debrisColor;
    vector<Debris> debrisObjects;

    bool isthrust;

    Explosion(Vector2 location,float size, Color color, bool thrust = false){

        explosionLocation = location;
        explosionSize = size;
        growspeed = 30;
        ended = false;
        debrisColor = color;
        isthrust = thrust;

        initDebris();

    }

    void initDebris(){

        random_device rd; 
        mt19937 gen(rd()); 

        std::uniform_int_distribution<> int_dis1(10, 20); 
        int random_amount = int_dis1(gen);

        for (int i = 0; i <= random_amount; i++){

            std:: uniform_int_distribution<> int_dis2(5,20);
            int random_size = int_dis2(gen);

            std:: uniform_int_distribution<> int_dis3(1,360);
            int random_angle = int_dis3(gen);

            std:: uniform_int_distribution<> int_dis4(2,10);
            int random_speed = int_dis4(gen);

            std:: uniform_int_distribution<> int_dis5(0,1);
            bool random_3d = int_dis5(gen);

            float angle_radians = random_angle * (math_pi / 180.0f);
            Vector2 newDirection;
            newDirection.x = cos(angle_radians);
            newDirection.y = sin(angle_radians);


            Debris newdebris = Debris(random_size,explosionLocation,newDirection,debrisColor,random_speed,random_3d);
            debrisObjects.push_back(newdebris);

        }
    }

    void updateDebris(){

        for (Debris& debris : debrisObjects){

            debris.update();

        }

    }


    void deleteInactive(){

        if (!debrisObjects.empty()){
        for (std::vector<Debris>::iterator it = debrisObjects.begin(); it != debrisObjects.end(); ){

            if (!it -> active){

                it = debrisObjects.erase(it);

            }
            else {
                it++;
            }

        }        
        }
        else {
            ended = true;
        }

    }

    void grow(){

        if (explosionSize < 1000 && !isthrust){
            explosionSize += growspeed;
        }
        else if (explosionSize < 100 && isthrust){
            explosionSize += growspeed *0.5;
        }
        else if (explosionSize >= 100 && isthrust){

            ended = true;

        }

    }


    void debug(){

        if (dSystem){

            string debugString = to_string(debrisObjects.size());
            string debugEnded = "Ended: " + to_string(ended);
            DrawText(debugString.c_str(),explosionLocation.x,explosionLocation.y,20,MAROON);
            DrawText(debugEnded.c_str(),explosionLocation.x,explosionLocation.y+10,20,MAROON);


        }

    }

    void drawDebris() {



    }

    void draw() {

        if (!isthrust){
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize,YELLOW);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+2,ORANGE);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+3,YELLOW);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+4,ORANGE);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+5,YELLOW);
            }
        else{
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize,debrisColor);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+2,WHITE);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+3,debrisColor);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+4,WHITE);
            DrawCircleLines(explosionLocation.x,explosionLocation.y,explosionSize+5,debrisColor);
        }
    }

    void update() {

        if (!isthrust){
        updateDebris();
        }

        deleteInactive();
        debug();

        if (!ended){
        grow();
        draw();
        }
    }


};

//MARK: FXAdmin

class FxAdmin {
public:
    std::vector <Explosion> explosions;


    void addExplosion(Vector2 location, int size, Color color,bool thrust = false){

        if (!thrust){
            Explosion newexplosion = Explosion(location,size,color);
            explosions.push_back(newexplosion);}
        else{
            Explosion newexplosion = Explosion(location,size,color,true);
            explosions.push_back(newexplosion);}            
        
        

    }

    void deleteInactive(){


        for (std::vector<Explosion>::iterator it = explosions.begin(); it != explosions.end(); ){

            if (it -> ended){

                it = explosions.erase(it);

            }
            else {
                it++;
            }

        }        
        


    }

    void updateExplosions(){

        deleteInactive();
        for (Explosion& explosion : explosions){

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
        fxadminmain.addExplosion(initLoc, size,enemyColor);
        

    }

    void moveEnemy(){

        if (!paused){
        initLoc.y += speed * global_speed; 
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
    std::vector <StaminaPickup> staminaGroup;

    int timerDefault = 50;
    int timer = 50;

    int staminaTimerDefault = 10;
    int staminaTimer = staminaTimerDefault;

    void funcTimer(){ //FOR BOTH TIMERS, ENEMY AND STAMINAPICKUPS

        if (timer > 0){
            timer--;
        }

        else if (timer <= 0) {
            generateEnemy();
            timer = timerDefault;
        }

        if (staminaTimer > 0){
            staminaTimer--;
        }
        else if (staminaTimer <= 0){
            generatePickup();
            staminaTimer = staminaTimerDefault;
        }

    }



    void timerDifficulty(){

        switch (speed_int)
        {
        case 1:
            timerDefault = 50;
            break;
        case 2:
            timerDefault = 45;
            break;
        case 3:
            timerDefault = 40;
            break;
        case 4:
            timerDefault = 30;
            break;
        case 5:
            timerDefault = 25;
            break;
        default:
            timerDefault =  20;
            break;       
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

    void generatePickup(){

        random_device rd; 
        mt19937 gen(rd()); 

        uniform_real_distribution<> dis(0.0f, 800.0f); // Rango de 0 a 800 // Generar un número float aleatorio en el rango especificado float numeroRandom =
        float numeroRandom = dis(gen);

        Vector2 newLocation = {numeroRandom,-100.0f};

        std::uniform_int_distribution<> int_dis(1, 10); 
        int random_int = int_dis(gen);

        if (random_int == 5){ //aquí se pueden meter otras condicionales para otros items segun el número obtenido
            StaminaPickup newstaminaPickup = StaminaPickup(newLocation);
            staminaGroup.push_back(newstaminaPickup);
        }
    } 

    private: 
    
    void updateEnemies() {

        for (Enemy& enemy : enemies) {
            enemy.update();
        }

    }

    void updateStaminaGroup(){

        for (StaminaPickup& stamina : staminaGroup){

            stamina.update();

        }
    }

    void delete_inactive(){


            for (std::vector<Enemy>::iterator it = enemies.begin(); it != enemies.end();){

                if (!it -> active){
                    it = enemies.erase(it);
                    total_enemies++;
                }
                else {
                    it++;
                }

            }

        }
        
    void delete_inactive_staminaPickups(){


            for (std::vector<StaminaPickup>::iterator it = staminaGroup.begin(); it != staminaGroup.end();){

                if (!it -> active){
                    it = staminaGroup.erase(it);
                }
                else {
                    it++;
                }

            }

        }        
    public: void update(){

        if (!paused){
            funcTimer();
            updateEnemies();
            updateStaminaGroup();
            }
        
        delete_inactive();
        delete_inactive_staminaPickups();
        timerDifficulty();
        


    }

};

//MARK: PLAYER

class Player {
public:

    int vida;
    Vector2 location;
    
    float speed;
    float initspeed;
    Color playerColor = GREEN;

    bool thrust = false;
    float thrustMulti = 1.0f;
    float thrustTurbina = 1.0f;
    float stamina = 50.0f;
    float maxstamina = stamina;
    float staminaDrain = 0.5f;

private:
    float friction = 0.5f;
    Rectangle playerRect = {location.x,location.y,50,50};
    Vector2 movement;

    int damageTimer = 25;
    int initTimer = 25;
    bool damaged = false;

    int turbinas[5] = {0,1,2,3,4};
    int turbinaActiva = turbinas[0];
    Color turbinaColor = {255,200,0,255};



    

    private: void dibujarPlayer() {
        DrawRectangleV(location,{50,50},playerColor);
    }

    private: void dibujarTurbina(){

        switch (turbinaActiva)
        {
            case 3: // location: TOP FACE
                DrawEllipse(location.x,location.y,10.0f,15.0f+thrustTurbina,turbinaColor);
                DrawEllipse(location.x+50,location.y,10.0f,15.0f+thrustTurbina,turbinaColor);
                break;

            case 4: // location: RIGHT FACE
                DrawEllipse(location.x+50,location.y,15.0f+thrustTurbina,10.0f,turbinaColor);
                DrawEllipse(location.x+50,location.y+50,15.0f+thrustTurbina,10.0f,turbinaColor);
                break;

            case 1: // location: BOTTOM FACE
                DrawEllipse(location.x,location.y+50,10.0f,15.0f+thrustTurbina,turbinaColor);
                DrawEllipse(location.x+50,location.y+50,10.0f,15.0f+thrustTurbina,turbinaColor);
                break;

            case 2: // location: LEFT FACE
                DrawEllipse(location.x,location.y,15.0f+thrustTurbina,10.0f,turbinaColor);
                DrawEllipse(location.x,location.y+50,15.0f+thrustTurbina,10.0f,turbinaColor);
                break;

            case 0:
                break;

        }
        


    }

    private: void thrustFunc(){

        if (stamina <= 0){
            stamina = 0;
            thrustMulti = 1.0f;
            thrustTurbina = 1.0f;
        }

        if (IsKeyPressed(KEY_SPACE) && stamina){
            fxadminmain.addExplosion(location,5,SKYBLUE,true);
        }

        if (IsKeyDown(KEY_SPACE)){

            if (stamina > 0){
            stamina -= staminaDrain;
            thrustMulti = 2.5f;
            thrustTurbina = 15.0f;}


        }
        else if (IsKeyDown(KEY_SPACE) && stamina <= 0){
            thrustMulti = 1.0f;
            thrustTurbina = 1.0f;
        }
        else {
            thrustMulti = 1.0f;
            thrustTurbina = 1.0f;            
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

        if (!paused){
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
                movement.x -= 1;
                speed = initspeed;
                turbinaActiva = turbinas[4];
            }
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
                movement.x += 1;
                speed = initspeed;
                turbinaActiva = turbinas[2];
            }
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
                movement.y -= 1;
                speed = initspeed;
                turbinaActiva = turbinas[1];
            } 
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)){
                movement.y += 1;
                speed = initspeed;
                turbinaActiva = turbinas[3];
            }
        }

        if (IsKeyPressed(KEY_P)){
            paused = !paused;

        }

        if (IsKeyPressed(KEY_O)){
            dSystem = !dSystem;

        }

        if (IsKeyPressed(KEY_K)){
            vida -= 90;

        }

        calculateFric();
        normalizeLocation();

        location += movement * speed * thrustMulti;

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

    void checkCollision(EnemyGen& group) { // Asegúrate de pasar el grupo por referencia

        playerRect = { location.x, location.y, 50, 50 };

        // Usamos un iterador explícito para recorrer la lista de enemigos
        for (std::vector<Enemy>::iterator it = group.enemies.begin(); it != group.enemies.end(); ++it) {
            // Accedemos al objeto enemigo a través del iterador
            Enemy& enemy = *it;

            if (CheckCollisionRecs(playerRect, enemy.enemyRect) && !damaged) {
                if (enemy.active) {
                    vida -= enemy.enemylevel;
                    damaged = true;
                    enemy.explode();
                }
            }
        }
        for (std::vector<StaminaPickup>::iterator it = group.staminaGroup.begin(); it != group.staminaGroup.end(); ++it) {
            // Accedemos al objeto enemigo a través del iterador
            StaminaPickup& stpickup = *it;

            if (CheckCollisionRecs(playerRect, stpickup.collisionRect) && !damaged) {
                if (stpickup.active) {
                    if (stamina < maxstamina){
                    stamina += 5;
                    stpickup.pickup();
                    }
                }
            }
        }
    }

    void limitMaxStamina(){

        if (stamina > maxstamina){

            stamina = maxstamina;

        }

    }

    public: void updatePlayer(EnemyGen& group) {

        if (!paused) {
            runTimer();
            thrustFunc();
            checkCollision(group);
            }
        limitMaxStamina();
        input();
        dibujarTurbina();
        dibujarPlayer();

    }

};

//MARK: GAME OVER

void drawGameOver(){

    DrawText("Game Over",screenWidth/3,screenHeight/2,40,RED);

    DrawText("Press (R) to restart",screenWidth/3-50,screenHeight/2 + 100,45,RED);


    if (IsKeyPressed(KEY_R)){

        dead = false;
        win = false;
        startMenuPlayed = false;
        total_enemies = 0;
        main();

    }

}

void debug_change_level(){


    if (IsKeyPressed(KEY_ONE)){

        global_speed = level_speed[0];
        speed_int = 1;

    }
    else if (IsKeyPressed(KEY_TWO)){

        global_speed = level_speed[1];
        speed_int = 2;

    }
    else if (IsKeyPressed(KEY_THREE)){

        global_speed = level_speed[2];
        speed_int = 3;

    }
    else if (IsKeyPressed(KEY_FOUR)){

        global_speed = level_speed[3];
        speed_int = 4;

    }
    else if (IsKeyPressed(KEY_FIVE)){

        global_speed = level_speed[4];
        speed_int = 5;

    }
    else if (IsKeyPressed(KEY_SIX)){

        global_speed = level_speed[5];
        speed_int = 6;

    }
    else if (IsKeyPressed(KEY_SEVEN)){

        global_speed = level_speed[6];
        speed_int = 7;

    }
    else if (IsKeyPressed(KEY_EIGHT)){

        global_speed = level_speed[7];
        speed_int = 8;

    }
    else if (IsKeyPressed(KEY_NINE)){

        global_speed = level_speed[8];
        speed_int = 9;

    }
    else if (IsKeyPressed(KEY_ZERO)){

        global_speed = level_speed[9];
        speed_int = 10;

    }


}

void change_level(){

    if (total_enemies < 20){
        
        global_speed = level_speed[0];
        speed_int = 1;
        global_void_offset += 0.1;


    }
    else if (total_enemies < 50){

        global_speed = level_speed[1];
        speed_int = 2;

    }
    else if (total_enemies < 70){

        global_speed = level_speed[2];
        speed_int = 3;

    }
    else if (total_enemies < 100){

        global_speed = level_speed[3];
        speed_int = 4;

    }
    else if (total_enemies < 130){

        global_speed = level_speed[4];
        speed_int = 5;

    }
    else if (total_enemies < 160){

        global_speed = level_speed[5];
        speed_int = 6;

    }
    else if (total_enemies < 190){

        global_speed = level_speed[6];
        speed_int = 7;
        global_void_offset += 0.1;

    }
    else if (total_enemies < 210){

        global_speed = level_speed[7];
        speed_int = 8;
        global_void_offset += 0.2;

    }
    else if (total_enemies < 240){

        global_speed = level_speed[8];
        speed_int = 9;
        global_void_offset += 0.2;


    }
    else if (total_enemies < 270){

        global_speed = level_speed[9];
        speed_int = 10;
        global_void_offset += 0.2;

    }

    else if (total_enemies < 320){

        win = true;

    }

    

}

void victoryScreen(){

    DrawText("YOU WON!",(screenWidth/2)-220,(screenHeight/2)-60,50,BLUE);
    DrawText("Press [ENTER] to Restart or [ESC] to exit",(screenWidth/2)-270,(screenHeight/2)+10,50,RED);

    if (IsKeyPressed(KEY_ENTER)){

        dead = false;
        win = false;
        startMenuPlayed = false;
        total_enemies = 0;
        main();

    }
}


void startMenu(){

    if (startfade && grisito.a > 0){
        grisito.a -= 5;
    }
    else if (startfade && grisito.a <= 0){
        startfade = false;
        startMenuPlayed = true;
    }

    DrawRectangle(1,1,800,600,grisito);
    DrawText("SHIP DODGER 9000",(screenWidth/2)-220,(screenHeight/2)-60,50,BLUE);
    DrawText("Press [ENTER] to start",(screenWidth/2)-270,(screenHeight/2)+10,50,RED);
    DrawText("-Controls-",(screenWidth/2)-270,(screenHeight/2)+50+20,30,GOLD);
    DrawText("- WASD or Arrows to move",(screenWidth/2)-270,(screenHeight/2)+100+10,20,GOLD);
    DrawText("- SPACE to Dash",(screenWidth/2)-270,(screenHeight/2)+130+10,20,GOLD);
    DrawText("- ESC to exit",(screenWidth/2)-270,(screenHeight/2)+160+10,20,GOLD);


    if (IsKeyPressed(KEY_ENTER)){

        startfade = true;
        
    }



}

void pauseMenu(){

    grisito = {2,2,2,220};
    DrawRectangle((screenWidth/2)-300,(screenHeight/2)-150,600,400,grisito);
    DrawText("PAUSED",(screenWidth/2)-220,(screenHeight/2)-80,50,BLUE);
    DrawText("Press [P] to return",(screenWidth/2)-270,(screenHeight/2)-10,50,GOLD);
    DrawText("-Controls-",(screenWidth/2)-270,(screenHeight/2)+50,30,GOLD);
    DrawText("- WASD or Arrows to move",(screenWidth/2)-270,(screenHeight/2)+100,20,GOLD);
    DrawText("- SPACE to Dash",(screenWidth/2)-270,(screenHeight/2)+130,20,GOLD);
    DrawText("- ESC to exit",(screenWidth/2)-270,(screenHeight/2)+160,20,GOLD);
    

}

//MARK: MAIN PROGRAM

int main() {

	const string playerName = "Player 1";
	int playerPunteo = 200;
	

    InitWindow(screenWidth, screenHeight, "Ship Dodger 9000 v1.22");

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
    string levelString;
    string amount_enemies;
    string enemytimerstring;
    

    int staminaGUI;
    
    DarkVoid background_void = DarkVoid();
    EnemyGen enemygenerator = EnemyGen();


    while (!WindowShouldClose()) {

        BeginDrawing();
        
        //cout << to_string(grisito.a);
        if (background_void.growVoid()){
            ClearBackground(WHITE);
        }
        else {
            ClearBackground(BLACK);
            }
        

        if (!startMenuPlayed){

            startMenu();
            }

        else if (startMenuPlayed && !win){

            if (dSystem){

                debug_change_level();

                enemiesString = "Active Containers: " + to_string(enemygenerator.enemies.size());
                explosionString = "Active Explosion Fx: " + to_string(fxadminmain.explosions.size());
                amount_enemies = "Enemies Dead: " + to_string(total_enemies);
                enemytimerstring = "Enemy Timer: " + to_string(enemygenerator.timerDefault);
                

                DrawText(enemiesString.c_str(),5,235,20,RED);
                DrawText(explosionString.c_str(),5,260,20,RED);
                DrawText(amount_enemies.c_str(),5,280,20,RED);
                DrawText(enemytimerstring.c_str(),5,300,20,RED);
                

            }

            valor = to_string(player.vida);

            enemygenerator.update();
            fxadminmain.updateExplosions();

            if (player.vida > 0){
                player.updatePlayer(enemygenerator);
            }
            else{

                if (!dead) {
                fxadminmain.addExplosion(player.location,50,player.playerColor);
                dead = true;
                }

                drawGameOver();
            }
        
            DrawText("SHIP DODGER 9000 v1.22",5,5,40,BLUE);
            DrawText("LIFE POINTS: ", 30, 80, 30, MAROON);
            DrawText(valor.c_str(), 30, 120, 40, LIME);
            DrawText("press [P] for Pause and Controls",20,50,20,RED);
            
            

            DrawText("LEVEL", screenWidth*0.8, 50, 30, MAROON);
            levelString = to_string(speed_int);

            DrawText(levelString.c_str(),screenWidth*0.8 +50,80,50,RED);

            DrawText("STAMINA: ", 30, 180, 30, MAROON);
            staminaGUI = (player.stamina/player.maxstamina)* 100; //Actualizar tamaño de stamina con regla de 3 
            DrawRectangleLines(28,219,102,12,BLACK); // Lineas negritas alrededor del rosadito
            DrawRectangle(30,220,staminaGUI,10,PINK);

            if (paused) {
                pauseMenu();
                }
            else {
                change_level();
                }
            }
        
        else if (startMenuPlayed && win){
            victoryScreen();
            }
		
        EndDrawing();
    }

    CloseWindow();

    return 0;
}