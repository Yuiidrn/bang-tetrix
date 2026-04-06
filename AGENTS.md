# BanG Tetrix! - Agent Coding Guidelines

This document provides guidelines for agents working on the BanG Tetrix! codebase, a Qt 6 C++ Tetris-style game.

## Project Overview

- **Framework**: Qt 6.8.0 (MinGW 64-bit)
- **C++ Standard**: C++17
- **Build System**: Qt .pro files (qmake)
- **Platform**: Windows

---

## Build Commands

### Qt Creator (Recommended)
1. Open `bang-tetrix.pro` in Qt Creator
2. Select Kit: `Desktop Qt 6.8.0 MinGW 64-bit`
3. Build: `Ctrl+B`
4. Run: `Ctrl+R`
5. Clean: `Build > Clean All` then rebuild

### Command Line Build
```bash
# Navigate to project directory
cd "D:\C++_document\QT\BanG_Tetrix!\bang-tetrix"

# Create build directory (if needed)
mkdir -p build/Release

# Run qmake
qmake bang-tetrix.pro -spec win32-g++ "CONFIG+=release"

# Build
mingw32-make -j4

# Clean
mingw32-make clean
```

### Visual Studio / VSCode
- Uses `.vscode/launch.json` for debugging
- Configuration: `Desktop Qt 6.8.0 MinGW 64-bit`
- Debug output: `build/Debug/outDebug`

### Build Directories
- Debug: `build/Desktop_Qt_6_8_0_MinGW_64_bit-Debug/`
- Release: `build/Desktop_Qt_6_8_0_MinGW_64_bit-Release/`
- Profile: `build/Desktop_Qt_6_8_0_MinGW_64_bit-Profile/`

### Note on Testing
This codebase does **not** have unit tests. Manual testing via Qt Creator or command-line execution is required.

---

## Code Style Guidelines

### File Organization

```
Project Root/
├── main.cpp             # Application entry
├── ui/                  # Main window, game widget, dialogs, *.ui
├── gameplay/            # Block logic/match/move, backgroundInfo, blockInfo
├── score/               # Score input UI; score/scoreTable/ = rankings + ScoreManager
├── legacy/              # Sources not in bang-tetrix.pro (reference only)
├── img/                 # Image resources
├── sound/               # Audio resources
├── build/               # Build artifacts (do not edit)
└── bang-tetrix.pro      # Qt project file (INCLUDEPATH: ui, gameplay, score, score/scoreTable)
```

### Header Organization
```cpp
#ifndef CLASSNAME_H
#define CLASSNAME_H

// 1. Qt includes
#include <QWidget>
#include <QTimer>

// 2. Standard library includes
#include <vector>
#include <algorithm>

// 3. Project includes (bang-tetrix.pro sets INCLUDEPATH for ui / gameplay / score / score/scoreTable)
#include "backgroundInfo.h"
#include "scoretable.h"

// 4. Forward declarations (for circular dependency prevention)
class GameWidget;  // NOT #include "game.h" here

namespace Ui { class Widget; }

class MyClass : public QWidget
{
    Q_OBJECT
    // ... class body
};

#endif // CLASSNAME_H
```

### C++ Style

#### Naming Conventions
| Element | Convention | Example |
|---------|-----------|---------|
| Classes | PascalCase | `GameWidget`, `BlockInfo` |
| Structs | PascalCase | `Block_info` |
| Enums | PascalCase | `Band_name`, `Direction` |
| Enum values | PascalCase | `Poppin_Party`, `Afterglow` |
| Member variables | camelCase with `m_` prefix | `m_score`, `gameTimer` |
| Global variables | camelCase | `availableGeometry` |
| Constants | camelCase | `defaultWidth`, `AREA_ROW` |
| Functions | camelCase | `initMenu()`, `startGame()` |
| Parameters | camelCase | `QWidget *parent` |
| Private/Protected members | camelCase (no prefix) | `score`, `isGameOver` |

#### Class Member Variables Pattern
```cpp
class GameWidget : public QWidget
{
    Q_OBJECT
public:
    void initMenu(Mainmenu *menu);

private:
    Ui::Widget *ui;              // Qt UI pointer (no prefix)
    Mainmenu *menu;              // Other widget pointers (no prefix)
    int score;                   // Simple types (no prefix)
    QTimer *gameTimer;           // Pointers get no special prefix
    bool isGameOver;             // Boolean flags
};
```

#### Qt Specific Patterns

**Signals and Slots:**
```cpp
class GameWidget : public QWidget
{
    Q_OBJECT
signals:
    void MarinaAnimation();           // Signal declaration
    void gameScoreChanged(int score);

public slots:
    void playStartMA();              // Public slot
    void onMTimeOut();               // Auto-connect slots use on prefix

private slots:
    void switchToGame();
```

**Forward Declarations (avoid circular includes):**
```cpp
// In header file - use forward declaration
class GameWidget;
class Mainmenu;

// In .cpp file - include the full header
#include "game.h"
```

**Constructor Initialization:**
```cpp
GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , score(0)
    , isGameOver(false)
{
    // Constructor body
}
```

### Formatting Rules

#### Braces
```cpp
// Function body - on same line
void GameWidget::InitGame() {
    if (condition) {
        // code
    } else {
        // code
    }
}

// Enums/structs - same line
enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SPACE
};
```

#### Indentation
- Use **4 spaces** for indentation (no tabs)
- Qt Creator default settings

#### Line Length
- Target **< 120 characters** per line
- Break long function calls at logical points

#### Spacing
```cpp
// Function calls - no spaces inside parens
initMenu(menu);

// Control flow - space after keyword
if (condition) {
    while (condition) {
        for (int i = 0; i < 10; i++) {
            switch (value) {
            case 1:
                break;
            }
        }
    }
}

// Operators - spaces around
int sum = a + b;
if (a == b && c != d)

// References and pointers - attach to type
void initMenu(Mainmenu *menu);
int *ptr;
```

### Code Patterns

#### Inline Helper Functions
```cpp
// Small utility functions can be inline
inline void block_cpy(Block_info &dblock, Block_info &sblock) { 
    dblock = sblock; 
}

// Delay helper
inline void Sleep(unsigned int msec) {
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < msec)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
}
```

#### Lambda Expressions
```cpp
// Qt5-style connections with lambdas
connect(gameTimer, &QTimer::timeout, this, [=](){
    BlockTranslate(DOWN);
});

// Capturing [this] when needed
connect(fadeAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
    currentOpacity = value.toReal();
    update();
});
```

#### Member Function Pointers (for signals)
```cpp
void(GameWidget::*MarinaAnimation0)() = &GameWidget::MarinaAnimation;
void(GameWidget::*playStartMA0)() = &GameWidget::playStartMA;
connect(this, MarinaAnimation0, this, playStartMA0);
```

### Error Handling

#### Null Checks
```cpp
if (original.isNull()) {
    return QPixmap();
}

// Pointer checks
if (reply && reply->error() != QNetworkReply::NoError) {
    // handle error
}
```

#### Signal/Slot Error Handling
```cpp
// Network errors - emit signals, use qDebug for logging
if (reply->error() != QNetworkReply::NoError) {
    qDebug() << "Error:" << reply->errorString();
    emit networkError("Failed: " + reply->errorString());
    return;
}
```

---

## Qt-Specific Guidelines

### Memory Management
- Use Qt's parent-child memory model
- Pointers created with `new` in constructors typically have `this` as parent
- UI pointers (`ui`) should be deleted in destructor

```cpp
GameWidget::~GameWidget() { 
    delete ui; 
}
```

### Timer Usage
```cpp
// Create and configure
gameTimer = new QTimer(this);
gameTimer->setInterval(speed_ms);

// Connect
connect(gameTimer, &QTimer::timeout, this, [=](){
    BlockTranslate(DOWN);
});

// Control
gameTimer->start();
gameTimer->stop();
```

### Painter Usage
```cpp
void GameWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Save/restore when transforming
    painter.save();
    painter.translate(x, y);
    painter.rotate(90);
    painter.drawPixmap(...);
    painter.restore();
}
```

---

## Important Notes

### Block Class Duplication
`block.h` and `blockInfo.h` have overlapping content. When modifying:
- `block.h`: Used for simple includes
- `blockInfo.h`: Contains the full `Block_info` class with member function logic

### Game Area Configuration
```cpp
const int AREA_ROW = 10;  // Game area rows
const int AREA_COL = 8;   // Game area columns
const int defaultWidth = 1920;  // Reference screen width
```

### Band Configuration
```cpp
const int BAND_NUM = 9;   // Number of bands
const int CHAR_NUM = 5;    // Members per band
const int BAND_SET[BAND_NUM] = {1, 1, 1, 1, 1, 1, 1, 1, 1}; // Enable/disable bands
```

### Direction Enum
```cpp
enum Direction { UP, DOWN, LEFT, RIGHT, SPACE };
// Movement vectors
const int di[4] = {1, 0, -1, 0};  // row delta
const int dj[4] = {0, -1, 0, 1};  // col delta
```

---

## Resource Files

### Image Resources (resource.qrc)
```qrc
<qresource prefix="/imgs">
    <file>img/ui/bg.png</file>
    <file>img/Roselia/*.png</file>
    <!-- etc -->
</qresource>
```

Access via: `:/imgs/img/ui/bg.png`

### Sound Resources
Access via: `qrc:/sounds/sound/land.wav`

---

## Debugging Tips

1. **qDebug** for logging: `qDebug() << "Value:" << value;`
2. **QMessageBox** for alerts: `QMessageBox::warning(this, "Title", "Message");`
3. Event filter installation blocks input:
```cpp
this->installEventFilter(this);   // Block input
this->removeEventFilter(this);    // Allow input
```
4. Use `Q_FUNC_INFO` to log function entry points
