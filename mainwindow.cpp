#include "mainwindow.h"

#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QMessageBox>
#include <QDateTime>

#include "mainwindow.h"

#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QMessageBox>
#include <QDateTime>

#include <QLabel>
#include <QVBoxLayout>

// Definition of the styleButton function
void MainWindow::styleButton(QPushButton *button) {
    button->setStyleSheet(
        "QPushButton {"
        "   background-color: #003366;" // Steel Blue
        "   border: 1px solid #4169e1;" // Royal Blue
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;" // Bold text
        "   padding: 8px 16px;"
        "   color: white;" // White text color
        "}"
        "QPushButton:hover {"
        "   background-color: #4682b4;" // Royal Blue on hover
        "   border: 1px solid #315c8a;" // Darker border on hover
        "}"
        "QPushButton:pressed {"
        "   background-color: #315c8a;" // Darker Steel Blue when pressed
        "   border: 1px solid #25485e;" // Even darker border when pressed
        "}"
        );
}

void MainWindow::setMainWindowStyle() {
    this->setStyleSheet(
        "background-color: #FAFAFA;" // Change to your preferred background color
        );
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentImageIndex(0), savedFilePath("") {
    // Create the title label
    QLabel *titleLabel = new QLabel("IRIS Image Selector", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #003366; }"); // Professional font color and size

    // Setup UI

    // Setup UI
    setMainWindowStyle(); // Set main window background color

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    infoLabel = new QLabel(this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");
    infoLabel->setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);

    lastSavedLabel = new QLabel(this);
    lastSavedLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    lastSavedLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    leftButton = new QPushButton("<", this);
    rightButton = new QPushButton(">", this);
    addButton = new QPushButton("Add", this);
    removeButton = new QPushButton("Remove", this);
    saveButton = new QPushButton("Save List", this);
    clearButton = new QPushButton("Clear List", this);
    deleteButton = new QPushButton("Delete", this);
    moveButton = new QPushButton("Move", this);

    // Style the buttons
    styleButton(leftButton);
    styleButton(rightButton);
    styleButton(addButton);
    styleButton(removeButton);
    styleButton(saveButton);
    styleButton(clearButton);
    styleButton(deleteButton);
    styleButton(moveButton);

    // Set button sizes
    QSize buttonSize(60, 40);
    leftButton->setFixedSize(buttonSize);
    rightButton->setFixedSize(buttonSize);
    addButton->setMinimumWidth(80);
    removeButton->setMinimumWidth(80);
    saveButton->setMinimumWidth(80);
    clearButton->setMinimumWidth(80);
    deleteButton->setMinimumWidth(80);
    moveButton->setMinimumWidth(80);

    addButton->setFixedHeight(buttonSize.height());
    removeButton->setFixedHeight(buttonSize.height());
    saveButton->setFixedHeight(buttonSize.height());
    clearButton->setFixedHeight(buttonSize.height());
    deleteButton->setFixedHeight(buttonSize.height());
    moveButton->setFixedHeight(buttonSize.height());

    connect(leftButton, &QPushButton::clicked, this, &MainWindow::showPreviousImage);
    connect(rightButton, &QPushButton::clicked, this, &MainWindow::showNextImage);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addImageToList);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeImageFromList);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveImageList);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearImageList);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedImage);
    connect(moveButton, &QPushButton::clicked, this, &MainWindow::moveSelectedImage);

    // Layout for navigation buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(leftButton);
    buttonLayout->addWidget(rightButton);
    buttonLayout->addStretch();

    // Layout for add/remove/save buttons
    QVBoxLayout *actionButtonLayout = new QVBoxLayout();
    actionButtonLayout->addWidget(addButton);
    actionButtonLayout->addWidget(removeButton);
    actionButtonLayout->addWidget(saveButton);
    actionButtonLayout->addWidget(clearButton);
    actionButtonLayout->addWidget(deleteButton);
    actionButtonLayout->addWidget(moveButton);

    // Create the list widget for image paths
    imageListWidget = new QListWidget(this);
    imageListWidget->setFixedWidth(300);

    // Layout for displaying the list widget and buttons
    QVBoxLayout *rightPanelLayout = new QVBoxLayout();
    rightPanelLayout->addWidget(imageListWidget);
    rightPanelLayout->addLayout(actionButtonLayout);
    rightPanelLayout->addWidget(lastSavedLabel); // Add last saved label below the buttons

    // Layout to ensure right alignment
    QHBoxLayout *rightPanelContainerLayout = new QHBoxLayout();
    rightPanelContainerLayout->addStretch();
    rightPanelContainerLayout->addLayout(rightPanelLayout);

    // Main layout for image display and right panel
    QVBoxLayout *imageLayout = new QVBoxLayout();
    imageLayout->addWidget(imageLabel);
    imageLayout->addLayout(buttonLayout);
    imageLayout->addWidget(infoLabel); // Add the infoLabel with image name and resolution

    // Create spacer items to ensure the image section is centered
    QSpacerItem *leftSpacer = new QSpacerItem(200, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);  // Larger spacer on the left
    QSpacerItem *rightSpacer = new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    // Central layout with spacers to center image layout between left side and right panel
    QHBoxLayout *centralLayout = new QHBoxLayout();
    centralLayout->addItem(leftSpacer);   // Add larger left spacer
    centralLayout->addLayout(imageLayout); // Image layout in the middle
    centralLayout->addLayout(rightPanelContainerLayout); // Right panel with list and buttons
    centralLayout->addItem(rightSpacer);  // Add right spacer

    // Apply stretch factors to adjust centering
    centralLayout->setStretch(0, 0);  // Fixed space for left spacer
    centralLayout->setStretch(1, 2);  // Stretch the image layout
    centralLayout->setStretch(2, 1);  // Stretch the right panel container

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(titleLabel); // Add title at the top
    mainLayout->addLayout(centralLayout);

    QWidget *container = new QWidget();
    container->setLayout(mainLayout);
    setCentralWidget(container);

    // Set initial window size
    setMinimumSize(1000, 600);

    // Initialize log file
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString logFileName = QString("log_%1.txt").arg(timestamp);
    logFile.setFileName(logFileName);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logStream.setDevice(&logFile);
        logActivity("Application started.");
    }
    // Create the menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = new QMenu("File", menuBar);
    QAction *openDirAction = new QAction("Open Directory", this);
    connect(openDirAction, &QAction::triggered, this, &MainWindow::openImageDirectory);
    fileMenu->addAction(openDirAction);
    menuBar->addMenu(fileMenu);
    setMenuBar(menuBar);

    // Style the menu bar
    menuBar->setStyleSheet("QMenuBar { background-color: #003366; color: white; }"
                           "QMenuBar::item { color: white; }"
                           "QMenuBar::item:selected { background-color: #4682b4; }"
                           "QMenu::item:selected { background-color: #00509E; }");

    // Try to load images at startup
    loadImagesFromDirectory();
    updateImage();

    setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Right) {
        showNextImage();
    } else if (event->key() == Qt::Key_Left) {
        showPreviousImage();
    }
}

void MainWindow::openImageDirectory() {
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Directory", "");
    if (!dirPath.isEmpty()) {
        // Clear the existing image list
        imageList.clear();
        selectedImagePaths.clear();
        currentImageIndex = 0;

        // Set new directory
        directory.setPath(dirPath);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
        imageList = directory.entryList(filters, QDir::Files, QDir::Name);

        // Log activity
        logActivity("Loaded images from directory: " + dirPath);

        // Update the UI
        updateImage();
    }
}

void MainWindow::showPreviousImage() {
    if (currentImageIndex > 0) {
        --currentImageIndex;
        updateImage();
        logActivity("Navigated to previous image.");
    }
}

void MainWindow::showNextImage() {
    if (currentImageIndex < imageList.size() - 1) {
        ++currentImageIndex;
        updateImage();
        logActivity("Navigated to next image.");
    }
}

void MainWindow::loadImagesFromDirectory() {
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Directory", "");
    if (!dirPath.isEmpty()) {
        directory.setPath(dirPath);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
        imageList = directory.entryList(filters, QDir::Files, QDir::Name);
        currentImageIndex = 0;
        logActivity("Loaded images from directory: " + dirPath);
    }
}

void MainWindow::updateImage() {
    if (imageList.isEmpty()) return;

    QString imagePath = directory.filePath(imageList.at(currentImageIndex));
    QImage image(imagePath);

    if (image.isNull()) return;

    imageLabel->setPixmap(QPixmap::fromImage(image).scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QString imageName = "Image: " + imageList.at(currentImageIndex);
    QString resolution = "Resolution: " + QString::number(image.width()) + " x " + QString::number(image.height());

    infoLabel->setText(imageName + "<br>" + resolution);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateImage();
}

void MainWindow::addImageToList() {
    if (imageList.isEmpty()) return;

    QString imagePath = directory.filePath(imageList.at(currentImageIndex));
    if (!selectedImagePaths.contains(imagePath)) {
        selectedImagePaths.append(imagePath);
        imageListWidget->addItem(imagePath);
        logActivity("Added image path to list: " + imagePath);
    }
}

void MainWindow::removeImageFromList() {
    QList<QListWidgetItem*> selectedItems = imageListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "No image path selected to remove.");
        return;
    }

    for (QListWidgetItem *item : selectedItems) {
        QString imagePath = item->text();
        selectedImagePaths.removeOne(imagePath);
        delete imageListWidget->takeItem(imageListWidget->row(item));

        // Show confirmation message
        QMessageBox::information(this, "Removed", "Removed: " + imagePath);

        // Log removal
        logActivity("Removed image path from list: " + imagePath);
    }
}

void MainWindow::saveImageList() {
    if (savedFilePath.isEmpty()) {
        QString filePath = QFileDialog::getSaveFileName(this, "Save List", "", "Text Files (*.txt)");
        if (filePath.isEmpty()) return; // User canceled the save operation

        savedFilePath = filePath;
    }

    QFile file(savedFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &path : selectedImagePaths) {
            out << path << "\n";
        }
        file.close();

        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        lastSavedLabel->setText("Last saved list at: " + timestamp);
        logActivity("Saved image list to: " + savedFilePath);
    } else {
        QMessageBox::warning(this, "Error", "Failed to save list.");
    }
}

void MainWindow::clearImageList() {
    // Prompt the user to confirm clearing the list
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Clear List", "Do you want to clear the current image list? This will not affect the saved list file.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // Clear the list without affecting the saved file
        selectedImagePaths.clear();
        imageListWidget->clear();
        lastSavedLabel->clear(); // Clear the last saved label

        // Reset the saved file path
        savedFilePath.clear();

        // Prompt the user to save the new list to a new file
        QMessageBox::information(this, "List Cleared", "The image list has been cleared. You can now start adding new images and save to a new file.");

        // Optionally, log activity
        logActivity("Cleared image list. Previous list file not affected. User prompted to create a new list.");
    }
}

void MainWindow::deleteSelectedImage() {
    if (savedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Save Required", "Please save the list before deleting images.");
        return;
    }

    // Confirm deletion with the user
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion", "Do you want to delete all images and their corresponding annotation files?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return; // User canceled the deletion
    }

    // Create 'deleted_images' folder in the parent directory
    QString parentDirPath = directory.path(); // Get the path of the current directory
    QDir parentDir(parentDirPath); // Create a QDir object for the current directory
    if (!parentDir.cdUp()) { // Move up one directory level
        QMessageBox::warning(this, "Error", "Failed to move up to the parent directory.");
        return;
    }

    QString deletedImagesPath = parentDir.filePath("deleted_images");
    if (!QDir(deletedImagesPath).exists()) {
        if (!QDir().mkpath(deletedImagesPath)) {
            QMessageBox::warning(this, "Error", "Failed to create 'deleted_images' directory.");
            return;
        }
    }

    // Move each image and its corresponding annotation file to the 'deleted_images' folder
    for (const QString &imagePath : selectedImagePaths) {
        QFileInfo fileInfo(imagePath);
        QString baseName = fileInfo.baseName();
        QString imageExt = fileInfo.suffix();

        QString deletedImagePath = deletedImagesPath + "/" + fileInfo.fileName();
        QString annotationPath = directory.filePath(baseName + ".txt");
        QString deletedAnnotationPath = deletedImagesPath + "/" + baseName + ".txt";

        // Move image file
        if (!QFile::rename(imagePath, deletedImagePath)) {
            QMessageBox::warning(this, "Error", "Failed to move image: " + imagePath);
        } else {
            // Remove from the list widget and log activity
            QListWidgetItem *item = imageListWidget->findItems(imagePath, Qt::MatchExactly).first();
            delete imageListWidget->takeItem(imageListWidget->row(item));
            logActivity("Moved image to deleted folder: " + imagePath);
        }

        // Move annotation file if it exists
        if (QFile::exists(annotationPath)) {
            if (!QFile::rename(annotationPath, deletedAnnotationPath)) {
                QMessageBox::warning(this, "Error", "Failed to move annotation file: " + annotationPath);
            } else {
                logActivity("Moved annotation file to deleted folder: " + annotationPath);
            }
        }
    }

    // Clear the selectedImagePaths list
    selectedImagePaths.clear();
    lastSavedLabel->clear(); // Clear the last saved label

    // Update the UI after deletion
    updateImage();
}

void MainWindow::moveSelectedImage() {
    if (selectedImagePaths.isEmpty()) {
        QMessageBox::warning(this, "No Images", "There are no images to move.");
        return;
    }

    // Prompt the user to select a destination directory
    QString destinationDirPath = QFileDialog::getExistingDirectory(this, "Select Destination Directory", "");
    if (destinationDirPath.isEmpty()) {
        return; // User canceled the directory selection
    }

    // Create the destination directory if it doesn't exist
    QDir destinationDir(destinationDirPath);
    if (!destinationDir.exists()) {
        if (!destinationDir.mkpath(destinationDirPath)) {
            QMessageBox::warning(this, "Error", "Failed to create the destination directory.");
            return;
        }
    }

    // Move each image and its corresponding annotation file to the selected directory
    for (const QString &imagePath : selectedImagePaths) {
        QFileInfo fileInfo(imagePath);
        QString baseName = fileInfo.baseName();
        QString imageExt = fileInfo.suffix();

        QString destinationImagePath = destinationDir.filePath(fileInfo.fileName());
        QString annotationPath = directory.filePath(baseName + ".txt");
        QString destinationAnnotationPath = destinationDir.filePath(baseName + ".txt");

        // Move image file
        if (!QFile::rename(imagePath, destinationImagePath)) {
            QMessageBox::warning(this, "Error", "Failed to move image: " + imagePath);
        } else {
            // Log activity
            logActivity("Moved image to: " + destinationImagePath);
        }

        // Move annotation file if it exists
        if (QFile::exists(annotationPath)) {
            if (!QFile::rename(annotationPath, destinationAnnotationPath)) {
                QMessageBox::warning(this, "Error", "Failed to move annotation file: " + annotationPath);
            } else {
                logActivity("Moved annotation file to: " + destinationAnnotationPath);
            }
        }
    }

    // Clear the selectedImagePaths list but keep the list widget populated
    selectedImagePaths.clear();
    lastSavedLabel->clear(); // Clear the last saved label

    // Update the UI after moving
    updateImage();
}


void MainWindow::logActivity(const QString &message) {
    if (logFile.isOpen()) {
        logStream << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << " - " << message << "\n";
        logStream.flush();
    }
}
