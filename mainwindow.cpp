#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <algorithm>
#include <ctime>
#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), rng(static_cast<unsigned>(time(nullptr))), answerShown(false) {
    ui->setupUi(this);
    setWindowTitle("五十音");

    // Initialize GUI elements
    modeComboBox = new QComboBox(this);
    modeComboBox->addItem("Select a mode");
    modeComboBox->addItem("Hiragana");
    modeComboBox->addItem("Katakana");
    modeComboBox->addItem("Mixed");
    modeComboBox->setCurrentIndex(0);
    modeComboBox->setMinimumHeight(40);
    modeComboBox->setMinimumWidth(200);
    modeComboBox->setStyleSheet(
        "QComboBox {"
        "   font-size: 16px;"
        "   padding: 8px;"
        "   border: 2px solid black;"
        "   border-radius: 5px;"
        "}"
        "QComboBox::drop-down {"
        "   width: 30px;"
        "}"
        "QComboBox::down-arrow {"
        "   image: url(:/dropdown_arrow.png);" // Use the custom icon
        "   width: 16px;" // Adjust size to match your icon
        "   height: 10px;"
        "}"
        );

    characterLabel = new QLabel("Select a mode to start the quiz!", this);
    characterLabel->setAlignment(Qt::AlignCenter);
    characterLabel->setStyleSheet("font-size: 48px;");

    characterTypeLabel = new QLabel("", this);
    characterTypeLabel->setAlignment(Qt::AlignCenter);
    characterTypeLabel->setStyleSheet("font-size: 16px; color: gray;");

    answerInput = new QLineEdit(this);
    answerInput->setPlaceholderText("Enter the romaji here");
    answerInput->setEnabled(false);
    answerInput->setMinimumHeight(40);

    submitButton = new QPushButton("Submit", this);
    submitButton->setEnabled(false);
    submitButton->setMinimumHeight(40);
    submitButton->setMinimumWidth(120);
    submitButton->setStyleSheet(
        "QPushButton {"
        "   font-size: 16px;"
        "   padding: 8px;"
        "   border: 2px solid black;"
        "   border-radius: 5px;"
        "   background-color: #555555;"
        "}"
        "QPushButton:hover {"
        "   background-color: #686868;"
        "}"
        );

    answerSkipButton = new QPushButton("Show Answer", this);
    answerSkipButton->setEnabled(false);
    answerSkipButton->setMinimumHeight(40);
    answerSkipButton->setMinimumWidth(120);
    answerSkipButton->setStyleSheet(
        "QPushButton {"
        "   font-size: 16px;"
        "   padding: 8px;"
        "   border: 2px solid black;"
        "   border-radius: 5px;"
        "   background-color: #555555;"
        "}"
        "QPushButton:hover {"
        "   background-color: #686868;"
        "}"
        );

    hintButton = new QPushButton("Hint", this);
    hintButton->setEnabled(false);
    hintButton->setMinimumHeight(40);
    hintButton->setMinimumWidth(120);
    hintButton->setStyleSheet(
        "QPushButton {"
        "   font-size: 16px;"
        "   padding: 8px;"
        "   border: 2px solid black;"
        "   border-radius: 5px;"
        "   background-color: #555555;"
        "}"
        "QPushButton:hover {"
        "   background-color: #686868;"
        "}"
        );

    answerLabel = new QLabel("", this);
    answerLabel->setAlignment(Qt::AlignCenter);
    answerLabel->setStyleSheet("font-size: 20px; color: #e33935;");

    hintLabel = new QLabel("", this);
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet("font-size: 20px; color: #bedd9b;");

    feedbackLabel = new QLabel("", this);
    feedbackLabel->setAlignment(Qt::AlignCenter);

    remainingLabel = new QLabel("", this);
    remainingLabel->setAlignment(Qt::AlignCenter);

    creditLabel = new QLabel("Made by TKwong", this); // Initialize the credit label
    creditLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    creditLabel->setStyleSheet("font-size: 10px; color: gray;"); // Style the credit label

    // Layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(modeComboBox);
    layout->addWidget(characterLabel);
    layout->addWidget(characterTypeLabel);
    layout->addWidget(answerInput);
    layout->addWidget(submitButton);
    layout->addWidget(answerSkipButton);
    layout->addWidget(hintButton);
    layout->addWidget(answerLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(feedbackLabel);
    layout->addWidget(remainingLabel);
    layout->addWidget(creditLabel); // Add the credit label at the bottom
    layout->addStretch(); // Optional: Add stretch to push the credit to the bottom if the window is resized

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Connect signals and slots
    connect(modeComboBox, &QComboBox::currentTextChanged, this, &MainWindow::startQuiz);
    connect(submitButton, &QPushButton::clicked, this, &MainWindow::checkAnswer);
    connect(answerInput, &QLineEdit::returnPressed, this, &MainWindow::checkAnswer);
    connect(answerSkipButton, &QPushButton::clicked, this, &MainWindow::showAnswerOrSkip);
    connect(hintButton, &QPushButton::clicked, this, &MainWindow::showHint);

    // Initialize character data
    initializeCharacters();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initializeCharacters() {
    // Populate the characters vector (same as the Python romaji list)
    characters = {
        {"あ", "ア", "a"}, {"い", "イ", "i"}, {"う", "ウ", "u"}, {"え", "エ", "e"}, {"お", "オ", "o"},
        {"か", "カ", "ka"}, {"き", "キ", "ki"}, {"く", "ク", "ku"}, {"け", "ケ", "ke"}, {"こ", "コ", "ko"},
        {"さ", "サ", "sa"}, {"し", "シ", "shi"}, {"す", "ス", "su"}, {"せ", "セ", "se"}, {"そ", "ソ", "so"},
        {"た", "タ", "ta"}, {"ち", "チ", "chi"}, {"つ", "ツ", "tsu"}, {"て", "テ", "te"}, {"と", "ト", "to"},
        {"な", "ナ", "na"}, {"に", "ニ", "ni"}, {"ぬ", "ヌ", "nu"}, {"ね", "ネ", "ne"}, {"の", "ノ", "no"},
        {"は", "ハ", "ha"}, {"ひ", "ヒ", "hi"}, {"ふ", "フ", "fu"}, {"へ", "ヘ", "he"}, {"ほ", "ホ", "ho"},
        {"ま", "マ", "ma"}, {"み", "ミ", "mi"}, {"む", "ム", "mu"}, {"め", "メ", "me"}, {"も", "モ", "mo"},
        {"や", "ヤ", "ya"}, {"ゆ", "ユ", "yu"}, {"よ", "ヨ", "yo"},
        {"ら", "ラ", "ra"}, {"り", "リ", "ri"}, {"る", "ル", "ru"}, {"れ", "レ", "re"}, {"ろ", "ロ", "ro"},
        {"わ", "ワ", "wa"}, {"を", "ヲ", "wo"}, {"ん", "ン", "n"},
        {"が", "ガ", "ga"}, {"ぎ", "ギ", "gi"}, {"ぐ", "グ", "gu"}, {"げ", "ゲ", "ge"}, {"ご", "ゴ", "go"},
        {"ざ", "ザ", "za"}, {"じ", "ジ", "ji"}, {"ず", "ズ", "zu"}, {"ぜ", "ゼ", "ze"}, {"ぞ", "ゾ", "zo"},
        {"だ", "ダ", "da"}, {"ぢ", "ヂ", "ji"}, {"づ", "ヅ", "zu"}, {"で", "デ", "de"}, {"ど", "ド", "do"},
        {"ば", "バ", "ba"}, {"び", "ビ", "bi"}, {"ぶ", "ブ", "bu"}, {"べ", "ベ", "be"}, {"ぼ", "ボ", "bo"},
        {"ぱ", "パ", "pa"}, {"ぴ", "ピ", "pi"}, {"ぷ", "プ", "pu"}, {"ぺ", "ペ", "pe"}, {"ぽ", "ポ", "po"}
    };
}

void MainWindow::startQuiz() {
    QString mode = modeComboBox->currentText();

    // If the placeholder is selected, reset the UI and do nothing
    if (mode == "Select a mode") {
        characterLabel->setText("Select a mode to start the quiz!");
        characterTypeLabel->clear();
        answerInput->setEnabled(false);
        submitButton->setEnabled(false);
        answerSkipButton->setEnabled(false);
        hintButton->setEnabled(false);
        answerLabel->clear();
        hintLabel->clear();
        feedbackLabel->clear();
        remainingLabel->clear();
        questions.clear();
        return;
    }

    // Enable input, submit button, answer/skip button, and hint button once a valid mode is selected
    answerInput->setEnabled(true);
    submitButton->setEnabled(true);
    answerSkipButton->setEnabled(true);
    hintButton->setEnabled(true);

    // Set the mode
    if (mode == "Hiragana") {
        currentMode = Hiragana;
    } else if (mode == "Katakana") {
        currentMode = Katakana;
    } else {
        currentMode = Mixed;
    }

    // Reset the quiz
    questions = characters;
    setupQuiz();
}

void MainWindow::setupQuiz() {
    if (questions.empty()) {
        characterLabel->setText("All characters are done!");
        characterTypeLabel->clear();
        answerInput->setEnabled(false);
        submitButton->setEnabled(false);
        answerSkipButton->setEnabled(false);
        hintButton->setEnabled(false);
        answerLabel->clear();
        hintLabel->clear();
        feedbackLabel->clear();
        remainingLabel->clear();
        return;
    }

    // Shuffle questions
    std::shuffle(questions.begin(), questions.end(), rng);

    // Display the first character
    displayNextCharacter();
}

void MainWindow::displayNextCharacter() {
    if (questions.empty()) {
        setupQuiz();
        return;
    }

    // Reset the answer shown flag and button text
    answerShown = false;
    answerSkipButton->setText("Show Answer");
    answerLabel->clear();
    hintLabel->clear();

    // Re-enable the submit button and input for the new question
    answerInput->setEnabled(true);
    submitButton->setEnabled(true);

    // Clear the feedback label (including the "Click 'Skip' to continue" message)
    feedbackLabel->clear();

    // Display the character based on the mode
    Character current = questions.front();
    bool isHiragana = true; // Default to hiragana

    if (currentMode == Hiragana) {
        characterLabel->setText(current.hiragana);
        characterTypeLabel->setText("Hiragana");
    } else if (currentMode == Katakana) {
        characterLabel->setText(current.katakana);
        characterTypeLabel->setText("Katakana");
    } else { // Mixed mode
        std::uniform_int_distribution<int> dist(0, 1);
        isHiragana = dist(rng) == 0;
        if (isHiragana) {
            characterLabel->setText(current.hiragana);
            characterTypeLabel->setText("Hiragana");
        } else {
            characterLabel->setText(current.katakana);
            characterTypeLabel->setText("Katakana");
        }
    }

    // Update remaining count
    remainingLabel->setText(QString("Characters left: %1").arg(questions.size()));
    answerInput->clear();
}

void MainWindow::checkAnswer() {
    if (questions.empty()) return;

    QString userAnswer = answerInput->text().trimmed().toLower();
    Character current = questions.front();

    if (userAnswer == "?") {
        feedbackLabel->setText(QString("There are %1 characters left").arg(questions.size()));
        return;
    }

    if (userAnswer == current.romaji) {
        feedbackLabel->setText("Correct!");
        questions.erase(questions.begin());
    } else {
        feedbackLabel->setText(QString("Incorrect. The correct romaji is: %1").arg(current.romaji));
    }

    displayNextCharacter();
}

void MainWindow::showAnswerOrSkip() {
    if (questions.empty()) return;

    Character current = questions.front();

    if (!answerShown) {
        // Show the answer and the corresponding character on a new line
        bool isHiragana = (characterTypeLabel->text() == "Hiragana");
        QString correspondingChar = isHiragana ? current.katakana : current.hiragana;
        QString correspondingType = isHiragana ? "片假名" : "平假名";
        answerLabel->setText(QString("Answer: %1\n%2: %3").arg(current.romaji, correspondingType, correspondingChar));

        // Disable the submit button and input to prevent cheating
        submitButton->setEnabled(false);
        answerInput->setEnabled(false);

        // Instruct the user to click "Skip"
        feedbackLabel->setText("Click 'Skip' to continue");

        // Update button to allow skipping
        answerShown = true;
        answerSkipButton->setText("Skip");
    } else {
        // Skip the question by moving it to the end of the list
        questions.push_back(current);
        questions.erase(questions.begin());
        displayNextCharacter();
    }
}

void MainWindow::showHint() {
    if (questions.empty()) return;

    Character current = questions.front();
    bool isHiragana = (characterTypeLabel->text() == "Hiragana");
    QString correspondingChar = isHiragana ? current.katakana : current.hiragana;
    QString correspondingType = isHiragana ? "片假名" : "平假名";
    hintLabel->setText(QString("%1: %2").arg(correspondingType, correspondingChar));
}
