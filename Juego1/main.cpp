#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QRect>
#include <QPixmap>

class Game : public QWidget {
public:
    Game(QWidget *parent = 0) : QWidget(parent) {
        x = 50;
        y = 50;
        groundLevel = 300;
        ySpeed = 0;
        gravity = 10;
        lives = 3;
        timerId = startTimer(16);

        // Cargar imágenes
        platformImage.load(":/images/platform.png");
        groundImage.load(":/images/ground.png");
        mortyImage.load(":/images/morty.png");

        // Crear algunas plataformas
        platforms.append(QRect(50, 200, 150, 10));  // Plataforma más baja
        platforms.append(QRect(250, 150, 150, 10)); // Plataforma intermedia
        platforms.append(QRect(450, 100, 150, 10)); // Plataforma más alta

        // Plataforma superior izquierda donde aparece Morty
        startingPlatform = QRect(50, 50, 100, 10);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(this);

        // Dibujar las plataformas con imágenes
        foreach (const QRect &platform, platforms) {
            painter.drawPixmap(platform, platformImage);
        }

        // Dibujar la plataforma inicial con imagen
        painter.drawPixmap(startingPlatform, platformImage);

        // Dibujar el suelo solo si Morty tiene vidas restantes con imagen
        if (lives > 0) {
            painter.drawPixmap(0, groundLevel, width(), height() - groundLevel, groundImage);
        }

        // Dibujar a Morty con imagen
        painter.drawPixmap(x, y, 50, 50, mortyImage);
    }

    void timerEvent(QTimerEvent *event) override {
        Q_UNUSED(event);

        ySpeed += gravity / 60.0;
        y += ySpeed;

        // Verificar colisiones con plataformas
        foreach (const QRect &platform, platforms) {
            if (platform.intersects(QRect(x, y, 50, 50))) {
                y = platform.top() - 50;
                ySpeed = 0;
                break;
            }
        }

        // Verificar colisión con la plataforma inicial
        if (startingPlatform.intersects(QRect(x, y, 50, 50))) {
            y = startingPlatform.top() - 50;
            ySpeed = 0;
        }

        // Verificar si Morty ha caído más allá de la pantalla
        if (y > height()) {
            if (lives > 0) {
                // Restar una vida y reiniciar posición de Morty
                lives--;
                resetPosition();
            } else {
                // Si no hay más vidas, detener el timer
                killTimer(timerId);
            }
        }

        update(); // Vuelve a pintar la pantalla
    }

    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_W:
            if (y == groundLevel) {
                ySpeed = -12;
            }
            break;
        case Qt::Key_A:
            xSpeed = -5;
            break;
        case Qt::Key_D:
            xSpeed = 5;
            break;
        }
    }

    void keyReleaseEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_A:
        case Qt::Key_D:
            xSpeed = 0;
            break;
        }
    }

private:
    int x;
    int y;
    int groundLevel;
    double ySpeed;
    double gravity;
    int lives;
    int xSpeed = 0;
    int timerId;
    QList<QRect> platforms;
    QRect startingPlatform;
    QPixmap platformImage;
    QPixmap groundImage;
    QPixmap mortyImage;

    void resetPosition() {
        // Reiniciar posición de Morty en la plataforma inicial
        x = startingPlatform.left();
        y = startingPlatform.top() - 50;
        ySpeed = 0;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Game game;
    game.setGeometry(100, 100, 600, 400);
    game.setWindowTitle("Morty's Adventure");
    game.show();

    return app.exec();
}
