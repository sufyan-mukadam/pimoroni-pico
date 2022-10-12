#include <math.h>
#include <cstdlib>
#include <string.h>
#include <time.h>
#include <cstdio>

#include "pico/stdlib.h"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/gfx_pack/gfx_pack.hpp"
#include "drivers/st7567/st7567.hpp"
#include "drivers/button/button.hpp"

using namespace pimoroni;
SPIPins pins = {PIMORONI_SPI_DEFAULT_INSTANCE, 17, SPI_DEFAULT_SCK, SPI_DEFAULT_MOSI, PIN_UNUSED, 16, SPI_BG_FRONT_PWM};
ST7567 st7567(128, 64, pins);
PicoGraphics_Pen1Bit graphics(st7567.width, st7567.height, nullptr);



Button button_a(gfx_pack::A);
Button button_b(gfx_pack::B);
Button button_x(gfx_pack::X);
Button button_y(gfx_pack::Y);

// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}

int main() {
  st7567.set_backlight(255);

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    uint16_t pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 10; i++) {
    pt shape;
    shape.x = rand() % graphics.bounds.w;
    shape.y = rand() % graphics.bounds.h;
    shape.r = (rand() % 10) + 3;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = rand() % 14;
    shapes.push_back(shape);
  }

  Point text_location(0, 0);

  //Pen BG = graphics.create_pen(120, 40, 60);
  //Pen WHITE = graphics.create_pen(255, 255, 255);

  while(true) {
    if(button_a.raw()) text_location.x -= 1;
    if(button_b.raw()) text_location.x += 1;

    if(button_x.raw()) text_location.y -= 1;
    if(button_y.raw()) text_location.y += 1;
  
    graphics.set_pen(0);
    graphics.clear();

    for(auto &shape : shapes) {
      shape.x += shape.dx;
      shape.y += shape.dy;
      if((shape.x - shape.r) < 0) {
        shape.dx *= -1;
        shape.x = shape.r;
      }
      if((shape.x + shape.r) >= graphics.bounds.w) {
        shape.dx *= -1;
        shape.x = graphics.bounds.w - shape.r;
      }
      if((shape.y - shape.r) < 0) {
        shape.dy *= -1;
        shape.y = shape.r;
      }
      if((shape.y + shape.r) >= graphics.bounds.h) {
        shape.dy *= -1;
        shape.y = graphics.bounds.h - shape.r;
      }

      graphics.set_pen(shape.pen);
      graphics.circle(Point(shape.x, shape.y), shape.r);

    }

    graphics.set_pen(15);
    graphics.text("Hello World", text_location, 320);

    // update screen
    st7567.update(&graphics);
    sleep_ms(1000/15);
  }

    return 0;
}
