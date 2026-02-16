#include "mainwindow.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QLinearGradient>
#include <QTime>
#include <utility>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    cellSize(22),
    fieldWidth(30),
    fieldHeight(22),
    score(0),
    highScore(0),
    baseSpeed(130),
    wrapMode(true),
    paused(false)
{
    setFixedSize(fieldWidth * cellSize, fieldHeight * cellSize);
    setWindowTitle("Snake");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);

    deathTimer = new QTimer(this);
    connect(deathTimer, &QTimer::timeout, this, &MainWindow::deathAnimation);

    gameState = StartScreen;
}

MainWindow::~MainWindow() {}

void MainWindow::initGame()
{
    snake.clear();
    foods.clear();

    snake.append(QPoint(8, 10));
    snake.append(QPoint(7, 10));
    snake.append(QPoint(6, 10));

    dir = Right;
    nextDir = Right;
    score = 0;
    paused = false;

    spawnFood();
    updateSpeed();
}

void MainWindow::spawnFood()
{
    while (foods.size() < foodCount)
    {
        int x = QRandomGenerator::global()->bounded(fieldWidth);
        int y = QRandomGenerator::global()->bounded(fieldHeight);
        QPoint newFood(x, y);

        if (!snake.contains(newFood) && !foods.contains(newFood))
            foods.append(newFood);
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (gameState == StartScreen)
    {
        QLinearGradient bg(0, 0, width(), height());
        bg.setColorAt(0, QColor(10, 10, 20));
        bg.setColorAt(1, QColor(30, 30, 50));
        painter.fillRect(rect(), bg);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 40, QFont::Bold));
        painter.drawText(rect().adjusted(0, -60, 0, 0), Qt::AlignCenter, "Snake");

        painter.setFont(QFont("Arial", 18));
        painter.drawText(rect().adjusted(0, 40, 0, 0), Qt::AlignCenter, "Press ENTER to Start");
        return;
    }

    QLinearGradient bg(0, 0, width(), height());
    bg.setColorAt(0, QColor(20, 20, 30));
    bg.setColorAt(1, QColor(40, 40, 60));
    painter.fillRect(rect(), bg);

    painter.setPen(QColor(60, 60, 80));
    for (int i = 0; i <= fieldWidth; i++)
        painter.drawLine(i * cellSize, 0, i * cellSize, height());
    for (int j = 0; j <= fieldHeight; j++)
        painter.drawLine(0, j * cellSize, width(), j * cellSize);

    painter.setPen(Qt::NoPen);
    for (const QPoint &f : std::as_const(foods))
    {
        painter.setBrush(QColor(255, 80, 80));
        painter.drawEllipse(f.x() * cellSize + 3, f.y() * cellSize + 3, cellSize - 6, cellSize - 6);
    }

    for (int i = 0; i < snake.size(); ++i)
    {
        QPoint p = snake[i];

        if (gameState == Dying)
            painter.setBrush(QColor(255, 0, 0));
        else if (i == 0)
            painter.setBrush(QColor(0, 255, 120));
        else
            painter.setBrush(QColor(0, 180, 90));

        painter.drawRoundedRect(p.x() * cellSize + 2, p.y() * cellSize + 2, cellSize - 4, cellSize - 4, 6, 6);
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(10, 25, "Score: " + QString::number(score));
    painter.drawText(10, 50, "High: " + QString::number(highScore));
    painter.drawText(width() - 160, 25, "Wrap: " + QString(wrapMode ? "ON" : "OFF"));

    if (gameState == Dying)
        painter.fillRect(rect(), QColor(255, 0, 0, 40));

    if (paused && gameState == Playing)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 150));
        bool blink = (QTime::currentTime().msec() < 500);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 36, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "PAUSED");

        if (blink)
        {
            painter.setFont(QFont("Arial", 16));
            painter.drawText(rect().adjusted(0, 80, 0, 0), Qt::AlignCenter, "Press P to continue");
        }
    }

    if (gameState == GameOver)
    {
        painter.fillRect(rect(), QColor(0, 0, 0, 180));
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 36, QFont::Bold));
        painter.drawText(rect().adjusted(0, -40, 0, 0), Qt::AlignCenter, "GAME OVER");

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 18));
        painter.drawText(rect().adjusted(0, 40, 0, 0), Qt::AlignCenter, "Press ENTER to Restart");
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (gameState == StartScreen)
    {
        if (event->key() == Qt::Key_Return)
        {
            initGame();
            gameState = Playing;
            timer->start(baseSpeed);
        }
        return;
    }

    if (gameState == GameOver)
    {
        if (event->key() == Qt::Key_Return)
        {
            initGame();
            gameState = Playing;
            timer->start(baseSpeed);
        }
        return;
    }

    switch (event->key())
    {
    case Qt::Key_Up:
        if (dir != Down) nextDir = Up;
        break;
    case Qt::Key_Down:
        if (dir != Up) nextDir = Down;
        break;
    case Qt::Key_Left:
        if (dir != Right) nextDir = Left;
        break;
    case Qt::Key_Right:
        if (dir != Left) nextDir = Right;
        break;
    case Qt::Key_Space:
        wrapMode = !wrapMode;
        break;
    case Qt::Key_P:
        paused = !paused;
        break;
    }
}

void MainWindow::gameLoop()
{
    if (gameState != Playing)
        return;

    if (paused)
    {
        update();
        return;
    }

    dir = nextDir;
    moveSnake();
    checkCollision();
    update();
}

void MainWindow::moveSnake()
{
    QPoint head = snake.first();

    switch (dir)
    {
    case Up:    head.ry()--; break;
    case Down:  head.ry()++; break;
    case Left:  head.rx()--; break;
    case Right: head.rx()++; break;
    }

    if (wrapMode)
    {
        if (head.x() < 0) head.setX(fieldWidth - 1);
        if (head.x() >= fieldWidth) head.setX(0);
        if (head.y() < 0) head.setY(fieldHeight - 1);
        if (head.y() >= fieldHeight) head.setY(0);
    }

    snake.prepend(head);

    bool ateFood = false;

    for (int i = 0; i < foods.size(); ++i)
    {
        if (head == foods[i])
        {
            foods.removeAt(i);
            score += 10;
            if (score > highScore)
                highScore = score;

            ateFood = true;
            break;
        }
    }

    if (!ateFood)
        snake.removeLast();
    else
    {
        spawnFood();
        updateSpeed();
    }
}

void MainWindow::checkCollision()
{
    QPoint head = snake.first();

    if (!wrapMode)
    {
        if (head.x() < 0 || head.x() >= fieldWidth || head.y() < 0 || head.y() >= fieldHeight)
        {
            timer->stop();
            gameState = Dying;
            deathIndex = snake.size() - 1;
            deathTimer->start(60);
            return;
        }
    }

    for (int i = 1; i < snake.size(); ++i)
    {
        if (head == snake[i])
        {
            timer->stop();
            gameState = Dying;
            deathIndex = snake.size() - 1;
            deathTimer->start(60);
            return;
        }
    }
}

void MainWindow::deathAnimation()
{
    if (deathIndex >= 0)
    {
        snake.removeAt(deathIndex);
        deathIndex--;
        update();
    }
    else
    {
        deathTimer->stop();
        gameState = GameOver;
        update();
    }
}

void MainWindow::updateSpeed()
{
    int newSpeed = baseSpeed - score / 5;
    if (newSpeed < 60)
        newSpeed = 60;

    timer->start(newSpeed);
}
