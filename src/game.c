/* 
  Copyright (C) 2010 Gavin Schultz
  
  This file is part of Asterad.
  
  Asterad is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  Asterad is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Asterad.  If not, see <http://www.gnu.org/licenses/>.
*/ 

#include    "game.h"
#include    <windows.h>

SDL_Surface *screen = NULL;
SHIP *p1 = NULL;
ASTEROID *asteroids = NULL;
PRIMITIVESYSTEM *spark_systems = NULL;
PRIMITIVESYSTEM *debris_systems = NULL;
BULLET bullets[MAX_BULLETS];     //TODO: Better suited to some sort of queue or dynamic array, I daresay
int last_bullet;
UFO *ufo = NULL;
int message_timer = 0;
static char* message = NULL;

// Sound mappings and corresponding enumerations
typedef enum
{ 
    FXGameStart, FXGameOver, FXBootup,
    FXAsteroidExplode,  
    FXShipFire, FXShipExplode, FXShipRespawn, FXShip1UP,
    FXUFOFire, FXUFOExplode, FXUFOPartExplode, FXUFOHum, FXUFOPing
} FXType;

static const char *soundmap[] = 
{ 
    "game_start", "game_over", "bootup", 
    "asteroid_explode", 
    "ship_fire", "ship_explode", "ship_respawn", "ship_oneup",
    "ufo_fire", "ufo_explode", "ufo_part_explode", "ufo_hum", "ufo_ping"
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int cmdShow)
//int main(int argc, char *argv[])
{
    SDL_Event event;

    init();

    while (!game.quit)
    {
        start_timer();

        if (SDL_PollEvent(&event))
        {
            handle_event(&event);
        }

        if (!game.paused)
        {
            handle_keystate(p1);
            move_objects();
            detect_collisions();
            draw_scene();
            check_game_status();
        }

        end_frame();
    }

    return 0;
}

void init_graphics()
{
    const SDL_VideoInfo *video_info;
    static int best_hres;
    static int best_vres;
    uint32_t video_flags = 0;

    video_info = SDL_GetVideoInfo();
    if (video_info == NULL)
    {
        trace("Could not obtain SDL video info.");
        exit(1);
    }

    if (video_info->hw_available)
    {
        video_flags |= SDL_HWSURFACE;
    }
    else
    {
        video_flags |= SDL_SWSURFACE;
    }

    if (best_hres == 0 && best_vres == 0)
    {
        best_hres = video_info->current_w;
        best_vres = video_info->current_h;
    }

    if (game.fullscreen)
    {
        SDL_ShowCursor(SDL_DISABLE);
        video_flags |= SDL_FULLSCREEN;
        game.hRes = best_hres;
        game.vRes = best_vres;
    }
    else
    {
        SDL_ShowCursor(SDL_ENABLE);
        game.hRes = DEFAULT_HRES;
        game.vRes = DEFAULT_VRES;
    }

    game.bitdepth = DEFAULT_COLORDEPTH;
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    screen = SDL_SetVideoMode(game.hRes, game.vRes, game.bitdepth, video_flags | SDL_OPENGL | SDL_ANYFORMAT | SDL_RESIZABLE);
    if (screen == NULL)
    {
        trace("SDL video mode setting failure: %s", SDL_GetError());
        exit(1);
    }

    init_gl(game.hRes, game.vRes, X_AXIS, Y_AXIS);
}

void init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
        trace("SDL initialization failed: %s.", SDL_GetError());
        exit(1);
    }

    srand((unsigned int)time(NULL));

    set_fps((int)FPS);
    set_axes(X_AXIS, Y_AXIS);

    atexit(shutdown);

    game.fullscreen = DEFAULT_FULLSCREEN; 
    game.show_bounding_boxes = 0;
    game.time_mult = 1.0f;

    init_graphics();
    init_sound();
    init_ships();
    load_sounds(RESFOLDER, soundmap, sizeof(soundmap) / sizeof(char*));

    // Configure key bindings. TODO: make these configurable
    game.keymap.p1_left = SDLK_a;
    game.keymap.p1_right = SDLK_d;
    game.keymap.p1_thrust = SDLK_w;
    game.keymap.p1_fire = SDLK_SPACE;
    game.keymap.p1_bomb = SDLK_s;

    reset();
}

void build_demo_objects()
{
    int i;
    ASTEROID *ast;

    clear_asteroids(&asteroids);
    for (i=0; i<5; i++)
    {
        ast = create_asteroid(ASTEROID_SPEED / FPS, (float)(rand() % 360), ASTEROID_RADIUS, 2); 
        set_primitive_position(&ast->primitive, (float)(rand() % (int)(X_AXIS * 2)), (float)(rand() % (int)(Y_AXIS * 2)));
        add_asteroid(&asteroids, ast);
    }
}

void shutdown()
{
    cleanup();
    SDL_FreeSurface(screen);
    shutdown_sound();
}

void handle_event(SDL_Event *event)
{
    SDLKey key = SDLK_CLEAR;

    if (event == NULL)
        return;

    // These events may be used at any time, even when the game is paused
    switch(event->type)
    {
        case SDL_QUIT:
            game.quit = 1;
            break;
        case SDL_KEYDOWN:
            key = event->key.keysym.sym;
            switch(key)
            {
                case SDLK_q:
                case SDLK_ESCAPE:
                    game.quit = 1;
                    break;
                case SDLK_p:
                    game.paused = (game.paused ? 0 : 1);
                    break;
                case SDLK_F2:
                    toggle_sound();
                    break;
                case SDLK_F3:
                    game.show_bounding_boxes = (game.show_bounding_boxes ? 0 : 1);
                    break;
                case SDLK_MINUS:
                    if (game.time_mult > 0.1f)
                    {
                        game.time_mult -= 0.1f;
                        set_time_mult(game.time_mult);
                    }
                    break;
                case SDLK_EQUALS:
                    if (game.time_mult < 1.0f)
                    {
                        game.time_mult += 0.1f;
                        set_time_mult(game.time_mult);
                    }
                    break;
                case SDLK_RETURN:
                    if (event->key.keysym.mod & KMOD_ALT)
                    {
                        game.fullscreen = (game.fullscreen ? 0 : 1);
                        init_graphics();
                    }
                    break;
            }
            break;
        case SDL_VIDEORESIZE:
            resize_window(event->resize.w, event->resize.h, X_AXIS, Y_AXIS);
            break;
    }

    // These events may only be used when the game is not paused
    if (!game.paused)
    {
        if (key == game.keymap.p1_fire)
        {
            ship_fire(p1);
        }
        switch(key)
        {
            case SDLK_r:
                reset();
                break;
            case SDLK_s:
                if (game.state == GAME_NOT_STARTED || game.state == DISPLAY_HIGH_SCORES || game.state == GAME_OVER)
                    start();
                else if (p1->dead && p1->lives > 0)
                    respawn(p1);
                break;
            case SDLK_o:
                hit_all_asteroids();
                break;
        }
    }

    // These events may only be used when entering a high score
    if (game.state == ENTER_HIGH_SCORE)
    {
        if (key == game.keymap.p1_left)
        {
            // move character entry left
            start_hiscore_scroll(SCROLL_LEFT);
        }
        else if (key == game.keymap.p1_right)
        {
            // move character entry right
            start_hiscore_scroll(SCROLL_RIGHT);
        }
        else if (key == SDLK_s)
        {
            // enter character
            enter_hiscore_character();
        }
    }
}

void handle_keystate(SHIP *ship)
{
    uint8_t *keystate = NULL;
    keystate = SDL_GetKeyState(NULL);
    if (ship != NULL)
    {
        if (keystate[game.keymap.p1_left])
        {
            ship->primitive.rotation = (SHIP_TURN_SPEED / FPS);
        }
        else if (keystate[game.keymap.p1_right])
        {
            ship->primitive.rotation = (-SHIP_TURN_SPEED / FPS);
        }
        else
        {
            ship->primitive.rotation = 0;
        }

        ship->thrusting = keystate[game.keymap.p1_thrust];
    }

    if (game.state == ENTER_HIGH_SCORE)
    {
        if (!keystate[game.keymap.p1_left] && !keystate[game.keymap.p1_right])
        {
            stop_hiscore_scroll();
        }
        manage_hiscore_state();
    }
}

void move_objects()
{
    int i;
    ASTEROID *ast = asteroids;

    move_ship(p1);

    while (ast != NULL)
    {
        move_primitive(&ast->primitive);
        ast = ast->next;
    }

    // Move bullets
    for (i=0;i<MAX_BULLETS;i++)
    {
        if (bullets[i].range > 0.0f)
        {
            move_point(&bullets[i].point, bullets[i].xVel * game.time_mult, bullets[i].yVel * game.time_mult);
            bullets[i].range -= bullets[i].speed * square(game.time_mult);
        }
    }

    // Move all primitive systems
    move_primitive_systems(&spark_systems);
    move_primitive_systems(&debris_systems);

    // Move UFO
    for (i=0; ufo != NULL && i<ufo->parts_count; i++)
    {
        if (ufo->parts[i].lifetime)
        {
            move_primitive(&ufo->parts[i]);
            if (!ufo->parts[i].lifetime)
            {
                hit_ufo(&ufo, &ufo->parts[i], NULL);
            }
        }
    }
}

void move_ship(struct Ship *ship)
{
    if (ship == NULL || ship->dead)
        return;

    if(ship->thrusting)
    {
        ship->primitive.xVel += (ship->acceleration / (FPS * FPS)) * cosf(to_radians(ship->primitive.angle)) * game.time_mult;
        ship->primitive.yVel += (ship->acceleration / (FPS * FPS)) * sinf(to_radians(ship->primitive.angle)) * game.time_mult;

        ship->primitive.speed = (float)sqrt(square(ship->primitive.xVel) + square(ship->primitive.yVel));
        if (ship->primitive.speed > (SHIP_MAX_SPEED / FPS) * game.time_mult)
        {
            ship->primitive.xVel += (ship->primitive.xVel / ship->primitive.speed) * (((SHIP_MAX_SPEED / FPS) * game.time_mult) - ship->primitive.speed);
            ship->primitive.yVel += (ship->primitive.yVel / ship->primitive.speed) * (((SHIP_MAX_SPEED / FPS) * game.time_mult) - ship->primitive.speed);
        } 

        ship_thrust(ship, 1);
    }
    else
    {
        ship_thrust(ship, 0);
    }

    move_primitive(&ship->primitive);

    ship->flare.xVel = ship->primitive.xVel;
    ship->flare.yVel = ship->primitive.yVel;
    ship->flare.angle = ship->primitive.angle;
    ship->flare.rotation = ship->primitive.rotation;
    move_primitive(&ship->flare);
}

void draw_scene()
{
    char score[30];
    int i;
    ASTEROID *ast = asteroids;
    PRIMITIVESYSTEM *ps = NULL;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    // draw axes
    glLineWidth(0.5);
    glBegin(GL_LINES);
    {
        glVertex2f(0.0, -Y_AXIS);
        glVertex2f(0.0, Y_AXIS);
        glVertex2f(-X_AXIS, 0.0);
        glVertex2f(X_AXIS, 0.0);
    }
    glEnd();

    if (game.state == GAME_NOT_STARTED)
    {
        set_vertical_align(VALIGN_NONE);
        set_text_dimensions(18.0f, 2.0f);
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, Y_AXIS / 3);
        set_horizontal_align(HALIGN_CENTER);
        draw_text("ASTERAD");

        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, Y_AXIS / 7);
        set_text_dimensions(7.0f, 0.75f);
        draw_text("PRESS S TO BEGIN");

        set_horizontal_align(HALIGN_LEFT);
        set_text_dimensions(4.0f, 0.75f);
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS + 29.0f);
        draw_text("CONTROLS");
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS + 23.0f);
        draw_text("A      TURN LEFT");
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS + 17.0f);
        draw_text("D      TURN RIGHT");
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS + 11.0f);
        draw_text("W      THRUST");
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS + 5.0f);
        draw_text("SPACE  FIRE");
    }
    else if (game.state == DISPLAY_HIGH_SCORES)
    {
        set_vertical_align(VALIGN_NONE);
        set_horizontal_align(HALIGN_CENTER);
        set_text_dimensions(12.0f, 1.0f);
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, 60);
        draw_text("HIGH SCORES");
        set_text_dimensions(9.0f, 0.75f);
        for (i=0; i<MAX_HISCORES; i++)
        {
            set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, ((4-i) * 11.0f));
            sprintf(score, "%-3s %8d", game.hiscores[i].initials, game.hiscores[i].score); 
            draw_text(score);
        }
    }
    else if (game.state == GAME_OVER || game.state == ENTER_HIGH_SCORE)
    {
        set_vertical_align(VALIGN_CENTER);
        set_horizontal_align(HALIGN_CENTER);
        set_text_dimensions(12.0f, 0.75f);
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS);
        draw_text("GAME OVER");

        if (game.state == ENTER_HIGH_SCORE)
        {
            set_vertical_align(VALIGN_NONE);
            set_text_dimensions(6.0f, 0.75f);
            set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS / 5.0f);
            draw_text("NEW HIGH SCORE. ENTER INITIALS");
            set_text_dimensions(9.0f, 0.75f);
            set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, (-Y_AXIS / 5.0f) - 12.5f);
            draw_text(get_current_hiscore_name());
        }
    }

    if (game.message_timer > 0)
    {
        set_text_dimensions(5.0f, 0.75f);
        set_text_boundaries(-X_AXIS, Y_AXIS, X_AXIS, -Y_AXIS);
        set_horizontal_align(HALIGN_CENTER);
        set_vertical_align(VALIGN_CENTER);
        draw_text(game.message);
    }

    // Draw asteroids
    while (ast != NULL)
    {
        draw_asteroid(ast);
        ast = ast->next;
    }

    draw_ship(p1);
    draw_scores();
    draw_lives(p1);

    draw_primitive_systems(&spark_systems);
    draw_primitive_systems(&debris_systems);

    if (ufo != NULL)
    {
        for (i=0; i<ufo->parts_count; i++)
        {
            draw_primitive(&ufo->parts[i]);
        }
    }

    // Draw bullets
    for (i=0;i<MAX_BULLETS;i++)
    {
        if (bullets[i].range > 0.0f)
        {
            if (bullets[i].owner_type == SHIP_UFO)
                glPointSize(2.0f);
            else
                glPointSize(1.0f);
            glBegin(GL_POINTS);
            glVertex2f(bullets[i].point.x, bullets[i].point.y);
            glEnd();
        }
    }
    glPointSize(1.0f);

    glPopMatrix();

    SDL_GL_SwapBuffers();
}

void draw_primitive_systems(PRIMITIVESYSTEM **list)
{
    int i;
    PRIMITIVESYSTEM *ps = *list;
    while (ps != NULL)
    {
        for(i=0; i<ps->size; i++)
        {
            draw_primitive(&ps->primitives[i]);

        }
        ps = ps->next;
    }
}

void draw_primitive(PRIMITIVE *p)
{
    int j;

    if (!p->lifetime || p->invisible)
        return;

    switch (p->size)
    {
        case 1:
            glBegin(GL_POINTS);
            break;
        case 2:
            glBegin(GL_LINES);
            break;
        default:    // Polygons
            glBegin(GL_LINE_LOOP);
            break;
    }

    for (j=0; j<p->size; j++)
    {
        glVertex2f(p->points[j].x, p->points[j].y);
    }
    glEnd();

    // Draw bounding box
    if (game.show_bounding_boxes)
    {
        glPushMatrix();
        glTranslatef(p->bounding_box.x, p->bounding_box.y, 0.0f);
        glBegin(GL_QUADS);
        {
            glVertex2f(0.0f, 0.0f);
            glVertex2f(0.0f, p->bounding_box.h);
            glVertex2f(p->bounding_box.w, p->bounding_box.h);
            glVertex2f(p->bounding_box.w, 0.0f);
        }
        glEnd();
        glPopMatrix();
    }
}

void draw_lives(SHIP *ship)
{
    int i, j;

    if (ship == NULL)
        return;

    glPushMatrix();
    glTranslatef(-X_AXIS, -Y_AXIS, 0.0f);
    for (i=1; i<ship->lives; i++)
    {
        glTranslatef(0.0f, ship->h + ship->h/5, 0.0f);
        glBegin(GL_POLYGON);
        for (j=0; j<(sizeof(ship_geometry)/sizeof(float)); j+=2)
            glVertex2f(ship_geometry[j], ship_geometry[j+1]);
        glEnd();
    }
    glPopMatrix();
}

void draw_ship(SHIP *ship)
{
    if (ship == NULL || ship->dead)
        return;

    draw_primitive(&ship->primitive);
    draw_primitive(&ship->flare);
}

void draw_scores()
{
    char score[10];

    if (game.state == GAME_NOT_STARTED || game.state == DISPLAY_HIGH_SCORES)
        return;

    if (p1 != NULL)
    {
        set_text_dimensions(5.0f, 0.75f);
        set_text_boundaries(-X_AXIS, Y_AXIS - 1.0f, X_AXIS, -Y_AXIS);
        set_horizontal_align(HALIGN_LEFT);
        set_vertical_align(VALIGN_TOP);
        sprintf(score, "%06d",p1->score);
        draw_text(score);
    }
}

void draw_asteroid(ASTEROID *asteroid)
{
    glColor3f(asteroid->color.r, asteroid->color.g, asteroid->color.b);
    draw_primitive(&asteroid->primitive);
}

void ship_fire(struct Ship *ship)
{
    struct Bullet *b;

    if (ship == NULL || ship->dead)
        return;

    last_bullet = (last_bullet++) % MAX_BULLETS;
    b = &bullets[last_bullet];
    b->point.x = ship->primitive.points[1].x;   // i.e. from the ship's nose
    b->point.y = ship->primitive.points[1].y;
    b->speed = BULLET_SPEED / FPS;
    b->angle = ship->primitive.angle;
    b->xVel = b->speed * cosf(to_radians(b->angle));
    b->yVel = b->speed * sinf(to_radians(b->angle));
    b->range = BULLET_RANGE;
    b->owner_type = SHIP_PLAYER;
    b->owner.ship = ship;

    play_sound(FXShipFire);
}

void ufo_fire(UFO *ufo)
{
    BULLET *b;
    FPOINT *muzzle;

    if (ufo == NULL || ufo->broken)
        return;

    last_bullet = (last_bullet++) % MAX_BULLETS;
    b = &bullets[last_bullet];

    // Find appropriate target
    if (p1 != NULL && !p1->dead)
    {
        b->angle = to_degrees(atan2f(p1->primitive.centroid.y - ufo->parts[0].centroid.y , p1->primitive.centroid.x - ufo->parts[0].centroid.x));
        b->angle += rand_sign() * randf(35.0f);
    }
    else
    {
        b->angle = randf(360.0f);
    }

    muzzle = &ufo->parts[0].points[(fabs(b->angle) < 90.0f ? 3 : 0)];
    b->point.x = muzzle->x;
    b->point.y = muzzle->y;
    b->speed = UFO_BULLET_SPEED / FPS;
    b->xVel = b->speed * cosf(to_radians(b->angle));
    b->yVel = b->speed * sinf(to_radians(b->angle));
    b->range = UFO_BULLET_RANGE;
    b->owner_type = SHIP_UFO;
    b->owner.ufo = ufo;

    ufo->reload_timer = UFO_RELOAD_TIME;

    play_sound(FXUFOFire);
}

void detect_collisions()
{
    int i;
    int b;
    int bullet_asteroid_collision = 0;
    int ufo_asteroid_collision = 0;
    ASTEROID *ast = asteroids;

    if (p1 != NULL && !p1->dead)
    {
        // Detect player / asteroid collisions
        while (ast != NULL)
        {
            if (primitives_colliding(&p1->primitive, &ast->primitive))
            {
                hit_ship(p1);
                hit_asteroid(ast, NULL);
                break;
            }

            ast = ast->next;
        }

        // Detect player / UFO collisions
        if (ufo != NULL)
        {
            for (i=0; i<UFO_COLLIDABLE_PRIMITIVES; i++)
            {
                if (ufo->parts[i].lifetime && primitives_colliding(&p1->primitive, &ufo->parts[i]))
                {
                    hit_ship(p1);
                    hit_ufo(&ufo, &ufo->parts[i], NULL);
                    break;
                }
            }
        }
    }

    // Detect UFO / asteroid collisions
    if (ufo != NULL)
    {
        ast = asteroids;
        while (ast != NULL && !ufo_asteroid_collision)
        {
            for (i=0; i<UFO_COLLIDABLE_PRIMITIVES && !ufo_asteroid_collision; i++)
            {
                if (ufo->parts[i].lifetime && primitives_colliding(&ast->primitive, &ufo->parts[i]))
                {
                    hit_asteroid(ast, NULL);
                    hit_ufo(&ufo, &ufo->parts[i], NULL);
                    ufo_asteroid_collision = 1;
                }
            }

            ast = ast->next;
        }
    }

    for (b=0;b<MAX_BULLETS;b++)
    {
        bullet_asteroid_collision = 0;

        if (bullets[b].range <= 0.0f)   // Don't bother with expired bullets
            continue;

        // Asteroid / bullet collision
        ast = asteroids;
        while (ast != NULL)
        {
            if (primitive_contains_point(&ast->primitive, &bullets[b].point))
            {
                hit_asteroid(ast, &bullets[b]);
                break;
            }
            ast = ast->next;
        }

        // Bullet / ufo collisions
        if (bullets[b].owner_type == SHIP_PLAYER && ufo != NULL && ufo->parts != NULL)
        {
            for (i=0; i<UFO_COLLIDABLE_PRIMITIVES; i++)
            {
                if (ufo->parts[i].lifetime && primitive_contains_point(&ufo->parts[i], &bullets[b].point))
                {
                    hit_ufo(&ufo, &ufo->parts[i], &bullets[b]);
                    break;
                }
            }
        }

        // Bullet / player collisions
        if (bullets[b].owner_type == SHIP_UFO && p1 != NULL && !p1->dead)
        {
            if (primitive_contains_point(&p1->primitive, &bullets[b].point))
            {
                hit_ship(p1);
                bullets[b].range = 0.0f;
            }
        }
    }
}

void hit_all_asteroids()
{
    ASTEROID *ast = asteroids;
    ASTEROID *temp = NULL;
    while (ast != NULL)
    {
        temp = ast->next;
        hit_asteroid(ast, NULL);
        ast = temp;
    }
}

int calculate_hit_score(BULLET *bullet, SHIP *ship, ASTEROID *ast)
{
    int score;

    if (bullet == NULL || ship == NULL || ast == NULL)
        return 0;

    // Base points score based on asteroid size; larger is less points
    score = (2 - ast->life) * 500 + 1000;

    // Bonus points if the ship is moving, faster is more points
    score += (int)(ship->primitive.speed * FPS * 10.0f);
    
    // Bonus points if the asteroid is extremely close or extremely far
    if (BULLET_RANGE - bullet->range <= (BULLET_RANGE / 8.0f))
    {
        score += 400;
    }
    else if (bullet->range <= (BULLET_RANGE / 6.0f))
    {
        score += 250;
    }

    score = (50 * (int)(score / 50.0f)) + (game.level * 50); 

    return score;
}

void hit_ship(SHIP *ship)
{
    int i;

    PRIMITIVESYSTEM *ship_debris = NULL;
    PRIMITIVESYSTEM *sparks = NULL;

    play_sound(FXShipExplode);

    ship_debris = create_debris(&ship->primitive);
    add_primitive_system(&debris_systems, ship_debris);

    sparks = create_sparks(&ship->primitive, 250, FROM_CENTER, 0.0f, 360.0f, ship->primitive.speed + (SHIP_MAX_SPEED / 2.0f / FPS)); // ship->primitive.speed);
    for (i=0; i<sparks->size; i++)
    {
        sparks->primitives[i].xVel += ship->primitive.xVel * cosf(to_radians(sparks->primitives[i].angle)); 
        sparks->primitives[i].yVel += ship->primitive.yVel * sinf(to_radians(sparks->primitives[i].angle)); 
    }
    add_primitive_system(&spark_systems, sparks);

    ship->dead = 1;
    ship_thrust(ship, 0);

    if (ship->lives == 1)
        game_over();
}

void game_over()
{
    p1->lives = 0;
    if (p1->score >= game.hiscores[8].score)
    {
        init_hiscore();
        set_game_state(ENTER_HIGH_SCORE);
    }
    else
    {
        set_game_state(GAME_OVER);
    }
    play_sound(FXGameOver);
}

void respawn(SHIP *ship)
{
    if (ship == NULL || ship->lives <= 0)
        return;

    ship->lives--;
    ship->dead = 0;
    set_primitive_position(&ship->primitive, 0.0f, 0.0f);
    set_primitive_position(&ship->flare, 0.0f, 0.0f);
    set_primitive_angle(&ship->primitive, 0.0f);
    set_primitive_angle(&ship->flare, 0.0f);
    ship->primitive.speed = ship->primitive.xVel = ship->primitive.yVel = 0.0f;

    play_sound(FXShipRespawn);
}

void hit_asteroid(ASTEROID *ast, BULLET *bullet)
{
    SHIP *ship_owner;

    PRIMITIVESYSTEM *sparks = NULL;
    PRIMITIVESYSTEM *asteroid_debris = NULL;

    if (ast == NULL)
        return;

    if (bullet != NULL)
    {
        if (bullet->owner_type == SHIP_PLAYER && !bullet->owner.ship->dead)
        {
            ship_owner = bullet->owner.ship;
            increment_score(ship_owner, calculate_hit_score(bullet, ship_owner, ast));
        }
        bullet->range = 0.0f;
    }

    // Add & initialise debris particles
    sparks = create_sparks(&ast->primitive, 25 + (rand() % 25), FROM_CENTER, 30.0f, 100.0f, (BULLET_SPEED / FPS / 4.0f));
    add_primitive_system(&spark_systems, sparks);

    if (ast->life == 0)
    {
        asteroid_debris = create_debris(&ast->primitive);
        add_primitive_system(&debris_systems, asteroid_debris);
    }

    play_sound(FXAsteroidExplode);
    break_asteroid(&asteroids, ast);
}

void hit_ufo(UFO **ufop, PRIMITIVE *part, BULLET *bullet)
{
    int i, p, score = 0;
    PRIMITIVESYSTEM *sparks = NULL;
    PRIMITIVESYSTEM *part_debris = NULL;
    UFO *ufo = *ufop;
    SHIP *ship_owner = NULL;

    if (ufo == NULL || part == NULL)
        return;

    if (bullet != NULL)
    {
        if (bullet->owner_type == SHIP_PLAYER)
        {
            ship_owner = bullet->owner.ship;
            increment_score(ship_owner, (ufo->broken ? 2000 : 5000));
        }
        bullet->range = 0.0f;
    }
    
    if (ufo->broken)
    {
        sparks = create_sparks(part, 1 + (int)(part->area) * 4, FROM_AREA, 0.0f, 55.0f, (BULLET_SPEED / FPS / 2.0f));
        add_primitive_system(&spark_systems, sparks);

        part_debris = create_debris(part);
        add_primitive_system(&debris_systems, part_debris);
        if (sparks->size > 20)
            play_sound(FXUFOPartExplode);
    }
    else
    {
        play_sound(FXUFOExplode);
        for (i=0; i<2; i++)
        {
            sparks = create_sparks(part, (1 + (int)ufo->size) * 40, FROM_AREA, 0.0f, 20.0f, (BULLET_SPEED / FPS / 4.0f));
            add_primitive_system(&spark_systems, sparks);
            for (p=0; p<sparks->size; p++)
            {
                sparks->primitives[p].points[0].x = ufo->parts[0].points[(i == 0 ? 0 : 3)].x;
                sparks->primitives[p].points[0].y = ufo->parts[0].points[(i == 0 ? 0 : 3)].y;
                sparks->primitives[p].angle = (i == 0 ? 180.0f : 0.0f) + (rand_sign() * randf(10.0f));
                sparks->primitives[p].xVel = part->xVel / (1.0f + randf(5.0f)) + sparks->primitives[p].speed * cosf(to_radians(sparks->primitives[p].angle)); 
                sparks->primitives[p].yVel = part->yVel / (1.0f + randf(5.0f)) + sparks->primitives[p].speed * sinf(to_radians(sparks->primitives[p].angle)); 
            }
        }
    }

    break_ufo(ufop, part);
    if (*ufop == NULL)
        reset_ufo_timer();
}

void start()
{
    set_game_state(GAME_PLAYING);
    game.level = 1;
    p1 = create_ship(0.0f, 0.0f);
    init_level();
    play_sound(FXGameStart);
}

void reset()
{
    cleanup();
    play_sound(FXBootup);
    build_demo_objects();
    reset_ufo_timer();
    read_high_scores(game.hiscores);
    play_music(BGMUSIC, LOOP_FOREVER);
}

void init_level()
{
    int i;
    float to_player_vector;
    float target_vector;
    ASTEROID *ast;

    srand((unsigned int)(time(NULL) + (rand() % 100)));

    memset(bullets, 0, MAX_BULLETS * sizeof(*bullets));
    last_bullet = 0;

    clear_asteroids(&asteroids);
    clear_primitive_systems(&spark_systems);
    clear_primitive_systems(&debris_systems);

    for (i=0; i<game.level+STARTING_ASTEROIDS-1; i++)
    {
        ast = create_asteroid(ASTEROID_SPEED / FPS, (float)(rand() % 360), ASTEROID_RADIUS, 2);

        // x & y coords moved out so they're not right on top of the player
        set_primitive_position(&ast->primitive, p1->primitive.centroid.x + ((rand() % 2) == 0 ? -1 : 1) * (20 + (rand() % 80)), p1->primitive.centroid.y + ((rand() % 2) == 0 ? -1 : 1) * (20 + (rand() % 80)));

        // also set angle so that they're not coming directly at the ship initially
        to_player_vector = to_degrees(atan2f(p1->primitive.centroid.y - ast->primitive.centroid.y, p1->primitive.centroid.x - ast->primitive.centroid.x));
        target_vector = wrapf(to_player_vector + (rand_sign() * (35.0f + (rand() % 290))), FULL_DEG);
        set_primitive_angle(&ast->primitive, target_vector);
        
        add_asteroid(&asteroids, ast);
    }

    delete_ufo(&ufo);
    reset_ufo_timer();

    display_message("LEVEL %d", game.level);
}

void increment_score(SHIP *ship, int points)
{
    int previous_score = 0;

    if (ship == NULL || points == 0)
        return;

    previous_score = ship->score;
    ship->score += points;
    if ((ship->score / SCORE_1UP) > (previous_score / SCORE_1UP))
    {
        display_message("EXTRA LIFE EARNED");
        ship->lives++;
        play_sound(FXShip1UP);
    }
}

void display_message(const char *message, ...)
{
    char str[200];
    va_list ap;
    va_start(ap, message);
    vsprintf(str, message, ap);
    va_end(ap);
    strncpy(game.message, str, 200);
    game.message_timer = MESSAGE_LIFE;
}

void check_game_status()
{
    if (game.state == GAME_NOT_STARTED && get_frame_ticks() - game.state_start_time > 6500)
    {
        set_game_state(DISPLAY_HIGH_SCORES);
    }
    else if (game.state == DISPLAY_HIGH_SCORES && get_frame_ticks() - game.state_start_time > 6500)
    {
        set_game_state(GAME_NOT_STARTED);
        if (asteroids == NULL)
            build_demo_objects();   // If asteroids have been destroyed while demoing, put them back
    }
    else if (game.state == GAME_PLAYING)
    {
        if (asteroids == NULL && ufo == NULL)
        {
            increment_score(p1, game.level * 10000);
            set_game_state(LEVEL_FINISHED);
        }
    }
    else if (game.state == LEVEL_FINISHED && debris_systems == NULL && spark_systems == NULL) 
    {
        set_game_state(GAME_PLAYING);
        game.level++;
        init_level();
    }
    else if (game.state == GAME_OVER && get_frame_ticks() - game.state_start_time > 3000)
    {
        set_game_state(DISPLAY_HIGH_SCORES);
    }
    else if (game.state == ENTER_HIGH_SCORE)
    {
        if (is_hiscore_done())
        {
            set_high_score(game.hiscores, get_current_hiscore_name(), p1->score);
            write_high_scores(game.hiscores);
            set_game_state(DISPLAY_HIGH_SCORES);
        }
        else
        {
            manage_hiscore_state();
        }
    }

    if (game.message_timer != 0)
        game.message_timer -= get_elapsed_ticks();

    if (game.message_timer < 0)
    {
        game.message[0] = '\0';
        game.message_timer = 0;
    }

    game.ufo_timer -= get_elapsed_ticks();
    if (game.ufo_timer < 0 && game.state != LEVEL_FINISHED && ufo == NULL)
    {
        spawn_ufo();
    }

    if (ufo != NULL)
    {
        ufo->reload_timer -= get_elapsed_ticks();
        ufo->ping_timer -= get_elapsed_ticks();
        if (ufo->reload_timer <= 0)
            ufo_fire(ufo);
        if (ufo->ping_timer <= 0 && !ufo->broken)
        {
            play_sound(FXUFOPing);
            ufo->ping_timer = 2000;
        }
    }
}

void reset_ufo_timer()
{
    game.ufo_timer = ((6 - game.level) + (rand() % 10)) * 1000;
    trace("Resetting UFO timer to %d, based on level %d", game.ufo_timer, game.level);
//    game.ufo_timer = 0;
}

void spawn_ufo()
{
    float x = 0.0f, y = 0.0f;
    float angle = 0.0f;
    float speed = 0.0f;
    int is_left = 0;
    enum UFOSize size;
    int level_modifier;

    level_modifier = (rand() % (game.level ? game.level : 1)) + (rand() % 15);
    if (level_modifier < 3)
        size = UFO_HUGE;
    else if (level_modifier < 12)
        size = UFO_NORMAL;
    else
        size = UFO_SMALL;

    level_modifier = (rand() % (game.level ? game.level : 1)) + (rand() % 15);
    if (level_modifier < 3)
        speed = 17.5f;
    else if (level_modifier < 12)
        speed = 30.0f;
    else
        speed = 55.0f;

    if (p1 != NULL)
        y = p1->primitive.centroid.y + Y_AXIS + (rand() % (int)(Y_AXIS/3.0f));
    else
        y = (float)(rand_sign() * (rand() % (int)Y_AXIS));

    is_left = (rand() % 2);
    angle = (float)((is_left ? 0.0f : 180.0f) + rand_sign() * (rand() % 65));
    x = (is_left ? -X_AXIS : X_AXIS);

    ufo = create_ufo(speed / FPS, angle, UFO_ROOKIE, size, -X_AXIS, y);

    play_sound(FXUFOHum);
}

void set_game_state(GAMESTATE state)
{
    game.state = state;
    game.state_start_time = get_frame_ticks();
}

PRIMITIVESYSTEM *create_debris(PRIMITIVE *p)
{
    PRIMITIVESYSTEM *debris = NULL;
    int i, j;
    float primitive_vector;
    
    if (p == NULL)
        return NULL;

    primitive_vector = to_degrees(atan2f(p->yVel, p->xVel));

    debris = create_primitive_system(p->size, 2);
    for (i=0; i<debris->size; i++)
    {
        j = (i+1) % debris->size;
        debris->primitives[i].points[0].x = p->points[i].x;
        debris->primitives[i].points[0].y = p->points[i].y;
        debris->primitives[i].points[1].x = p->points[j].x;
        debris->primitives[i].points[1].y = p->points[j].y;
        debris->primitives[i].centroid.x = (p->points[i].x + p->points[j].x) / 2.0f;
        debris->primitives[i].centroid.y = (p->points[i].y + p->points[j].y) / 2.0f;
        debris->primitives[i].angle = primitive_vector + (rand_sign() * (randf(55.0f)));
        debris->primitives[i].rotation = randf(5.0f);
        debris->primitives[i].speed = p->speed / (1.0f + randf(10.0f));
        debris->primitives[i].xVel = debris->primitives[i].speed * cosf(to_radians(debris->primitives[i].angle));
        debris->primitives[i].yVel = debris->primitives[i].speed * sinf(to_radians(debris->primitives[i].angle));
        debris->primitives[i].lifetime = (rand() % DEBRIS_LIFE);
    }

    return debris;
}

PRIMITIVESYSTEM *create_sparks(PRIMITIVE *source, int spark_count, enum RelativePrimitivePosition position, float min_angle_range, float max_angle_range, float max_speed)
{
    PRIMITIVESYSTEM *sparks = NULL;
    float source_vector = 0.0f;
    int i;

    if (source == NULL)
        return NULL;

    source_vector = to_degrees(atan2f(source->yVel, source->xVel));

    sparks = create_primitive_system(spark_count, 1);
    for (i=0; i<spark_count; i++)
    {
        if (position == FROM_CENTER)
        {
            sparks->primitives[i].points[0].x = source->centroid.x;
            sparks->primitives[i].points[0].y = source->centroid.y; 
        }
        else
        {
            sparks->primitives[i].points[0].x = source->centroid.x + (rand_sign() * randf(source->centroid.x - source->points[0].x));
            sparks->primitives[i].points[0].y = source->centroid.y + (rand_sign() * randf(source->centroid.y - source->points[0].y));
        }
        sparks->primitives[i].angle = wrapf(source_vector + (rand_sign() * (((min_angle_range/2.0f) + randf((max_angle_range-min_angle_range))/2.0f))), FULL_DEG);
        sparks->primitives[i].speed = max_speed / (1.0f + randf(15.0f));
        sparks->primitives[i].xVel = (source->xVel / (1.0f + randf(5.0f))) + sparks->primitives[i].speed * cosf(to_radians(sparks->primitives[i].angle)); 
        sparks->primitives[i].yVel = (source->yVel / (1.0f + randf(5.0f))) + sparks->primitives[i].speed * sinf(to_radians(sparks->primitives[i].angle)); 
        sparks->primitives[i].lifetime = (rand() % DEBRIS_LIFE);
    }

    return sparks;
}

void cleanup()
{
    set_game_state(GAME_NOT_STARTED);
    delete_ship(&p1);
    clear_asteroids(&asteroids);
    clear_primitive_systems(&spark_systems);
    clear_primitive_systems(&debris_systems);
    delete_ufo(&ufo);
}
