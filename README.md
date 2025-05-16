# 🎮 V2 Game Engine

V2 is a minimalistic game engine with its own OpenGL renderer, built-in audio engine *(in development)* and scripting language. The project is written from scratch, without using ready-made game engines.

---

## ✨ Features

- 🚀 Cross-platform OpenGL renderer
- 🔊 Audio engine *(in development)*
- 📜 Own scripting language
- 🎯 2D graphics support
- 🧠 Simple architecture
- 🔌 Minimum dependencies

---

## 🖼 Example program

![image](https://github.com/user-attachments/assets/1e8e8703-c37c-4b0d-a5db-19c91aa9ccee)

``` C++
#include <v2/log.hpp>
#include <v2/render/quad.hpp>
#include <v2/render/render.hpp>
#include <v2/window.hpp>

const glm::vec4 background_color(153.0f / 255, 59.0f / 255, 80.0f / 255, 1.0f);

int main() { 
  Window window("V2Example", glm::uvec2(600, 600)); 
  Renderer renderer(glm::uvec2(600, 600)); 

  trace::info("Window created, Renderer created."); 

  Texture eye_texture("some_texture.png"); 
  Quad eye_quad(glm::vec2(300 - 150, 300 - 150), glm::vec2(300, 300), 
  eye_texture); 

  while (window.running()) { 
    eye_quad.angle += 1; 
    window.clear(background_color);
    renderer.draw_quads({eye_quad});
  }
  trace::info("Window closed.");
  return 0;
}
```

---

## 🧪 Script example

```rust
// Function declaration
fn <name>(<param: type>) {
  // function body
}

// Variable declaration
let <name> = <value>;

// Conditional constructs
if <condition> {
  // if the condition is true
} else {
  // otherwise
}

// Loop declaration
while <condition> {
  // as long as the condition is true
}

// Execution pause
wait(<time_in_seconds>);

```

## ⚙️ Build
``` Bash
git clone https://github.com/Suleiman0212/v2.git
cd v2
meson setup build
meson install -C build/
```
