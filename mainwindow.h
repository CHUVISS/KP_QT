#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QVector>
#include <QPoint>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();
    void deathAnimation();

private:
    void initGame();
    void moveSnake();
    void checkCollision();
    void spawnFood();
    void updateSpeed();

    QTimer *timer;
    QTimer *deathTimer;

    QVector<QPoint> snake;
    QVector<QPoint> foods;

    enum Direction { Up, Down, Left, Right };
    Direction dir;
    Direction nextDir;

    enum GameState { StartScreen, Playing, Dying, GameOver };
    GameState gameState;

    int cellSize;
    int fieldWidth;
    int fieldHeight;

    int score;
    int highScore;

    int baseSpeed;
    bool wrapMode;
    bool paused;

    int deathIndex;

    const int foodCount = 3;
};

#endif
