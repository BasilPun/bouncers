#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>
#include <bn_random.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_boostingbird.h"

// Set max/min x position to be the edges of the display
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;

static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;

static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Starting speed of a bouncer
static constexpr bn::fixed BASE_SPEED = 2;

bn::random random;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;

class Bouncer
{
private:
public:
    bn::sprite_ptr sprite;
    bn::fixed x_speed;
    bn::fixed y_speed;
    bn::fixed scale = 1;

    Bouncer(bn::fixed x_start, bn::fixed y_start) : sprite(bn::sprite_items::boostingbird.create_sprite(x_start, y_start)),
                                                    x_speed(random.get_unbiased_fixed(-BASE_SPEED, BASE_SPEED)),
                                                    y_speed(random.get_unbiased_fixed(-BASE_SPEED, BASE_SPEED))
    {
    }

    void update()
    {
        bn::fixed x = sprite.x();
        bn::fixed y = sprite.y();

        // Update x position by adding speed
        x += x_speed;
        y += y_speed;

        // If we've gone off the screen on the right
        if (x > MAX_X)
        {
            // Snap back to screen and reverse direction
            x = MAX_X;
            x_speed *= -1;
            scale = scale - 0.09;
        }
        // If we've gone off the screen on the left
        if (x < MIN_X)
        {
            // Snap back to screen and reverse direction
            x = MIN_X;
            x_speed *= -1;
            scale = scale - 0.09;
        }

        if (y > MAX_Y)
        {
            // Snap back to screen and reverse direction
            y = MAX_Y;
            y_speed *= -1;
            scale = scale - 0.1;
        }
        // If we've gone off the screen on the left
        if (y < MIN_Y)
        {
            // Snap back to screen and reverse direction
            y = MIN_Y;
            y_speed *= -1;
            scale = scale - 0.1;
        }

        if (scale <= 0)
        {
            // delete sprite
            sprite.set_visible(false);
            x_speed = 0;
            y_speed = 0;
            return;
        }

        sprite.set_scale(scale);
        // if x_speed is negative make it pointing left. else pointing right
        if (x_speed <= 0)
        {
            sprite.set_horizontal_flip(false);
        }
        else
        {
            sprite.set_horizontal_flip(true);
        }

        sprite.set_y(y);
        sprite.set_x(x);
    }
};

bn::fixed average_x(bn::vector<bn::sprite_ptr, MAX_BOUNCERS> &sprites)
{
    // Add all x positions together
    bn::fixed x_sum = 0;
    for (bn::sprite_ptr sprite : sprites)
    {
        x_sum += sprite.x();
    }

    bn::fixed x_average = x_sum;

    // Only divide if we have 1 or more
    // Prevents division by 0
    if (sprites.size() > 0)
    {
        x_average /= sprites.size();
    }

    return x_average;
}

void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
    if (bouncers.size() < bouncers.max_size())
    {
        bouncers.push_back(Bouncer(random.get_unbiased_fixed(MIN_X, MAX_X), random.get_unbiased_fixed(MIN_Y, MAX_Y)));
    }
}

int main()
{
    bn::core::init();

    // Sprites and x speeds of bouncers
    // Items with the same index correspond to each other
    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};

    // Bouncer manual = Bouncer(20, 20);

    while (true)
    {
        // if A is pressed add a new bouncer
        if (bn::keypad::a_pressed())
        {
            add_bouncer(bouncers);
        }

        // // if B is pressed print the average to the console
        // if (bn::keypad::b_pressed())
        // {
        //     bn::fixed x_average = average_x(sprites);
        //     BN_LOG("AVERAGE X: ", x_average);
        // }
        for (Bouncer &b : bouncers)
        {
            b.update();
        }

        bn::core::update();
    }
}