// This is the final project for EECS 183
#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

bool start_game = true;
bool play_normal = false;
bool play_boss = false;

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();
void print_boss();
void game_won();
void print_menu(int potentiometerVal);
void print_boss_lives();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      strength = 0;
    }
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      strength = 0;
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    void set_strength(int strength_arg) {
      strength = strength_arg;
    }

    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      y++;
    }
    
    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      if (strength == 1) {
        draw_with_rgb(RED, BLUE);
      }
      else if(strength == 2) {
        draw_with_rgb(ORANGE, BLUE);
      }
      else if(strength == 3) {
        draw_with_rgb(YELLOW, BLUE);
      }
      else if(strength == 4) {
        draw_with_rgb(GREEN, BLUE);
      }
      else if(strength == 5) {
        draw_with_rgb(BLUE, BLUE);
      }
      else if(strength == 6) {
        draw_with_rgb(PURPLE, BLUE);
      }
      else if (strength == 7) {
        draw_with_rgb(WHITE, BLUE);
      }
      else {
        erase();
      }
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      erase();
      strength--;
      if (strength > 0) {
        draw();
      }
    }

  private:
    int x;
    int y;
    int strength;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y, body_color.to_333());
      matrix.drawPixel(x + 2, y, body_color.to_333());
      matrix.drawPixel(x + 3, y, BLACK.to_333());

      matrix.drawPixel(x, y + 1, body_color.to_333());
      matrix.drawPixel(x + 1, y + 1, eye_color.to_333());
      matrix.drawPixel(x + 2, y + 1, eye_color.to_333());
      matrix.drawPixel(x + 3, y + 1, body_color.to_333());

      matrix.drawPixel(x, y + 2, body_color.to_333());
      matrix.drawPixel(x + 1, y + 2, body_color.to_333());
      matrix.drawPixel(x + 2, y + 2, body_color.to_333());
      matrix.drawPixel(x + 3, y + 2, body_color.to_333());

      matrix.drawPixel(x, y + 3, body_color.to_333());
      matrix.drawPixel(x + 1, y + 3, BLACK.to_333());
      matrix.drawPixel(x + 2, y + 3, BLACK.to_333());
      matrix.drawPixel(x + 3, y + 3, body_color.to_333());
    }
};

class Cannonball {
  public:
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    void fire(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      fired = true;
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
      y--;
      if (y < 0) {
        fired = false;
        //erase(); or maybe reset();
      }
    }
    
    // resets private data members to initial values
    void hit() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // draws the Cannonball, if it is fired
    void draw() {
      if (fired) {
        matrix.drawPixel(x, y, ORANGE.to_333());
        matrix.drawPixel(x, y + 1, ORANGE.to_333());
      }
    }
    
    // draws black where the Cannonball used to be
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x, y + 1, BLACK.to_333());
      matrix.drawPixel(x, y + 2, BLACK.to_333());
    }

  private:
    int x;
    int y;
    bool fired;
};

class Player {
  public:
    Player() {
      x = 0;
      y = 0;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }

    void set_y(int y_arg) {
      y = y_arg;
    }

    void reset_lives() {
      lives = 3;
    }

    void set_lives_to_one() {
      lives = 1;
    }

    void set_lives_to_three() {
      lives = 3;
    }

    // Modifies: lives
    void die() {
      lives--;
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(AQUA);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // draws the player
    void draw_with_rgb(Color color) {
      matrix.drawPixel(x, y, color.to_333());
      matrix.drawPixel(x - 1, y + 1, color.to_333());
      matrix.drawPixel(x, y + 1, color.to_333());
      matrix.drawPixel(x + 1, y + 1, color.to_333());
    }
};

class EnemyCannonball {

  public:
    EnemyCannonball() {
        x = 0;
        y = 0;
        fired = false;
    }

    // Resets private data members to initial values
    void reset() {
        x = 0;
        y = 0;
        fired = false;
    }

    // Getter methods
    int get_x() const {
        return x;
    }

    int get_y() const {
        return y;
    }

    bool has_been_fired() const {
        return fired;
    }

    // Fires the cannonball at a specific position
    void fire(int x_arg, int y_arg) {
        x = x_arg;
        y = y_arg;
        fired = true;
    }

    // Moves the cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
        y++;
        if (y > 14) {
            fired = false;
        }
    }

    // Resets private data members to initial values
    void hit() {
        x = 0;
        y = 0;
        fired = false;
    }

    // Draws the cannonball if it is fired
    void draw() {
        if (fired) {
            matrix.drawPixel(x, y, PURPLE.to_333());
        }
    }

    // Draws black where the cannonball used to be
    void erase() {
        matrix.drawPixel(x, y, BLACK.to_333());
    }

  private:
    int x;
    int y;
    bool fired;
};

class Boss {
  public:
    // Constructors
    Boss() {
      x = 0;
      y = 0;
      strength = 0;
    }
    // sets values for private date members x and y
    Boss(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      strength = 0;
    }
    // sets values for private data members
    Boss(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    void set_strength(int strength_arg) {
      strength = strength_arg;
    }

    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // Moves the Boss down the screen by one row
    // Modifies: y
    void vertical_move() {
      y++;
    }

    void left_move() {
      x--;
    }
    
    void right_move() {
      x++;
    }

    // draws the Boss if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      if (strength > 25) {
        draw_with_rgb(BLUE, PURPLE);
      }
      else if(strength <= 25) {
        draw_with_rgb(WHITE, RED);
      }
      else {
        erase();
      }
    }
    
    // draws black where the Boss used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }    
    
    // Boss is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      erase();
      strength--;
      if (strength > 0) {
        draw();
      }
    }


  private:
    int x;
    int y;
    int strength;
    
    // draws the Boss
    void draw_with_rgb(Color body_color, Color eye_color) {
      matrix.drawPixel(x, y + 7, body_color.to_333());
      matrix.drawPixel(x, y + 6, body_color.to_333());
      matrix.drawPixel(x, y + 5, body_color.to_333());
      matrix.drawPixel(x + 1, y + 5, body_color.to_333());
      matrix.drawPixel(x + 1, y + 4, body_color.to_333());
      matrix.drawPixel(x + 1, y + 3, body_color.to_333());
      matrix.drawPixel(x + 1, y + 2, body_color.to_333());
      matrix.drawPixel(x + 2, y + 1, body_color.to_333());
      matrix.drawPixel(x + 2, y + 2, body_color.to_333());
      matrix.drawPixel(x + 2, y + 3, body_color.to_333());
      matrix.drawPixel(x + 2, y + 5, body_color.to_333());
      matrix.drawPixel(x + 2, y + 6, body_color.to_333());
      matrix.drawPixel(x + 1, y + 0, body_color.to_333());
      matrix.drawPixel(x + 3, y + 7, body_color.to_333());
      matrix.drawPixel(x + 3, y + 5, body_color.to_333());
      matrix.drawPixel(x + 3, y + 4, body_color.to_333());
      matrix.drawPixel(x + 3, y + 3, body_color.to_333());
      matrix.drawPixel(x + 3, y + 2, body_color.to_333());
      matrix.drawPixel(x + 4, y + 7, body_color.to_333());
      matrix.drawPixel(x + 4, y + 5, body_color.to_333());
      matrix.drawPixel(x + 4, y + 4, body_color.to_333());
      matrix.drawPixel(x + 4, y + 3, body_color.to_333());
      matrix.drawPixel(x + 4, y + 2, body_color.to_333());
      matrix.drawPixel(x + 5, y + 6, body_color.to_333());
      matrix.drawPixel(x + 5, y + 5, body_color.to_333());
      matrix.drawPixel(x + 5, y + 3, body_color.to_333());
      matrix.drawPixel(x + 5, y + 2, body_color.to_333());
      matrix.drawPixel(x + 5, y + 1, body_color.to_333());
      matrix.drawPixel(x + 6, y + 5, body_color.to_333());
      matrix.drawPixel(x + 6, y + 4, body_color.to_333());
      matrix.drawPixel(x + 6, y + 3, body_color.to_333());
      matrix.drawPixel(x + 6, y + 2, body_color.to_333());
      matrix.drawPixel(x + 6, y, body_color.to_333());
      matrix.drawPixel(x + 7, y + 7, body_color.to_333());
      matrix.drawPixel(x + 7, y + 6, body_color.to_333());
      matrix.drawPixel(x + 7, y + 5, body_color.to_333());
      matrix.drawPixel(x, y, body_color.to_333());
      matrix.drawPixel(x + 7, y, body_color.to_333());


      matrix.drawPixel(x + 2, y + 4, eye_color.to_333());
      matrix.drawPixel(x + 5, y + 4, eye_color.to_333());
      matrix.drawPixel(x + 3, y + 6, eye_color.to_333());
      matrix.drawPixel(x + 4, y + 6, eye_color.to_333());


    }
};

class BossCannonball {
  public:
    BossCannonball() {
        x = 0;
        y = 0;
        fired = false;
    }

    // Resets private data members to initial values
    void reset() {
        x = 0;
        y = 0;
        fired = false;
    }

    // Getter methods
    int get_x() const {
        return x;
    }

    int get_y() const {
        return y;
    }

    bool has_been_fired() const {
        return fired;
    }

    // Fires the cannonball at a specific position
    void fire(int x_arg, int y_arg) {
        x = x_arg;
        y = y_arg;
        fired = true;
    }

    // Moves the cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
        y++;
        if (y > 15) {
            fired = false;
        }
    }

    // Resets private data members to initial values
    void hit() {
        x = 0;
        y = 0;
        fired = false;
    }

    // Draws the cannonball if it is fired
    void draw() {
        if (fired) {
            matrix.drawPixel(x, y, RED.to_333());
            matrix.drawPixel(x, y + 1, RED.to_333());
            matrix.drawPixel(x + 1, y, RED.to_333());
            matrix.drawPixel(x + 1, y + 1, RED.to_333());
        }
    }

    // Draws black where the cannonball used to be
    void erase() {
        matrix.drawPixel(x, y, BLACK.to_333());
            matrix.drawPixel(x, y + 1, BLACK.to_333());
            matrix.drawPixel(x + 1, y, BLACK.to_333());
            matrix.drawPixel(x + 1, y + 1, BLACK.to_333());
    }

  private:
    int x;
    int y;
    bool fired;
};

class Game {
  public:
    Game() {
      level = 0;
      time = 0;
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setupGame() {
      reset_level();
      restart_level = true;
    }

    int getLevel() {
      return level;
    }
    
    
    void update(int potentiometer_value, bool button_pressed) {
      if (restart_level) {
         matrix.fillScreen(BLACK.to_333());
         print_level(level);
         delay(2000);
         matrix.fillScreen(BLACK.to_333());
         print_lives(player.get_lives());
         delay(2000);
         matrix.fillScreen(BLACK.to_333());

         if (level == 1) {
          for (int i = 0; i < 8; ++i) {
            enemies[i].initialize(i * 4, 0, 1);
          }
          for (int i = 8 ; i < 16; ++i) {
            enemies[i].initialize(i * 4, 4, 0);
          }
         }

         if (level == 2) {
          enemies[0].initialize(0, 0, 1);
          enemies[1].initialize(4, 0, 2);
          enemies[2].initialize(8, 0, 1);
          enemies[3].initialize(12, 0, 2);
          enemies[4].initialize(16, 0, 1);
          enemies[5].initialize(20, 0, 2);
          enemies[6].initialize(24, 0, 1);
          enemies[7].initialize(28, 0, 2);
          enemies[8].initialize(0, 4, 2);
          enemies[9].initialize(4, 4, 1);
          enemies[10].initialize(8, 4, 2);
          enemies[11].initialize(12, 4, 1);
          enemies[12].initialize(16, 4, 2);
          enemies[13].initialize(20, 4, 1);
          enemies[14].initialize(24, 4, 2);
          enemies[15].initialize(28, 4, 1);
        }
        if (level == 3) {
          enemies[0].initialize(0, 0, 1);
          enemies[1].initialize(4, 0, 2);
          enemies[2].initialize(8, 0, 3);
          enemies[3].initialize(12, 0, 4);
          enemies[4].initialize(16, 0, 5);
          enemies[5].initialize(20, 0, 1);
          enemies[6].initialize(24, 0, 2);
          enemies[7].initialize(28, 0, 3);
          enemies[8].initialize(0, 4, 4);
          enemies[9].initialize(4, 4, 5);
          enemies[10].initialize(8, 4, 1);
          enemies[11].initialize(12, 4, 2);
          enemies[12].initialize(16, 4, 3);
          enemies[13].initialize(20, 4, 4);
          enemies[14].initialize(24, 4, 5);
          enemies[15].initialize(28, 4, 1);
        }

        if (level == 4) {
          enemies[0].initialize(0, 0, 5);
          enemies[1].initialize(4, 0, 4);
          enemies[2].initialize(8, 0, 5);
          enemies[3].initialize(12, 0, 4);
          enemies[4].initialize(16, 0, 5);
          enemies[5].initialize(20, 0, 4);
          enemies[6].initialize(24, 0, 5);
          enemies[7].initialize(28, 0, 4);
          enemies[8].initialize(0, 4, 2);
          enemies[9].initialize(4, 4, 3);
          enemies[10].initialize(8, 4, 2);
          enemies[11].initialize(12, 4, 3);
          enemies[12].initialize(16, 4, 2);
          enemies[13].initialize(20, 4, 3);
          enemies[14].initialize(24, 4, 2);
          enemies[15].initialize(28, 4, 3);
        }
        if (level >= 5) {
          enemies[0].initialize(0, 0, random(1,7));
          enemies[1].initialize(4, 0, random(1,7));
          enemies[2].initialize(8, 0, random(1,7));
          enemies[3].initialize(12, 0, random(1,7));
          enemies[4].initialize(16, 0, random(1,7));
          enemies[5].initialize(20, 0, random(1,7));
          enemies[6].initialize(24, 0, random(1,7));
          enemies[7].initialize(28, 0, random(1,7));
          enemies[8].initialize(0, 4, random(1,7));
          enemies[9].initialize(4, 4, random(1,7));
          enemies[10].initialize(8, 4, random(1,7));
          enemies[11].initialize(12, 4, random(1,7));
          enemies[12].initialize(16, 4, random(1,7));
          enemies[13].initialize(20, 4, random(1,7));
          enemies[14].initialize(24, 4, random(1,7));
          enemies[15].initialize(28, 4, random(1,7));
        }
        for (int i = 0; i < NUM_ENEMIES; i++) {
          enemies[i].draw();
        }
        delay(1000);
        restart_level = false;
        last_invader_update = millis();
        last_enemy_ball_update = millis();
      }

      // update player
      player.erase();
      int xVal = map(potentiometer_value, 1023, 0, 1, 30);
      player.set_x(xVal);
      player.set_y(14);
      if (player.get_lives() > 0)
        player.draw();


      // update cannonball
      if (button_pressed && !ball.has_been_fired()) {
        ball.fire(xVal, 13);
      }

      if (ball.has_been_fired()) {
        ball.erase();
        ball.move();
        ball.draw();
      }

      // check if all enemies on the second row died
      bool secondRowAlive = false;
      for (int i = 8; i < 16; i++) {
        if (enemies[i].get_strength() > 0) {
          secondRowAlive = true;
        }
      }

      // update enemy cannonball
      
      if (!enemy_ball.has_been_fired()) {
        int enemy_index = 0;
        bool select_alive = false;
        if (level == 1) {
          while (!select_alive) {
            enemy_index = random(0,8);
            if (enemies[enemy_index].get_strength() != 0) {
              select_alive = true;
            }
          }
          enemy_ball.fire(enemies[enemy_index].get_x() + random(1,3), enemies[enemy_index].get_y() + 3);
        }
        else if (level > 1) {
          if (secondRowAlive) {
            while (!select_alive) {
              enemy_index = random(8,16);
              if (enemies[enemy_index].get_strength() != 0) {
                select_alive = true;
              }
            }
            enemy_ball.fire(enemies[enemy_index].get_x() + random(1,3), enemies[enemy_index].get_y() + 3);
          }
          else {
            while (!select_alive) {
              enemy_index = random(0,8);
              if (enemies[enemy_index].get_strength() != 0) {
                select_alive = true;
              }
            }
            enemy_ball.fire(enemies[enemy_index].get_x() + random(0,3), enemies[enemy_index].get_y() + 3);
          }
        }
      }
      
      
      // update the enemy ball
      current_time2 = millis();
      if (enemy_ball.has_been_fired()){
        if (level < 3) {
          if ((current_time2 - last_enemy_ball_update) >= 175) {
            enemy_ball.erase();
            enemy_ball.move();
            enemy_ball.draw();
            last_enemy_ball_update = current_time2;
          }
        }
        else {
          if ((current_time2 - last_enemy_ball_update) >= 125) {
            enemy_ball.erase();
            enemy_ball.move();
            enemy_ball.draw();
            last_enemy_ball_update = current_time2;
          }
        }
      }
      
      

      // update invader
      current_time = millis();
      if (current_time - last_invader_update >= 2000) {
        if (secondRowAlive) {
          for (int i = 8; i < 16; i++) {
            enemies[i].erase();
            enemies[i].move();
            if (enemies[i].get_strength() > 0)
              enemies[i].draw();
          }
        }
        else {
          for (int i = 0; i < 8; i++) {
            enemies[i].erase();
            enemies[i].move();
            if (enemies[i].get_strength() > 0)
              enemies[i].draw();

          }
        }
        last_invader_update = current_time;
      }

      
      // detect if the enemy ball has hitted the player
      if ((enemy_ball.get_x() == player.get_x() && enemy_ball.get_y() == player.get_y()) || 
          (enemy_ball.get_x() == player.get_x() - 1 && enemy_ball.get_y() == player.get_y() + 1) ||
          (enemy_ball.get_x() == player.get_x() && enemy_ball.get_y() == player.get_y() + 1) ||
          (enemy_ball.get_x() == player.get_x() + 1 && enemy_ball.get_y() == player.get_y() + 1)) {
            enemy_ball.erase();
            enemy_ball.hit();
            player.die();
            restart_level = true;
      }
      

      // detect if the ball has hitted the invader
      for (int i = 0; i < NUM_ENEMIES; ++i) {
        int xCoordinate = enemies[i].get_x();
        int yCoordinate = enemies[i].get_y();
        if (enemies[i].get_strength() > 0) {
          if (ball.get_x() == xCoordinate && ball.get_y() == (yCoordinate + 3) ||
          (ball.get_x() == (xCoordinate + 1) && ball.get_y() == (yCoordinate + 2)) ||
          (ball.get_x() == (xCoordinate + 2) && ball.get_y() == (yCoordinate + 2)) ||
          (ball.get_x() == (xCoordinate + 3) && ball.get_y() == (yCoordinate + 3)) ||
          (ball.get_x() == xCoordinate && ball.get_y() == (yCoordinate + 2)) ||
          (ball.get_x() == (xCoordinate + 1) && ball.get_y() == (yCoordinate + 1)) ||
          (ball.get_x() == (xCoordinate + 2) && ball.get_y() == (yCoordinate + 1)) ||
          (ball.get_x() == (xCoordinate + 3) && ball.get_y() == (yCoordinate + 2))) {
            ball.erase();
            ball.hit();
            enemies[i].hit();
          }
        }
      }

      // detect if the palyer collide with invaders
      for (int i = 0; i < NUM_ENEMIES; ++i) {
        int xCoordinate = enemies[i].get_x();
        int yCoordinate = enemies[i].get_y();
        if (enemies[i].get_strength() > 0) {
          if (player.get_x() == xCoordinate && player.get_y() == (yCoordinate + 3) ||
          (player.get_x() == (xCoordinate + 1) && player.get_y() == (yCoordinate + 2)) ||
          (player.get_x() == (xCoordinate + 2) && player.get_y() == (yCoordinate + 2)) ||
          (player.get_x() == (xCoordinate + 3) && player.get_y() == (yCoordinate + 3))) {
            player.die();
            restart_level = true;
          }
        }
      }

      if (level_cleared()) {
        level++;
        restart_level = true;
      }

      bool reach_ground = false;
      for (int i = 0; i < NUM_ENEMIES; ++i) {
        if (enemies[i].get_y() > 11) {
          reach_ground = true;
          restart_level = true;
        }
      }

      if (reach_ground) {
        player.die();
      }


      if (player.get_lives() <= 0) {
        matrix.fillScreen(BLACK.to_333());
        game_over();
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        level = 1;
        player = Player();
        restart_level = true;
        start_game = true;
        play_normal = false;
      }

      delay(50);
    }

    
    void boss_update(int potentiometer_value, bool button_pressed) {

      if (player.get_lives() == 0) {
        restart_level = true;
      }

      if (restart_level) {
        // set player's live to one in boss mode
        player.set_lives_to_one();

        // print lives
        matrix.fillScreen(BLACK.to_333());
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        print_boss_lives();
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        boss.initialize(12, 0, boss_lives);
        bool direction = true;

        delay(1000);
        restart_level = false;
        last_boss_update = millis();
        last_boss_ball_update = millis();

        boss.draw();
        
      }

      // update player
      player.erase();
      int xVal = map(potentiometer_value, 1023, 0, 1, 30);
      player.set_x(xVal);
      player.set_y(14);
      if (player.get_lives() > 0)
        player.draw();


      // update cannonball
      if (button_pressed && !ball.has_been_fired()) {
        ball.fire(xVal, 13);
      }

      if (ball.has_been_fired()) {
        ball.erase();
        ball.move();
        ball.draw();
      }

      
      if (boss.get_x() == 24) {
        direction = false;
        boss.erase();
        boss.vertical_move();
        boss.draw();
        delay(500);
      }

      else if (boss.get_x() == 0) {
        direction = true;
        boss.erase();
        boss.vertical_move();
        boss.draw();
        delay(500);
      }

      current_time3 = millis();
      
      if (boss.get_strength() > boss_lives/2) {
        if (current_time3 - last_boss_update >= 500) {
          if (direction) {
            boss.erase();
            boss.right_move();
            boss.draw();
            last_boss_update = current_time3;
          }
          else {
            boss.erase();
            boss.left_move();
            boss.draw();
            last_boss_update = current_time3;
          }
        }
        
      }

      
      else {
        if (current_time3 - last_boss_update >= 250) {
          if (direction) {
            boss.erase();
            boss.right_move();
            boss.draw();
            last_boss_update = current_time3;
          }
          else {
            boss.erase();
            boss.left_move();
            boss.draw();
            last_boss_update = current_time3;
          }
        }
        
      }

      // update boss cannonball
      
      if (!boss_ball.has_been_fired()) {
        int x_position = boss.get_x() + random(0, 7);
        int y_position = 0;
        if ((x_position == 1) || (x_position == 5))
          y_position = boss.get_y() + 7;
        else
          y_position = boss.get_y() + 8;
        boss_ball.fire(x_position, y_position);
      }
      
      
      // update the enemy ball
      current_time4 = millis();
      if (boss_ball.has_been_fired()){
        if ((current_time4 - last_boss_ball_update) >= 175) {
          boss_ball.erase();
          boss_ball.move();
          boss_ball.draw();
          last_boss_ball_update = current_time4;
        }
      }

      

      // TODO: check if player's ball hits the boss
      if (boss.get_strength() > 0) {
        int xCoordinate = boss.get_x();
        int yCoordinate = boss.get_y();
        if (ball.get_x() == xCoordinate && ball.get_y() == (yCoordinate + 7) ||
            (ball.get_x() == (xCoordinate + 1) && ball.get_y() == (yCoordinate + 5)) ||
            (ball.get_x() == (xCoordinate + 2) && ball.get_y() == (yCoordinate + 6)) ||
            (ball.get_x() == (xCoordinate + 3) && ball.get_y() == (yCoordinate + 7)) ||
            (ball.get_x() == xCoordinate && ball.get_y() == (yCoordinate + 6)) ||
            (ball.get_x() == (xCoordinate + 1) && ball.get_y() == (yCoordinate + 4)) ||
            (ball.get_x() == (xCoordinate + 2) && ball.get_y() == (yCoordinate + 5)) ||
            (ball.get_x() == (xCoordinate + 3) && ball.get_y() == (yCoordinate + 6)) ||
            (ball.get_x() == (xCoordinate + 4) && ball.get_y() == (yCoordinate + 7)) ||
            (ball.get_x() == (xCoordinate + 5) && ball.get_y() == (yCoordinate + 6)) ||
            (ball.get_x() == (xCoordinate + 6) && ball.get_y() == (yCoordinate + 5)) ||
            (ball.get_x() == (xCoordinate + 7) && ball.get_y() == (yCoordinate + 7)) ||
            (ball.get_x() == (xCoordinate + 4) && ball.get_y() == (yCoordinate + 6)) ||
            (ball.get_x() == (xCoordinate + 5) && ball.get_y() == (yCoordinate + 5)) ||
            (ball.get_x() == (xCoordinate + 6) && ball.get_y() == (yCoordinate + 4)) ||
            (ball.get_x() == (xCoordinate + 7) && ball.get_y() == (yCoordinate + 6))) {
              ball.erase();
              ball.hit();
              boss.hit();
          }
       }

      // TODO: check if boss's ball hits the player
      if ((boss_ball.get_x() == player.get_x() && boss_ball.get_y() == player.get_y()) || 
          (boss_ball.get_x() == player.get_x() - 1 && boss_ball.get_y() == player.get_y() + 1) ||
          (boss_ball.get_x() == player.get_x() && boss_ball.get_y() == player.get_y() + 1) ||
          (boss_ball.get_x() == player.get_x() + 1 && boss_ball.get_y() == player.get_y() + 1) ||
          
          (boss_ball.get_x() + 1 == player.get_x() && boss_ball.get_y() == player.get_y()) || 
          (boss_ball.get_x() + 1 == player.get_x() - 1 && boss_ball.get_y() == player.get_y() + 1) ||
          (boss_ball.get_x() + 1 == player.get_x() && boss_ball.get_y() == player.get_y() + 1) ||
          (boss_ball.get_x() + 1 == player.get_x() + 1 && boss_ball.get_y() == player.get_y() + 1) ||
          
          (boss_ball.get_x() == player.get_x() && boss_ball.get_y() + 1 == player.get_y()) || 
          (boss_ball.get_x() == player.get_x() - 1 && boss_ball.get_y() + 1 == player.get_y() + 1) ||
          (boss_ball.get_x() == player.get_x() && boss_ball.get_y() + 1 == player.get_y() + 1) ||
          (boss_ball.get_x() == player.get_x() + 1 && boss_ball.get_y() + 1 == player.get_y() + 1) ||
          
          (boss_ball.get_x() +1 == player.get_x() && boss_ball.get_y() + 1 == player.get_y()) || 
          (boss_ball.get_x() + 1 == player.get_x() - 1 && boss_ball.get_y() + 1 == player.get_y() + 1) ||
          (boss_ball.get_x() + 1 == player.get_x() && boss_ball.get_y() + 1 == player.get_y() + 1) ||
          (boss_ball.get_x() + 1 == player.get_x() + 1 && boss_ball.get_y() + 1== player.get_y() + 1)) {
            boss_ball.erase();
            boss_ball.hit();
            player.die();
          }
 


      // TODO: check if boss  hits the player
      // check if the boss touches the player
      int xCoordinate = boss.get_x();
      int yCoordinate = boss.get_y() + 7;
      if (boss.get_strength() > 0) {
        if ((player.get_x() == xCoordinate && player.get_y() == yCoordinate) ||
          (player.get_x() == xCoordinate + 1 && player.get_y() == yCoordinate - 1) ||
          (player.get_x() == xCoordinate + 2 && player.get_y() == yCoordinate - 1) ||
          (player.get_x() == xCoordinate + 3 && player.get_y() == yCoordinate) ||
          (player.get_x() == xCoordinate + 4 && player.get_y() == yCoordinate) ||
          (player.get_x() == xCoordinate + 5 && player.get_y() == yCoordinate - 1) ||
          (player.get_x() == xCoordinate + 6 && player.get_y() == yCoordinate - 1) ||
          (player.get_x() == xCoordinate + 7 && player.get_y() == yCoordinate)) {
            player.die();
          }
      }

      // if the player dies, the player goes back to the menu

      if (player.get_lives() <= 0) {
        matrix.fillScreen(BLACK.to_333());
        game_over();
        delay(2000);
        matrix.fillScreen(BLACK.to_333());
        start_game = true;
        play_boss = false;
        restart_level = true;
        reset_level();
        player.set_lives_to_three();
      }

      if (boss.get_strength() == 0) {
        if (player.get_lives() == 3) {
          restart_level = true;
        }
        else {
          matrix.fillScreen(BLACK.to_333());
          game_won();
          delay(2000);
          matrix.fillScreen(BLACK.to_333());
          player.die();
          start_game = true;
          play_boss = false;
          reset_level();
          boss_ball.reset();
          player.set_lives_to_three();
        }
      }

      /*
      // if player wins, goes back to the menu
      if (boss.get_strength() == 0) {
        matrix.fillScreen(BLACK.to_333());
        game_won();
        delay(2000);
        start_game = true;
        play_boss = false;
      }
      */
      delay(50);
    }
    

  private:
    int current_time = 0;
    int current_time2 = 0;
    int current_time3 = 0;
    int current_time4 = 0;
    int last_invader_update = 0;
    int last_enemy_ball_update = 0;
    int last_boss_update = 0;
    int last_boss_ball_update = 0;
    bool restart_level = false;
    bool direction;
    int level;
    int boss_lives = 50;
    unsigned long time;
    Player player;
    Cannonball ball;
    EnemyCannonball enemy_ball;
    Invader enemies[NUM_ENEMIES];
    Boss boss;
    BossCannonball boss_ball;


    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      bool cleared = true;
      for (int i = 0; i < NUM_ENEMIES; ++i) {
        if (enemies[i].get_strength() > 0) {
          cleared = false;
        }
      }
      return cleared;
    }

    // set up a level
    void reset_level() {
      level = 1;
    }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  
  game.setupGame();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {

  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  // Serial.println(potentiometer_value);
  // Serial.println(button_pressed);
  if (start_game) {
    print_menu(potentiometer_value);
    if ((button_pressed) && (potentiometer_value > 512)) {
      play_normal = true;
      start_game = false;
    }
    else if ((button_pressed) && (potentiometer_value <= 512)) {
      play_boss = true;
      start_game = false;
    }
    
  }

  if (play_normal) {
    game.update(potentiometer_value, button_pressed); 
  }
  
  if (play_boss) {
    game.boss_update(potentiometer_value, button_pressed);
  }
  
}

// displays Level
void print_level(int level) {
  matrix.setCursor(0,0);
  matrix.setTextSize(1);
  matrix.print("Level: ");
  matrix.print(level);
}

// displays number of lives
void print_lives(int lives) {
  matrix.setCursor(0,0);
  matrix.setTextSize(1);
  matrix.print("Lives: ");
  matrix.print(lives);
}

// displays "game over"
void game_over() {
  matrix.setCursor(0,0);
  matrix.setTextSize(1);
  matrix.print("Game Over");
}

void game_won() {
  matrix.setCursor(0,0);
  matrix.setTextSize(1);
  matrix.print("You");
  matrix.setCursor(0, 8);
  matrix.print("Won");
}

void print_boss() {
  matrix.setCursor(0,0);
  matrix.setTextSize(1);
  matrix.print("boss");
}

void print_boss_lives() {
  matrix.setCursor(0,0);
  matrix.setTextSize(1);
  matrix.print("Boss");
  matrix.setCursor(0,8);
  matrix.print("Lives");
  delay(1000);
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(0, 0);
  matrix.print("-----");
  matrix.setCursor(10, 5);
  matrix.print("50");
  matrix.setCursor(0, 10);
  matrix.print("-----");
}

void print_menu(int potentiometerVal) {
  if (potentiometerVal > 512) {
    matrix.setCursor(0, 0);
    matrix.setTextColor(BLUE.to_333());
    matrix.print("Basic");
    matrix.setCursor(0, 8);
    matrix.setTextColor(WHITE.to_333());
    matrix.print("Boss");
  }
  else {
    matrix.setCursor(0, 0);
    matrix.setTextColor(WHITE.to_333());
    matrix.print("Basic");
    matrix.setCursor(0, 8);
    matrix.setTextColor(RED.to_333());
    matrix.print("Boss");
  }
}