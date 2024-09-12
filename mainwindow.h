#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QMenuBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void showPreviousImage();
    void showNextImage();
    void addImageToList();
    void removeImageFromList();
    void saveImageList();
    void clearImageList();
    void deleteSelectedImage();
    void moveSelectedImage();
    void openImageDirectory(); // New slot for opening the directory

private:
    void loadImagesFromDirectory();
    void updateImage();
    void logActivity(const QString &message);

    QListWidget *imageListWidget;
    QLabel *imageLabel;
    QLabel *infoLabel;
    QLabel *lastSavedLabel;
    QPushButton *leftButton;
    QPushButton *rightButton;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *saveButton;
    QPushButton *clearButton;
    QPushButton *deleteButton;
    QPushButton *moveButton;

    QStringList imageList;
    QStringList selectedImagePaths;
    QDir directory;
    int currentImageIndex;
    QString savedFilePath;

    QFile logFile;
    QTextStream logStream;

    // Function to style buttons
    void styleButton(QPushButton *button);

    // Function to style background
    void setMainWindowStyle();

};

#endif // MAINWINDOW_H
