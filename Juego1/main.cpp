#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QMessageBox>
#include <QDebug>

class Game : public QWidget {
public:
    Game(QWidget *parent = 0) : QWidget(parent) {
        x = 50;
        y = 50;
        groundLevel = 300;
        ySpeed = 0;
        gravity = 15;
        lives = 3;
        timerId = startTimer(16);
        level = 1; // Iniciar en el nivel 1

        // Cargar imágenes
        if (!platformImage.load("C:/Users/Admin/Documents/Juego1/images/platform.png")) {
            qDebug() << "Error cargando la imagen de la plataforma";
        }

        if (!groundImage.load("C:/Users/Admin/Documents/Juego1/images/ground.png")) {
            qDebug() << "Error cargando la imagen del suelo";
        }

        if (!mortyImage.load("C:/Users/Admin/Documents/Juego1/images/morty.png")) {
            qDebug() << "Error cargando la imagen de Morty";
        }

        if (!goalImage.load("C:/Users/Admin/Documents/Juego1/images/goal.png")) {
            qDebug() << "Error cargando la imagen de la meta";
        }

        if (!platform2Image.load("C:/Users/Admin/Documents/Juego1/images/platform2.png")) {
            qDebug() << "Error cargando la imagen de la plataforma2";
        }

        // Crear algunas plataformas para el nivel 1
        platforms.append(QRect(50, 200, 150, 10));  // Plataforma más baja
        platforms.append(QRect(250, 150, 150, 10)); // Plataforma intermedia
        platforms.append(QRect(450, 100, 150, 10)); // Plataforma más alta

        // Plataforma superior izquierda donde aparece Morty
        startingPlatform = QRect(50, 50, 100, 10);

        // Definir la meta para el nivel 1
        goal = QRect(500, 50, 50, 50);

        // Crear algunas plataformas para el nivel 2
        if (level == 2) {
            platforms.clear();
            platforms.append(QRect(50, 250, 150, 10));
            platforms.append(QRect(250, 200, 150, 10));
            platforms.append(QRect(450, 150, 150, 10));
            startingPlatform = QRect(50, 200, 100, 10);
            goal = QRect(500, 150, 50, 50);
        }
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(this);

        // Dibujar las plataformas con imágenes
        foreach (const QRect &platform, platforms) {
            painter.drawPixmap(platform, (level == 2) ? platform2Image : platformImage);
        }

        // Dibujar la plataforma inicial con imagen
        painter.drawPixmap(startingPlatform, (level == 2) ? platform2Image : platformImage);

        // Dibujar el suelo solo si Morty tiene vidas restantes con imagen
        if (lives > 0) {
            painter.drawPixmap(0, groundLevel, width(), height() - groundLevel, groundImage);
        }

        // Dibujar a Morty con imagen
        painter.drawPixmap(x, y, 50, 50, mortyImage);

        // Dibujar la meta con imagen y resaltado si está activa
        painter.drawPixmap(goal, (goalActive && level == 1) ? goalActiveImage : goalImage);

        // Dibujar el texto del nivel
        painter.setFont(QFont("Arial", 12));
        painter.setBrush(QBrush(QColor(255, 255, 255))); // Color blanco
        painter.drawText(10, 20, "Nivel: " + QString::number(level));
    }

    void timerEvent(QTimerEvent *event) override {
        Q_UNUSED(event);

        ySpeed += gravity / 60.0;
        y += ySpeed;

        // Verificar colisiones con plataformas
        bool onPlatform = false;
        foreach (const QRect &platform, platforms) {
            if (platform.intersects(QRect(x, y, 50, 50))) {
                y = platform.top() - 50;
                ySpeed = 0;
                onPlatform = true;
                break;
            }
        }

        // Verificar colisión con la plataforma inicial
        if (startingPlatform.intersects(QRect(x, y, 50, 50))) {
            y = startingPlatform.top() - 50;
            ySpeed = 0;
            onPlatform = true;
        }

        // Verificar colisión con la meta
        if (goal.intersects(QRect(x, y, 50, 50)) && level == 1) {
            // Morty ha alcanzado la meta, avanzar al siguiente nivel
            advanceToNextLevel();
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

        // Actualizar posición de Morty según la velocidad horizontal
        x += xSpeed;

        // Permitir el salto solo cuando está en el suelo o en una plataforma
        if (onPlatform || y >= groundLevel - 5) {
            canJump = true;
        }

        // Actualizar la activación de la meta para resaltarla
        updateGoalStatus();

        update(); // Vuelve a pintar la pantalla
    }

    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_W:
            if (canJump) {
                ySpeed = -10;
                canJump = false;
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
    QPixmap goalImage;
    QPixmap goalActiveImage;
    QPixmap platform2Image;
    QRect goal;
    bool canJump = true;
    int level;
    bool goalActive = false;

    void resetPosition() {
        // Reiniciar posición de Morty en la plataforma inicial
        x = startingPlatform.left();
        y = startingPlatform.top() - 50;
        ySpeed = 0;
    }

    void advanceToNextLevel() {
        // Aumentar el nivel y definir la nueva meta para el siguiente nivel
        level++;
        qDebug() << "¡Has avanzado al nivel" << level << "!";
            goal = QRect(500, 150, 50, 50);
        goalActive = false;
        resetPosition();
    }

    void updateGoalStatus() {
        // Resaltar la meta por un corto período después de alcanzarla
        if (goal.intersects(QRect(x, y, 50, 50)) && level == 1) {
            goalActive = true;
            QTimer::singleShot(1000, this, &Game::deactivateGoal);
        }
    }

    void deactivateGoal() {
        goalActive = false;
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
