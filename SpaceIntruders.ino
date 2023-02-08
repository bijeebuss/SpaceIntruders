// NEED TO MAKE ROCKS EXPLODE WHEN BEAM HITS THEM
// DIfferent version with no rotation and a button to control the laser
// add scoreboard

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void setup() {
  Serial.begin(9600);
  matrix.begin(0x70);  // pass in the address

}
int p1_position = 3; // starting position of player 1
int p1_base_interval = 500; // increase this to slow down player 1 / reduce sensitivity
int no_of_rocks = 6; // set the number of falling rocks
int initial_rotation = 3; // initial rotation of the display, change from 0 to 3 to set rotation
int p1_norm = 515; // normalizer value. Increase if you move to the left and decrease if you move to the right
int p1_threshold = 50; // the minimum joystick movement to start moving. Increase to lower sensitivity
int rock_base = 600; // increase to slow down rocks
int rock_maxspeed = 6; // increase to make the fastest rocks faster
int rock_minspeed = 2; // increase to make the slowest rocks faster
int brightness = 3;    // change between 0 for dimmest and 15 for brightest

// other startup variables you shouldnt need to change
int powerup_x;
int powerup_y = 9;
long powerup_interval; 
long powerup_prev = 0;
int powerup_collision = 0;
int laser_on;
int rock_explode = 0;
int rock_x[6];
int rock_y[6] = {
  9,9,9,9,9,9};
long rock_prev[6] = {
  0,0,0,0,0,0};
long p1_prev = 0;
int laser =0;
int laser_last = 0;
long laser_prev =0;
long laser_current;
long laser_interval = 10000;
int rotation = 0;
long beam_current;
long beam_prev = 0;
int beam_y = 6;
int rock_speed[6];
int p1_direction;
int p1_speed;
int p1_score = 0;
int print_score = 1;

void loop() {
  
  if (print_score == 1){
    Serial.println(p1_score);
  }
  print_score = 0;
  
  randomSeed(millis());    // initialize random seed
  matrix.setBrightness(brightness);
  matrix.setRotation(initial_rotation + rotation);   // set rotation
  matrix.setTextColor(LED_RED);
  matrix.setTextWrap(false);

  int sensorValue_x = analogRead(A0);  //read voltage
  int ecks = sensorValue_x - p1_norm ;     // normalize 

  // CLEAR
  matrix.clear();

  // LIGHT UP 
  for (int8_t i=0; i < no_of_rocks; i++){   // light up only number of rocks
    matrix.drawPixel(rock_x[i],rock_y[i],LED_RED);
  }
  if (laser == 0){
    matrix.drawPixel(powerup_x ,powerup_y, LED_YELLOW);  // light up powerup
  }

  if (laser == 0 ){
    matrix.drawPixel(p1_position,7,LED_GREEN);          // light up player
  }

  if ( laser ==1 ){     // light up laser
    matrix.drawPixel(p1_position,7,LED_RED);
    matrix.drawPixel(p1_position,6,LED_GREEN);
    matrix.drawPixel(p1_position +1,7, LED_GREEN);
    matrix.drawPixel(p1_position +-1,7, LED_GREEN);
    matrix.drawPixel(p1_position ,beam_y, LED_YELLOW);
  }

  // WRITE
  matrix.writeDisplay();  

  // GET DIRECTION

  if (abs(ecks) < p1_threshold){
    p1_direction = 0;
  }   // no direction
  else if ( ecks > 0 ){ 
    p1_direction = 1;
  }   // left
  else if ( ecks < 0 ){
    p1_direction = 2;
  }   // right

  // GET SPEED

  if (abs(ecks) < 100){
    p1_speed = 1;
  }
  else if (abs(ecks) < 200){
    p1_speed = 2;
  }
  else if (abs(ecks) < 300){
    p1_speed = 3;
  }
  else if (abs(ecks) < 400){
    p1_speed = 4;
  }
  else if (abs(ecks) > 400){
    p1_speed = 5;
  }

  long p1_interval = p1_base_interval / p1_speed; 

  // MOVE
  unsigned long current_p1 = millis();       
  if(current_p1 - p1_prev > p1_interval) {     // move after time interval

    if (p1_direction == 0){
    }
    else if (p1_direction == 1 && p1_position > 0){
      p1_prev = current_p1;       // only set previous time if a movement was made
      p1_position--;
    }
    else if(p1_direction == 2 && p1_position < 7 ){
      p1_prev = current_p1; 
      p1_position++;
    }
  }

  // ROCK SPAWN
 for (int8_t i=0; i < no_of_rocks; i++){   
    if (rock_y[i] > 8){
      rock_x[i] = random(0,8);  
      rock_y[i] = 0;
      rock_speed[i] = random(rock_minspeed,rock_maxspeed); 
    }
  }

  // ROCK FALL
  for (int8_t i=0; i < no_of_rocks; i++){
    int rock_interval[6];
    long rock_current[6];
    rock_interval[i] = rock_base / rock_speed[i]; 
    rock_current[i] = millis();
    if(rock_current[i] - rock_prev[i] > rock_interval[i]) {
      rock_prev[i] = rock_current[i]; 
      rock_y[i]++;
    }
  }

  // POWERUP SPAWN
  if (powerup_y > 8 || powerup_collision == 1){
    powerup_x = random(0,8);  
    powerup_y = 0;
    powerup_interval = random(50,300); 
    powerup_collision = 0;
  }

  // POWERUP FALL
  long powerup_current = millis();
  if(powerup_current - powerup_prev > powerup_interval) {
    powerup_prev = powerup_current; 
    powerup_y++;
  }

  // POWERUP COLLISION
  if (powerup_x == p1_position && powerup_y == 7 && laser == 0){
    rotation++;
    powerup_collision = 1;
    laser_on = 1;
  }

  //ROCK COLLISION 
    for (int8_t i=0; i < no_of_rocks; i++){
   if (rock_x[i] == p1_position && rock_y[i] == 7 && laser == 0){
     for (int8_t z=0; z <= 2; z++) // Blink 5 times
   { 
   matrix.clear(); 
   matrix.writeDisplay(); // write blank screen
   delay(200); // wait 
   matrix.clear();  
   matrix.drawPixel(p1_position,7,LED_YELLOW);
   matrix.writeDisplay();  
   delay(200); // wait
   }
   while (p1_interval > 0){
   for (int8_t x=7; x>=-75; x--) {
   matrix.setRotation(initial_rotation);
   matrix.clear();
   matrix.setCursor(x,0);
   matrix.print("Game Over :(");
   matrix.writeDisplay();
   delay(100);
   }
   }
   }
   }
 
 
  // ACTIVATE LASERS
  laser_current = millis();
if (rotation % 4 == 0 && rotation != initial_rotation && laser_on == 1 ){  // turn laser on and start timer
    laser = 1;
    laser_prev = laser_current;
  }

  if(laser_current - laser_prev > laser_interval) {   // turn laser off after time period
    laser = 0;
  }
// SHOOT LASERS
  beam_current = millis();
  if(beam_current - beam_prev > 50) {   // speed of lasers
    beam_y--;
    beam_prev = beam_current;
  }
// LASER COLLISION
  for (int8_t i=0; i < no_of_rocks; i++){
    if (rock_x[i] == p1_position && rock_y[i] == beam_y && laser == 1){
      rock_explode = 1;
      rock_x[i] = random(0,8);  
      rock_y[i] = 0;
      rock_speed[i] = random(rock_minspeed,rock_maxspeed);
      p1_score = p1_score + 100;
      Serial.println(p1_score);
    }
  }

// LASER RESPAWN  
  if (beam_y == -1 || rock_explode == 1){    // laser blocks
    beam_y = 5;
    rock_explode = 0;
  }

  laser_on = 0;   // returns to zero so that the laser timer above does not reset

// RESET ROCKS WHEN LASER TURNS FROM ON TO OFF
  if (laser != laser_last) {
    if (laser == 1) {       // if the current state is HIGH then the button went from off to on:
      int thecowsaysmoo = 5;
    } 
    else {
      for (int8_t i=0; i < no_of_rocks; i++){
        rock_y[i] = 0;
      }
    }  
  }
  laser_last = laser; // save the current state as the last state, for next time through the loop

}



