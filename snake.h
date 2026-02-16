#ifndef SNAKE_H
#define SNAKE_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QKeyEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QList>

class SnakeGame : public QWidget {
    Q_OBJECT

public:
    explicit SnakeGame(QWidget *parent = nullptr);
    void startGame();
    void pauseGame();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateGame();

private:
    enum Direction {
        Up,
        Down,
        Left,
        Right
    };

    static const int CELL_SIZE = 20;
    static const int INITIAL_SPEED = 150;

    bool isRunning;
    bool isPaused;
    int score;
    int speed;
    Direction direction;
    Direction nextDirection;

    QVector<QPoint> snake;
    QPoint food;
    QTimer timer;

    void generateFood();
    void moveSnake();
    bool checkCollision() const;
    void drawSnake(QPainter &painter) const;
    void drawFood(QPainter &painter) const;
    void gameOver();
    QSize gridSize() const;
};

#endif // SNAKE_H
