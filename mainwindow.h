#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <vector>
#include <random>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startQuiz();
    void checkAnswer();
    void showAnswerOrSkip();
    void showHint();

private:
    Ui::MainWindow *ui;

    // GUI elements
    QComboBox *modeComboBox;
    QLabel *characterLabel;
    QLabel *characterTypeLabel;
    QLineEdit *answerInput;
    QPushButton *submitButton;
    QPushButton *answerSkipButton;
    QPushButton *hintButton;
    QLabel *answerLabel;
    QLabel *hintLabel;
    QLabel *feedbackLabel;
    QLabel *remainingLabel;
    QLabel *creditLabel; // New label for the credit

    // Data structure for characters
    struct Character {
        QString hiragana;
        QString katakana;
        QString romaji;
    };
    std::vector<Character> characters;
    std::vector<Character> questions;
    enum Mode { Hiragana, Katakana, Mixed } currentMode;

    // Random number generation
    std::mt19937 rng;

    // Flag to track if the answer has been shown
    bool answerShown;

    void initializeCharacters();
    void setupQuiz();
    void displayNextCharacter();
};

#endif // MAINWINDOW_H
