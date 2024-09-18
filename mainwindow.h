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
#include <QVBoxLayout>
#include <QVector>
#include <QRect>
#include <QImage>
#include <QGroupBox>

struct BoundingBox {
    QRect rect;
    QString label;
    float confidence;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();  // Declare the destructor

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void updateImage();                                   // Update image with bounding boxes

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
    void copySelectedImages();
    void toggleYoloBoundingBoxes();  // Slot for the new toggle button
    void on_loadNamesFileButton_clicked();  // This slot takes no arguments

private:

    QMap<int, QColor> classColors;  // To store class colors

    QImage currentImage; // Holds the currently displayed image
    struct Annotation {
        QRect boundingBox; // Bounding box coordinates
        QString className; // Detected class name
        float confidence;  // Confidence level
    };
    QVector<Annotation> currentAnnotations; // Holds YOLO annotations for the current image
    QStringList classNames;
    QString getClassName(int classId);                    // Method to get class name from ID
    void loadClassNames(const QString& namesFilePath);    // Method to load the .names file

    void loadImagesFromDirectory();
    void logActivity(const QString &message);
    void displayBoundingBoxes();
    void hideBoundingBoxes();

    QVector<BoundingBox> getAnnotationsForCurrentImage();
    QVector<BoundingBox> boundingBoxes; // Example list of bounding boxes

    QListWidget *imageListWidget;
    QLabel *imageLabel;
    QLabel *titleLabel;
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
    QPushButton *copyButton;
    QPushButton *toggleYoloButton;  // New button to toggle bounding boxes
    QPushButton *loadNamesButton;    // Button for loading .names file
    QGroupBox *listGroupBox;
    QGroupBox *actionGroupBox;
    QGroupBox *yoloGroupBox;
    QStringList imageList;
    QStringList selectedImagePaths;
    QString savedFilePath;

    QDir directory;
    int currentImageIndex;

    QFile logFile;
    QTextStream logStream;
    QAction *loadNamesAction;        // Menu action for loading .names file

    // Layouts
    QVBoxLayout *actionButtonLayout;  // Layout for action buttons

    // Function to style buttons
    void styleButton(QPushButton *button);

    // Function to style background
    void setMainWindowStyle();

    // Bounding box toggle state
    bool showYoloBoundingBoxes = false;  // State to track if bounding boxes should be displayed

    // Additional methods to load and display YOLO bounding boxes
    void loadYOLOAnnotations(const QString &imagePath);
    void displayBoundingBoxes(const QImage &image);
};


#endif // MAINWINDOW_H
