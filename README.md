# 🎮 V2 Game Engine

V2 — это минималистичный, но игровой движок с собственным рендерером на OpenGL, встроенным аудио движком *(в разработке)* и скриптовым языком. Проект написан с нуля, без использования готовых игровых движков.

---

## ✨ Возможности

- 🚀 Кроссплатформенный OpenGL-рендерер
- 🔊 Аудио движок *(в разработке)*
- 📜 Собственный скриптовый язык
- 🎯 Поддержка 2D графики
- 🧠 Простая архитектура
- 🔌 Минимум зависимостей

---

## 🖼 Примеры графики

![image](https://github.com/user-attachments/assets/1e8e8703-c37c-4b0d-a5db-19c91aa9ccee)

``` C++
#include "include/log.hpp"
#include "include/quad.hpp"
#include "include/render.hpp"
#include "include/window.hpp"

const glm::vec4 background_color(153.0f / 250, 59.0f / 250, 80.0f / 250, 1.0f);

int main() {
  Window window("V2", 600, 600);
  Renderer renderer(window);

  trace::info("Window created, Renderer created.");

  Texture eye_texture(
      "assets/textures/eye_of_providence/eye_of_providence.png");
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

## 🧪 Пример скрипта

```v2script
// Объявление функции
fn <name>(<param: type>) {
    // тело функции
}

// Объявление переменной
let <name> = <value>;

// Условные конструкции
if <condition> {
    // если условие истинно
} else {
    // иначе
}

// Пауза выполнения
wait(<time_in_seconds>);

```

## ⚙️ Сборка и запуск
``` Bash
git clone https://github.com/Suleiman0212/v2.git
cd v2
meson setup build
meson compile -C build/ && ./build/app
```
