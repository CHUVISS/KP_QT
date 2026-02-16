#include "Header Files/snake.h"
#include <QMessageBox>
#include <QFont>

SnakeGame::SnakeGame(QWidget *parent)
    : QWidget(parent),
    isRunning(false),
    isPaused(false),
    score(0),
    speed(INITIAL_SPEED),
    direction(Right),
    nextDirection(Right)
{
    setFixedSize(600, 400);
    setWindowTitle("Змейка — Qt");

    snake.append(QPoint(5, 10));
    snake.append(QPoint(4, 10));
    snake.append(QPoint(3, 10));

    generateFood();

    connect(&timer, &QTimer::timeout, this, &SnakeGame::updateGame);
}

void SnakeGame::startGame()
{
    isRunning = true;
    isPaused = false;
    score = 0;
    speed = INITIAL_SPEED;
    direction = Right;
    nextDirection = Right;

    snake.clear();
    snake.append(QPoint(5, 10));
    snake.append(QPoint(4, 10));
    snake.append(QPoint(3, 10));

    generateFood();
    timer.start(speed);
    update();
}

void SnakeGame::pauseGame()
{
    isPaused = !isPaused;
    if (isPaused) {
        timer.stop();
    } else {
        timer.start(speed);
    }
    update();
}

void SnakeGame::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::black);

    if (!isRunning) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "Нажмите ПРОБЕЛ для старта");
        return;
    }

    if (isPaused) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "ПАУЗА");
    }

    drawFood(painter);
    drawSnake(painter);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(10, 20, QString("Счёт: %1").arg(score));
}

void SnakeGame::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        if (!isRunning) {
            startGame();
        } else {
            pauseGame();
        }
        break;
    case Qt::Key_Left:
        if (direction != Right) nextDirection = Left;
        break;
    case Qt::Key_Right:
        if (direction != Left) nextDirection = Right;
        break;
    case Qt::Key_Up:
        if (direction != Down) nextDirection = Up;
        break;
    case Qt::Key_Down:
        if (direction != Up) nextDirection = Down;
        break;
    case Qt::Key_Escape:
        qApp->quit();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void SnakeGame::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (isRunning && !isPaused) {
        generateFood();
    }
}

void SnakeGame::updateGame()
{
    if (!isRunning || isPaused) return;

    direction = nextDirection;
    moveSnake();

    if (checkCollision()) {
        gameOver();
        return;
    }

    if (snake.first() == food) {
        score += 10;
        generateFood();

        if (score % 50 == 0 && speed > 50) {
            speed -= 10;
            timer.start(speed);
        }
    } else {
        snake.removeLast();
    }

    update();
}

void SnakeGame::generateFood()
{
    QSize grid = gridSize();
    do {
        food.setX(QRandomGenerator::global()->bounded(grid.width()));
        food.setY(QRandomGenerator::global()->bounded(grid.height()));
    } while (snake.contains(food));
}

void SnakeGame::moveSnake()
{
    QPoint head = snake.first();
    switch (direction) {
    case Up:    head.setY(head.y() - 1); break;
    case Down:  head.setY(head.y() + 1); break;
    case Left:  head.setX(head.x() - 1); break;
    case Right: head.setX(head.x() + 1); break;
    }

    QSize grid = gridSize();
    if (head.x() < 0) head.setX(grid.width() - 1);
    if (head.x() >= grid.width()) head.setX(0);
    if (head.y() < 0) head.setY(grid.height() - 1);
    if (head.y() >= grid.height()) head.setY(0);

    snake.insert(0, head);
}

bool SnakeGame::checkCollision() const
{
    QPoint head = snake.first();

    for (int i = 1; i < snake.size(); ++i) {
        if (head == snake[i]) {
            return true;
        }
    }

    return false;
}

void SnakeGame::drawSnake(QPainter &painter) const
{
    painter.setBrush(QColor(70, 180, 70));
    QPoint head = snake.first();
    painter.drawRect(head.x() * CELL_SIZE, head.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);

    painter.setBrush(QColor(50, 150, 50));
    for (int i = 1; i < snake.size(); ++i) {
        QPoint pos = snake[i];
        painter.drawRect(pos.x() * CELL_SIZE, pos.y() * CELL_SIZE, CELL_SIZE, CELL_SIZE);
    }
}

void SnakeGame::drawFood(QPainter &painter) const
{
    painter.setBrush(Qt::red);
    painter.drawEllipse(food
