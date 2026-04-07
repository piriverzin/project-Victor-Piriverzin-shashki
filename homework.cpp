#include <iostream>
#include <cctype>

const int MAX_WORDS = 1000;
const int MAX_WORD_LENGTH = 100;
const int ALPHABET_SIZE = 26;

// Структура для хранения слов
struct Word {
    char letters[MAX_WORD_LENGTH];
    int length;
};

// Глобальные переменные
Word words[MAX_WORDS];
int wordCount = 0;
char currentWord[MAX_WORD_LENGTH];
int currentWordLength = 0;
bool textEnded = false;

// Функция для проверки, является ли символ допустимым
bool isValidChar(char c) {
    return (c >= 'a' && c <= 'z') || c == ' ' || c == '.' || c == '\b';
}

// Функция для проверки, является ли символ латинской буквой
bool isLatinLetter(char c) {
    return c >= 'a' && c <= 'z';
}

// Функция для сравнения двух слов
int compareWords(const Word& w1, const Word& w2) {
    int minLength = w1.length < w2.length ? w1.length : w2.length;
    
    for (int i = 0; i < minLength; i++) {
        if (w1.letters[i] != w2.letters[i]) {
            return w1.letters[i] - w2.letters[i];
        }
    }
    
    return w1.length - w2.length;
}

// Функция для копирования слова
void copyWord(Word& dest, const Word& src) {
    dest.length = src.length;
    for (int i = 0; i < src.length; i++) {
        dest.letters[i] = src.letters[i];
    }
}

// Сортировка слов (сортировка вставками)
void sortWords() {
    for (int i = 1; i < wordCount; i++) {
        Word key;
        copyWord(key, words[i]);
        int j = i - 1;
        
        while (j >= 0 && compareWords(words[j], key) > 0) {
            copyWord(words[j + 1], words[j]);
            j--;
        }
        
        copyWord(words[j + 1], key);
    }
}

// Функция для добавления слова в массив
void addWordToArray() {
    if (currentWordLength > 0) {
        if (wordCount < MAX_WORDS) {
            words[wordCount].length = currentWordLength;
            for (int i = 0; i < currentWordLength; i++) {
                words[wordCount].letters[i] = currentWord[i];
            }
            wordCount++;
        }
        currentWordLength = 0;
    }
}

// Функция для вывода слов
void printWords() {
    for (int i = 0; i < wordCount; i++) {
        for (int j = 0; j < words[i].length; j++) {
            std::cout.put(words[i].letters[j]);
        }
        std::cout.put('\n');
    }
}

// Основная функция ввода текста
void inputText() {
    char c;
    
    std::cout << "Введите текст (только строчные латинские буквы, пробелы и точка в конце):\n";
    
    while (!textEnded) {
        // Чтение символа
        c = std::cin.get();
        
        // Игнорируем символы перевода строки при вводе
        if (c == '\n') {
            continue;
        }
        
        // Проверяем, является ли символ допустимым
        if (!isValidChar(c)) {
            continue;
        }
        
        // Обработка Backspace
        if (c == '\b') {
            if (currentWordLength > 0) {
                currentWordLength--;
                std::cout.put('\b');
                std::cout.put(' ');
                std::cout.put('\b');
            }
            continue;
        }
        
        // Выводим только допустимые символы
        if (isLatinLetter(c) || c == ' ' || c == '.') {
            std::cout.put(c);
        }
        
        // Обработка точки (конец текста)
        if (c == '.') {
            addWordToArray();
            textEnded = true;
            break;
        }
        
        // Обработка пробела
        if (c == ' ') {
            addWordToArray();
            continue;
        }
        
        // Обработка буквы
        if (isLatinLetter(c)) {
            if (currentWordLength < MAX_WORD_LENGTH) {
                currentWord[currentWordLength] = c;
                currentWordLength++;
            }
        }
    }
    
    std::cout.put('\n');
}

int main() {
    // Ввод текста
    inputText();
    
    // Сортировка слов
    sortWords();
    
    // Вывод отсортированных слов
    std::cout << "\nСлова в алфавитном порядке:\n";
    printWords();
    
    return 0;
}