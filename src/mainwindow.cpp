#include "mainwindow.h"

#include <QAction>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>
#include <QRegularExpression>
#include <QStatusBar>
#include <QSet>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QSize>
#include <QStyle>
#include <QUrl>
#include <QDesktopServices>
#include <QToolBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <algorithm>

// ------------------------------------------------------------
// Styling
// ------------------------------------------------------------
void MainWindow::styleButton(QPushButton *button)
{
    button->setStyleSheet(
        "QPushButton {"
        "   background-color: #003366;"
        "   border: 1px solid #4169e1;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 8px 16px;"
        "   color: white;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4682b4;"
        "   border: 1px solid #315c8a;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #315c8a;"
        "   border: 1px solid #25485e;"
        "}"
    );
}

void MainWindow::setMainWindowStyle()
{
    this->setStyleSheet("background-color: #FAFAFA; color: black;");
}

void MainWindow::updateToggleYoloButtonStyle()
{
    // Re-integrated: Toggle button status color
    if (!toggleYoloButton) return;

    if (showYoloBoundingBoxes) {
        toggleYoloButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #1f7a1f;"
            "   border: 1px solid #155315;"
            "   border-radius: 5px;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   padding: 8px 16px;"
            "   color: white;"
            "}"
            "QPushButton:hover { background-color: #2b9a2b; }"
            "QPushButton:pressed { background-color: #155315; }"
        );
    } else {
        toggleYoloButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #6b6b6b;"
            "   border: 1px solid #4f4f4f;"
            "   border-radius: 5px;"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   padding: 8px 16px;"
            "   color: white;"
            "}"
            "QPushButton:hover { background-color: #7a7a7a; }"
            "QPushButton:pressed { background-color: #4f4f4f; }"
        );
    }
}

// ------------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Class colors (stable palette)
    classColors[0] = QColor("#FF6347");
    classColors[1] = QColor("#9400D3");
    classColors[2] = QColor("#32CD32");
    classColors[3] = QColor("#FFD700");
    classColors[4] = QColor("#FF4500");
    classColors[5] = QColor("#00CED1");
    classColors[6] = QColor("#A0522D");
    classColors[7] = QColor("#FF1493");
    classColors[8] = QColor("#4682b4");
    classColors[9] = QColor("#2E8B57");
    classColors[10] = QColor("#DC143C");
    classColors[11] = QColor("#8A2BE2");
    classColors[12] = QColor("#7FFF00");
    classColors[13] = QColor("#D2691E");
    classColors[14] = QColor("#FFB6C1");
    classColors[15] = QColor("#800080");
    classColors[16] = QColor("#C71585");
    classColors[17] = QColor("#FF69B4");
    classColors[18] = QColor("#5F9EA0");
    classColors[19] = QColor("#FFDAB9");
    classColors[20] = QColor("#FF8C00");
    classColors[21] = QColor("#20B2AA");
    classColors[22] = QColor("#B22222");
    classColors[23] = QColor("#4682B4");
    classColors[24] = QColor("#008080");

    setMainWindowStyle();

    // Title
    titleLabel = new QLabel("AI Image Suite", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #003366; }");

    // Image view
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Compare widget (hidden by default)
    compareWidget = new QWidget(this);
    compareLeftImageLabel = new QLabel(this);
    compareRightImageLabel = new QLabel(this);
    compareLeftInfoLabel = new QLabel(this);
    compareRightInfoLabel = new QLabel(this);

    for (QLabel *l : {compareLeftImageLabel, compareRightImageLabel}) {
        l->setAlignment(Qt::AlignCenter);
        l->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    for (QLabel *l : {compareLeftInfoLabel, compareRightInfoLabel}) {
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet("QLabel { color: black; background-color: transparent; }");
    }

    QVBoxLayout *leftCol = new QVBoxLayout();
    leftCol->addWidget(compareLeftImageLabel, 1);
    leftCol->addWidget(compareLeftInfoLabel);
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->addWidget(compareRightImageLabel, 1);
    rightCol->addWidget(compareRightInfoLabel);
    QHBoxLayout *compareRow = new QHBoxLayout();
    compareRow->addLayout(leftCol, 1);
    compareRow->addLayout(rightCol, 1);
    compareWidget->setLayout(compareRow);
    compareWidget->setVisible(false);

    // Info label (center below image)
    infoLabel = new QLabel(this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    // Folder timestamp label (based on first image modified time)
    dateTimeLabel = new QLabel(this);
    dateTimeLabel->setAlignment(Qt::AlignCenter);
    dateTimeLabel->setStyleSheet("QLabel { color: #003366; background-color: transparent; font-weight: bold; }");
    dateTimeLabel->setText("");

    // Tagging hint label
    taggingHintLabel = new QLabel(this);
    taggingHintLabel->setAlignment(Qt::AlignLeft);
    taggingHintLabel->setStyleSheet("QLabel { color: #003366; background-color: transparent; }");

    // Last saved label
    lastSavedLabel = new QLabel(this);
    lastSavedLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    lastSavedLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    // Navigation buttons
    leftButton = new QPushButton("<", this);
    rightButton = new QPushButton(">", this);
    styleButton(leftButton);
    styleButton(rightButton);
    leftButton->setFixedSize(60, 40);
    rightButton->setFixedSize(60, 40);

    // Slider (re-integrated)
    imageSlider = new QSlider(Qt::Horizontal, this);
    imageSlider->setObjectName("imageSlider");
    imageSlider->setRange(0, 0);
    imageSlider->setSingleStep(1);
    imageSlider->setPageStep(1);
    imageSlider->setTickPosition(QSlider::TicksBelow);

    // Bottom-right index (re-integrated)
    indexLabel = new QLabel("0 / 0", this);
    indexLabel->setObjectName("indexLabel");
    statusBar()->addPermanentWidget(indexLabel, 0);

    // Buttons
    configureTaggingButton = new QPushButton("Configure Image Tagging", this);

    addButton = new QPushButton("Add", this);
    removeButton = new QPushButton("Remove", this);
    saveButton = new QPushButton("Save Lists", this);
    clearButton = new QPushButton("Clear Category", this);

    deleteButton = new QPushButton("Delete", this);
    moveButton = new QPushButton("Move", this);
    copyButton = new QPushButton("Copy", this);

    toggleYoloButton = new QPushButton("Toggle YOLO Bounding Boxes", this);
    loadNamesButton = new QPushButton("Load Class Names", this);
    loadLabelsButton = new QPushButton("Load Labels Directory", this);

        toggleCocoButton = new QPushButton("Toggle COCO Annotations", this);
    loadCocoButton = new QPushButton("Load COCO Folder", this);

    toggleLabelMeButton = new QPushButton("Toggle LabelMe Segmentation", this);
    loadLabelMeButton = new QPushButton("Load LabelMe Folder", this);

    for (QPushButton *b : {configureTaggingButton, addButton, removeButton, saveButton, clearButton,
                           deleteButton, moveButton, copyButton, toggleYoloButton, loadNamesButton, loadLabelsButton, toggleCocoButton, loadCocoButton, toggleLabelMeButton, loadLabelMeButton}) {
        styleButton(b);
        b->setFixedHeight(40);
    }

    // Category tabs
    categoryTabs = new QTabWidget(this);
    categoryTabs->setFixedWidth(320);

    // Default category
    ensureDefaultCategory();
    rebuildCategoryTabs();
    updateTaggingHintLabel();

    // --- Layouts ---
    // Navigation row under image
    QHBoxLayout *navRow = new QHBoxLayout();
    navRow->addStretch();
    navRow->addWidget(leftButton);
    navRow->addWidget(rightButton);
    navRow->addStretch();

    QVBoxLayout *imageCol = new QVBoxLayout();
    imageCol->addWidget(imageLabel, 1);
    imageCol->addWidget(compareWidget, 1);
    imageCol->addWidget(imageSlider);
    imageCol->addLayout(navRow);
    imageCol->addWidget(infoLabel);
    imageCol->addWidget(dateTimeLabel);

    // Right panel
    QVBoxLayout *rightPanel = new QVBoxLayout();
    rightPanel->addWidget(configureTaggingButton);
    rightPanel->addWidget(taggingHintLabel);
    rightPanel->addWidget(categoryTabs, 1);

    // List controls (tagging lists)
    QGroupBox *listGroup = new QGroupBox("List", this);
    QVBoxLayout *listLayout = new QVBoxLayout();
    listLayout->addWidget(addButton);
    listLayout->addWidget(removeButton);
    listLayout->addWidget(saveButton);
    listLayout->addWidget(clearButton);
    listGroup->setLayout(listLayout);
    rightPanel->addWidget(listGroup);
    rightPanel->addWidget(lastSavedLabel);

    // Action controls
    QGroupBox *actionGroup = new QGroupBox("Action", this);
    QVBoxLayout *actionLayout = new QVBoxLayout();
    actionLayout->addWidget(deleteButton);
    actionLayout->addWidget(moveButton);
    actionLayout->addWidget(copyButton);
    actionGroup->setLayout(actionLayout);
    rightPanel->addWidget(actionGroup);

    // YOLO controls
    QGroupBox *yoloGroup = new QGroupBox("YOLO", this);
    QVBoxLayout *yoloLayout = new QVBoxLayout();
    yoloLayout->addWidget(toggleYoloButton);
    yoloLayout->addWidget(loadNamesButton);
    yoloLayout->addWidget(loadLabelsButton);
    yoloGroup->setLayout(yoloLayout);
    rightPanel->addWidget(yoloGroup);

    // COCO controls
    QGroupBox *cocoGroup = new QGroupBox("COCO", this);
    QVBoxLayout *cocoLayout = new QVBoxLayout();
    cocoLayout->addWidget(toggleCocoButton);
    cocoLayout->addWidget(loadCocoButton);
    cocoGroup->setLayout(cocoLayout);
    rightPanel->addWidget(cocoGroup);

    // LabelMe controls
    QGroupBox *labelMeGroup = new QGroupBox("LabelMe", this);
    QVBoxLayout *labelMeLayout = new QVBoxLayout();
    labelMeLayout->addWidget(toggleLabelMeButton);
    labelMeLayout->addWidget(loadLabelMeButton);
    labelMeGroup->setLayout(labelMeLayout);
    rightPanel->addWidget(labelMeGroup);

    // Spacers: fixed left gap, expandable right
    QSpacerItem *leftSpacer = new QSpacerItem(200, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
    QSpacerItem *rightSpacer = new QSpacerItem(50, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    QHBoxLayout *centerRow = new QHBoxLayout();
    centerRow->addItem(leftSpacer);
    centerRow->addLayout(imageCol, 2);
    centerRow->addLayout(rightPanel, 0);
    centerRow->addItem(rightSpacer);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(centerRow);

    QWidget *container = new QWidget(this);
    container->setLayout(mainLayout);
    setCentralWidget(container);

    setMinimumSize(1100, 650);

    // Menu
    QMenuBar *mb = new QMenuBar(this);
    QMenu *fileMenu = new QMenu("File", mb);
    QAction *openDir = new QAction("Open Directory", this);
    connect(openDir, &QAction::triggered, this, &MainWindow::openImageDirectory);
    fileMenu->addAction(openDir);
    mb->addMenu(fileMenu);
    setMenuBar(mb);

    // ------------------------------------------------------------
    // Top toolbar (organized by function)
    // ------------------------------------------------------------
    QToolBar *topBar = new QToolBar("Toolbar", this);
    topBar->setMovable(false);
    topBar->setIconSize(QSize(18, 18));
    addToolBar(Qt::TopToolBarArea, topBar);

    auto makeTbBtn = [this](const QString &text, QStyle::StandardPixmap icon, const QString &tip) {
        QPushButton *b = new QPushButton(text, this);
        b->setIcon(style()->standardIcon(icon));
        b->setToolTip(tip);
        b->setFixedHeight(32);
        // Use same palette as other buttons but slightly tighter
        b->setStyleSheet(
            "QPushButton { background-color: #003366; border: 1px solid #4169e1; border-radius: 5px; "
            "font-size: 13px; font-weight: bold; padding: 6px 12px; color: white; }"
            "QPushButton:hover { background-color: #4682b4; border: 1px solid #315c8a; }"
            "QPushButton:pressed { background-color: #315c8a; border: 1px solid #25485e; }"
        );
        return b;
    };

    // File
    QPushButton *tbOpenDir = makeTbBtn("Load Image Directory", QStyle::SP_DirOpenIcon, "Select a new image directory");
    topBar->addWidget(tbOpenDir);

    QPushButton *tbOpenFolder = makeTbBtn("Open Current Directory", QStyle::SP_DirIcon, "Open the current image folder in your file explorer");
    topBar->addWidget(tbOpenFolder);

    topBar->addSeparator();

    // Navigate
    QPushButton *tbPrev = makeTbBtn("Prev Dir", QStyle::SP_ArrowBack, "Previous directory (sibling folder)");
    topBar->addWidget(tbPrev);

    QPushButton *tbNext = makeTbBtn("Next Dir", QStyle::SP_ArrowForward, "Next directory (sibling folder)");
    topBar->addWidget(tbNext);

    topBar->addSeparator();

    // Actions
    QPushButton *tbCopy = makeTbBtn("Copy", QStyle::SP_DialogSaveButton, "Copy selected images (per-category destinations)");
    topBar->addWidget(tbCopy);

    QPushButton *tbMove = makeTbBtn("Move", QStyle::SP_ArrowRight, "Move selected images (per-category destinations)");
    topBar->addWidget(tbMove);

    QPushButton *tbDelete = makeTbBtn("Delete", QStyle::SP_TrashIcon, "Delete selected images (implemented as move to chosen folders)");
    topBar->addWidget(tbDelete);

    topBar->addSeparator();

    // YOLO
    QPushButton *tbYoloToggle = new QPushButton("YOLO", this);
    tbYoloToggle->setToolTip("Toggle YOLO bounding boxes");
    tbYoloToggle->setFixedHeight(32);
    topBar->addWidget(tbYoloToggle);

    QPushButton *tbLoadNames = makeTbBtn("Load Names", QStyle::SP_FileIcon, "Load .names file for class labels");
    topBar->addWidget(tbLoadNames);

    QPushButton *tbLoadLabels = makeTbBtn("Load Labels", QStyle::SP_DirOpenIcon, "Select a labels directory containing YOLO .txt files (optional)");
    topBar->addWidget(tbLoadLabels);

    topBar->addSeparator();

    QPushButton *tbCompare = makeTbBtn("Compare", QStyle::SP_FileDialogDetailedView, "Compare two model output folders side-by-side");
    topBar->addWidget(tbCompare);

    QPushButton *tbExitCompare = makeTbBtn("Exit Compare", QStyle::SP_DialogCloseButton, "Exit compare mode");
    topBar->addWidget(tbExitCompare);

    auto syncTbYoloToggle = [this, tbYoloToggle]() {
        if (showYoloBoundingBoxes) {
            tbYoloToggle->setStyleSheet(
                "QPushButton { background-color: #1f7a1f; border: 1px solid #155315; border-radius: 5px; "
                "font-size: 13px; font-weight: bold; padding: 6px 12px; color: white; }"
                "QPushButton:hover { background-color: #2b9a2b; }"
                "QPushButton:pressed { background-color: #155315; }"
            );
        } else {
            tbYoloToggle->setStyleSheet(
                "QPushButton { background-color: #6b6b6b; border: 1px solid #4f4f4f; border-radius: 5px; "
                "font-size: 13px; font-weight: bold; padding: 6px 12px; color: white; }"
                "QPushButton:hover { background-color: #7a7a7a; }"
                "QPushButton:pressed { background-color: #4f4f4f; }"
            );
        }
    };
    syncTbYoloToggle();

    // Toolbar connections
    connect(tbOpenDir, &QPushButton::clicked, this, &MainWindow::openImageDirectory);
    connect(tbOpenFolder, &QPushButton::clicked, this, &MainWindow::openCurrentImageFolderInExplorer);

    connect(tbPrev, &QPushButton::clicked, this, &MainWindow::showPreviousDirectory);
    connect(tbNext, &QPushButton::clicked, this, &MainWindow::showNextDirectory);

    connect(tbCopy, &QPushButton::clicked, this, &MainWindow::copySelectedImages);
    connect(tbMove, &QPushButton::clicked, this, &MainWindow::moveSelectedImage);
    connect(tbDelete, &QPushButton::clicked, this, &MainWindow::deleteSelectedImage);

    connect(tbYoloToggle, &QPushButton::clicked, this, [this, syncTbYoloToggle]() {
        toggleYoloBoundingBoxes();
        syncTbYoloToggle();
    });
    connect(tbLoadNames, &QPushButton::clicked, this, &MainWindow::on_loadNamesFileButton_clicked);
    connect(tbLoadLabels, &QPushButton::clicked, this, &MainWindow::on_loadLabelsDirButton_clicked);
    connect(tbCompare, &QPushButton::clicked, this, &MainWindow::startCompareMode);
    connect(tbExitCompare, &QPushButton::clicked, this, &MainWindow::exitCompareMode);

    mb->setStyleSheet("QMenuBar { background-color: #003366; color: white; }"
                      "QMenuBar::item { color: white; }"
                      "QMenuBar::item:selected { background-color: #4682b4; }"
                      "QMenu::item:selected { background-color: #00509E; }");

    // Connections
    connect(leftButton, &QPushButton::clicked, this, &MainWindow::showPreviousImage);
    connect(rightButton, &QPushButton::clicked, this, &MainWindow::showNextImage);

    connect(imageSlider, &QSlider::valueChanged, this, [this](int v){
        if (compareMode) {
            if (compareImageList.isEmpty()) return;
            v = std::clamp(v, 0, compareImageList.size()-1);
            compareIndex = v;
            updateCompareView();
            return;
        }
        if (imageList.isEmpty()) return;
        v = std::clamp(v, 0, imageList.size()-1);
        currentImageIndex = v;
        updateImage();
    });

    connect(configureTaggingButton, &QPushButton::clicked, this, &MainWindow::openTaggingConfig);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addImageToList);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeImageFromList);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveImageList);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearImageList);

    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedImage);
    connect(moveButton, &QPushButton::clicked, this, &MainWindow::moveSelectedImage);
    connect(copyButton, &QPushButton::clicked, this, &MainWindow::copySelectedImages);

    connect(toggleYoloButton, &QPushButton::clicked, this, &MainWindow::toggleYoloBoundingBoxes);
    connect(toggleCocoButton, &QPushButton::clicked, this, &MainWindow::toggleCocoAnnotations);
    connect(loadCocoButton, &QPushButton::clicked, this, &MainWindow::on_loadCocoJsonButton_clicked);
    connect(toggleLabelMeButton, &QPushButton::clicked, this, &MainWindow::toggleLabelMeAnnotations);
    connect(loadLabelMeButton, &QPushButton::clicked, this, &MainWindow::on_loadLabelMeDirButton_clicked);
    connect(loadNamesButton, &QPushButton::clicked, this, &MainWindow::on_loadNamesFileButton_clicked);
    connect(loadLabelsButton, &QPushButton::clicked, this, &MainWindow::on_loadLabelsDirButton_clicked);

    // Logging
    const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    const QString logName = QString("log_%1.txt").arg(timestamp);
    logFile.setFileName(logName);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logStream.setDevice(&logFile);
        logActivity("Application started.");
    }

    // Startup: prompt for folder
	    if (compareMode) exitCompareMode();
	    loadImagesFromDirectory();
    updateImage();

    // initial YOLO toggle style
    updateToggleYoloButtonStyle();

    setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    if (logFile.isOpen()) {
        logActivity("Application closed.");
        logFile.close();
    }
}

// ------------------------------------------------------------
// Events
// ------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int k = event->key();

    if (k == Qt::Key_Right) { showNextImage(); return; }
    if (k == Qt::Key_Left)  { showPreviousImage(); return; }

    // Re-integrated: 'a' adds to current category + advances
    if (k == Qt::Key_A) {
        addCurrentImageToCategory(currentCategory(), true);
        return;
    }

    // Tagging keys: add to mapped category + advance
    if (keyToCategory.contains(k)) {
        addCurrentImageToCategory(keyToCategory.value(k), true);
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateImage();
}

// ------------------------------------------------------------
// Directory / navigation
// ------------------------------------------------------------
void MainWindow::loadImagesFromDirectory()
{
    const QString dirPath = QFileDialog::getExistingDirectory(this, "Select Directory", "");
    if (dirPath.isEmpty()) return;

    loadImagesFromDirectoryPath(dirPath, true);
}

bool MainWindow::loadImagesFromDirectoryPath(const QString &dirPath, bool logIt)
{
    if (dirPath.isEmpty()) return false;

    directory.setPath(dirPath);

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG" << "*.PNG";

    imageList = directory.entryList(filters, QDir::Files, QDir::Name);
    currentImageIndex = 0;

    imageSlider->setRange(0, std::max(0, imageList.size() - 1));
    imageSlider->blockSignals(true);
    imageSlider->setValue(currentImageIndex);
    imageSlider->blockSignals(false);

    indexLabel->setText(imageList.isEmpty() ? "0 / 0" : QString("1 / %1").arg(imageList.size()));

    updateDirectoryNameLabel();

    updateFolderDateTimeLabel();

    // Keep category tabs as-is, but ensure view refresh
    updateImage();

    if (logIt) logActivity("Loaded images from directory: " + dirPath);
    return true;
}

QStringList MainWindow::siblingDirectories(QString *outCurrentName) const
{
    // Return sibling directory NAMES under the parent of the current image directory,
    // sorted by name (QDir::Name). Also returns the current directory name.
    const QString curAbs = directory.absolutePath();
    if (curAbs.isEmpty()) return {};

    // Canonical path helps avoid mismatch due to symlinks/trailing slashes.
    const QString curCanonical = QFileInfo(curAbs).canonicalFilePath().isEmpty()
                                     ? QDir(curAbs).absolutePath()
                                     : QFileInfo(curAbs).canonicalFilePath();

    QFileInfo curInfo(curCanonical);
    const QString curName = curInfo.fileName();
    if (outCurrentName) *outCurrentName = curName;

    QDir parentDir = curInfo.dir(); // parent of current directory
    if (!parentDir.exists()) return {};

    // List sibling directories (names only)
    QStringList dirs = parentDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    return dirs;
}

void MainWindow::showPreviousDirectory()
{
    QString curName;
    const QStringList dirs = siblingDirectories(&curName);
    if (dirs.isEmpty() || curName.isEmpty()) return;

    int idx = dirs.indexOf(curName);
    if (idx < 0) {
        // If the name doesn't match, try matching by canonical path basename
        const QString base = QFileInfo(directory.absolutePath()).fileName();
        idx = dirs.indexOf(base);
    }
    if (idx <= 0) return;

    const QString curAbs = directory.absolutePath();
    const QString curCanonical = QFileInfo(curAbs).canonicalFilePath().isEmpty()
                                     ? QDir(curAbs).absolutePath()
                                     : QFileInfo(curAbs).canonicalFilePath();
    QDir parentDir = QFileInfo(curCanonical).dir(); // parent of current directory

    const QString target = parentDir.filePath(dirs[idx - 1]);
    if (!loadImagesFromDirectoryPath(target, true)) return;

    pruneMissingFiles();
    rebuildCategoryTabs();
    updateImage();
}

void MainWindow::showNextDirectory()
{
    QString curName;
    const QStringList dirs = siblingDirectories(&curName);
    if (dirs.isEmpty() || curName.isEmpty()) return;

    int idx = dirs.indexOf(curName);
    if (idx < 0) {
        const QString base = QFileInfo(directory.absolutePath()).fileName();
        idx = dirs.indexOf(base);
    }
    if (idx < 0 || idx >= dirs.size() - 1) return;

    const QString curAbs = directory.absolutePath();
    const QString curCanonical = QFileInfo(curAbs).canonicalFilePath().isEmpty()
                                     ? QDir(curAbs).absolutePath()
                                     : QFileInfo(curAbs).canonicalFilePath();
    QDir parentDir = QFileInfo(curCanonical).dir(); // parent of current directory

    const QString target = parentDir.filePath(dirs[idx + 1]);
    if (!loadImagesFromDirectoryPath(target, true)) return;

    pruneMissingFiles();
    rebuildCategoryTabs();
    updateImage();
}

void MainWindow::openImageDirectory()
{
    loadImagesFromDirectory();
    pruneMissingFiles();
    rebuildCategoryTabs();
    updateImage();
	}

void MainWindow::showPreviousImage()
{
    if (compareMode) {
        if (compareImageList.isEmpty()) return;
        if (compareIndex > 0) {
            --compareIndex;
            imageSlider->blockSignals(true);
            imageSlider->setValue(compareIndex);
            imageSlider->blockSignals(false);
            updateCompareView();
        }
        return;
    }

    if (imageList.isEmpty()) return;

    if (currentImageIndex > 0) {
        --currentImageIndex;
        imageSlider->blockSignals(true);
        imageSlider->setValue(currentImageIndex);
        imageSlider->blockSignals(false);
        updateImage();
        logActivity("Navigated to previous image.");
    }
}

void MainWindow::showNextImage()
{
    if (compareMode) {
        if (compareImageList.isEmpty()) return;
        if (compareIndex < compareImageList.size() - 1) {
            ++compareIndex;
            imageSlider->blockSignals(true);
            imageSlider->setValue(compareIndex);
            imageSlider->blockSignals(false);
            updateCompareView();
        }
        return;
    }

    if (imageList.isEmpty()) return;

    if (currentImageIndex < imageList.size() - 1) {
        ++currentImageIndex;
        imageSlider->blockSignals(true);
        imageSlider->setValue(currentImageIndex);
        imageSlider->blockSignals(false);
        updateImage();
        logActivity("Navigated to next image.");
    }
}

// ------------------------------------------------------------
// Image refresh (draw + info + counters)
// ------------------------------------------------------------
void MainWindow::updateImage()
{
    if (compareMode) {
        updateCompareView();
        return;
    }
    if (imageList.isEmpty()) return;

    const QString imagePath = directory.filePath(imageList.at(currentImageIndex));
    QImage image(imagePath);
    if (image.isNull()) return;

    currentImage = image;

    // Apply YOLO overlay from effective labels directory (can be separate from images)
    if (showYoloBoundingBoxes) {
        const QString labelsDir = effectiveLabelsDirForSingleView();
        image = renderBoundingBoxesOn(image, imagePath, labelsDir);
    }

    // Apply COCO overlay (bbox + segmentation polygons), if enabled
    if (showCocoAnnotations && !cocoJsonPath.isEmpty()) {
        image = renderCocoForPath(image, imagePath, cocoJsonPath, cocoCacheSingle);
    }

    // Apply LabelMe overlay (polygon/rectangle), if enabled
    if (showLabelMeAnnotations) {
        const QString lmDir = labelMeJsonDirPath.isEmpty() ? directory.absolutePath() : labelMeJsonDirPath;
        image = renderLabelMeOn(image, imagePath, lmDir);
    }

    imageLabel->setPixmap(QPixmap::fromImage(image)
                              .scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    const QString imageName = "Image: " + imageList.at(currentImageIndex);
    QString resolution = QString("Resolution: %1 x %2").arg(image.width()).arg(image.height());
    if (!yoloLabelsDirPath.trimmed().isEmpty()) {
        resolution += "<br>YOLO labels: " + yoloLabelsDirPath;
    }
    if (cocoCacheSingle.loaded && !cocoJsonPath.trimmed().isEmpty()) {
        resolution += "<br>COCO: " + cocoJsonPath;
    }
    infoLabel->setText(imageName + "<br>" + resolution);
}

// ------------------------------------------------------------
// YOLO helpers
// ------------------------------------------------------------
QString MainWindow::getClassName(int classId) const
{
    if (classId >= 0 && classId < classNames.size()) return classNames.at(classId);
    return QString("Class %1").arg(classId);
}

void MainWindow::loadClassNames(const QString &namesFilePath)
{
    classNames.clear();
    QFile f(namesFilePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Load Names", "Failed to open .names file.");
        return;
    }
    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (!line.isEmpty()) classNames << line;
    }
    f.close();
    logActivity("Loaded class names: " + namesFilePath);
}

void MainWindow::on_loadNamesFileButton_clicked()
{
    const QString file = QFileDialog::getOpenFileName(this, "Select .names file", "", "Names (*.names);;All (*)");
    if (file.isEmpty()) return;
    loadClassNames(file);
    updateImage();
}

void MainWindow::on_loadLabelsDirButton_clicked()
{
    const QString dir = QFileDialog::getExistingDirectory(this, "Select Labels Directory (YOLO .txt)");
    if (dir.isEmpty()) return;
    yoloLabelsDirPath = dir;
    logActivity("Set labels directory: " + yoloLabelsDirPath);
    updateImage();
}

QString MainWindow::effectiveLabelsDirForSingleView() const
{
    if (!yoloLabelsDirPath.trimmed().isEmpty()) return yoloLabelsDirPath;
    return directory.absolutePath();
}

QImage MainWindow::renderBoundingBoxesOn(const QImage &img,
                                        const QString &imagePath,
                                        const QString &labelsDir) const
{
    QImage out = img.convertToFormat(QImage::Format_RGB32);

    const int W = out.width();
    const int H = out.height();

    const QString base = QFileInfo(imagePath).completeBaseName();
    const QString txtPath = QDir(labelsDir).filePath(base + ".txt");
    QFile file(txtPath);
    if (!file.exists()) return out;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return out;

    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing, true);

    QVector<QRect> placedLabelRects;
    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        const QStringList values = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (values.size() < 5) continue;

        bool ok0=false, ok1=false, ok2=false, ok3=false, ok4=false, ok5=true;
        const int cls = values[0].toInt(&ok0);
        const double xc = values[1].toDouble(&ok1);
        const double yc = values[2].toDouble(&ok2);
        const double ww = values[3].toDouble(&ok3);
        const double hh = values[4].toDouble(&ok4);
        float conf = 0.0f;
        if (values.size() >= 6) conf = values[5].toFloat(&ok5);
        if (!(ok0 && ok1 && ok2 && ok3 && ok4 && ok5)) continue;

        const int x = int((xc - ww/2.0) * W);
        const int y = int((yc - hh/2.0) * H);
        const int w = int(ww * W);
        const int h = int(hh * H);
        const QRect bbox(x, y, w, h);

        const QString clsName = getClassName(cls);
        QString label = clsName;
        if (conf > 0.0f) label += QString(" (%1)").arg(conf, 0, 'f', 2);

        QColor c = classColors.contains(cls) ? classColors.value(cls) : QColor("#00FF00");
        QPen pen(c, 3);
        p.setPen(pen);
        p.drawRect(bbox);

        QFont font = p.font();
        font.setBold(true);
        font.setPointSize(10);
        p.setFont(font);

        const int pad = 4;
        QRect textRect = p.fontMetrics().boundingRect(label);
        textRect.adjust(-pad, -pad, pad, pad);

        int tx = bbox.left();
        int ty = bbox.top() - textRect.height() - 2;

        QRect labelRect(tx, ty, std::max(60, textRect.width()), textRect.height());
        // avoid collisions with previous labels
        for (int iter = 0; iter < 20; ++iter) {
            bool collide = false;
            for (const QRect &r : placedLabelRects) {
                if (labelRect.intersects(r)) { collide = true; break; }
            }
            if (!collide) break;
            ty -= (labelRect.height() + 2);
            labelRect.moveTop(ty);
        }
        if (labelRect.top() < 0) {
            // fallback: draw inside bbox
            labelRect.moveTop(bbox.top() + 2);
        }
        placedLabelRects.push_back(labelRect);

        p.fillRect(labelRect, QColor(0, 0, 0, 140));
        p.setPen(Qt::white);
        p.drawText(labelRect.adjusted(pad, 0, -pad, 0), Qt::AlignVCenter, label);
    }

    p.end();
    file.close();
    return out;
}

void MainWindow::toggleYoloBoundingBoxes()
{
    showYoloBoundingBoxes = !showYoloBoundingBoxes;
    updateToggleYoloButtonStyle();
    if (compareMode) updateCompareView();
    else updateImage();
    logActivity(QString("YOLO bounding boxes %1").arg(showYoloBoundingBoxes ? "ON" : "OFF"));
}


// ------------------------------------------------------------
// COCO
// ------------------------------------------------------------
static QColor cocoColorForId(int id)
{
    const int hue = (id * 47) % 360;
    return QColor::fromHsv(hue, 220, 230);
}

// Decode COCO uncompressed RLE (counts array) into an alpha mask.
// COCO RLE is column-major (Fortran order): index i maps to (x=i/h, y=i%h)
static QImage decodeCocoRleCountsArray(const QJsonArray &counts, int h, int w)
{
    if (h <= 0 || w <= 0 || counts.isEmpty()) return QImage();

    QImage mask(w, h, QImage::Format_Alpha8);
    mask.fill(0);

    const int total = h * w;
    int idx = 0;
    int val = 0; // starts with background

    for (const QJsonValue &cv : counts) {
        const int run = cv.toInt(0);
        if (run <= 0) continue;
        const int end = std::min(total, idx + run);
        if (val == 1) {
            for (int i = idx; i < end; ++i) {
                const int x = i / h;
                const int y = i % h;
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    mask.scanLine(y)[x] = 255;
                }
            }
        }
        idx = end;
        val = 1 - val;
        if (idx >= total) break;
    }

    return mask;
}

QString MainWindow::imageFileNameKey(const QString &imagePath) const
{
    // COCO image file_name may include subfolders; we key by basename for robustness.
    return QFileInfo(imagePath).fileName();
}

bool MainWindow::loadCocoJson(const QString &jsonPath, CocoCache &cache)
{
    cache = CocoCache{};
    cache.jsonPath = jsonPath;

    QFile f(jsonPath);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "COCO", "Failed to open COCO JSON file.");
        return false;
    }

    const QByteArray bytes = f.readAll();
    f.close();

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(bytes, &err);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "COCO", "Invalid COCO JSON (parse error).\n" + err.errorString());
        return false;
    }

    const QJsonObject root = doc.object();

    // categories
    const QJsonArray categories = root.value("categories").toArray();
    for (const QJsonValue &v : categories) {
        const QJsonObject o = v.toObject();
        const int id = o.value("id").toInt(-1);
        const QString name = o.value("name").toString();
        if (id >= 0 && !name.isEmpty()) cache.categoryIdToName.insert(id, name);
    }

    // images
    const QJsonArray images = root.value("images").toArray();
    for (const QJsonValue &v : images) {
        const QJsonObject o = v.toObject();
        const int id = o.value("id").toInt(-1);
        const QString fileName = o.value("file_name").toString();
        if (id < 0 || fileName.isEmpty()) continue;
        cache.fileNameToImageId.insert(fileName, id);
        const QString base = QFileInfo(fileName).fileName();
        if (!base.isEmpty()) cache.fileNameToImageId.insert(base, id);
    }

    // annotations
    const QJsonArray anns = root.value("annotations").toArray();
    for (const QJsonValue &v : anns) {
        const QJsonObject o = v.toObject();
        const int imageId = o.value("image_id").toInt(-1);
        const int catId = o.value("category_id").toInt(-1);
        if (imageId < 0 || catId < 0) continue;

        CocoAnnotation ann;
        ann.categoryId = catId;
        ann.categoryName = cache.categoryIdToName.value(catId, QString("cat_%1").arg(catId));

        const QJsonArray bbox = o.value("bbox").toArray();
        if (bbox.size() == 4) {
            const double x = bbox.at(0).toDouble();
            const double y = bbox.at(1).toDouble();
            const double w = bbox.at(2).toDouble();
            const double h = bbox.at(3).toDouble();
            ann.bbox = QRectF(x, y, w, h);
        }

        // segmentation (polygons)
        const QJsonValue segV = o.value("segmentation");
        if (segV.isArray()) {
            const QJsonArray segA = segV.toArray();
            if (!segA.isEmpty() && segA.at(0).isArray()) {
                // list of polygons
                for (const QJsonValue &polyV : segA) {
                    const QJsonArray polyA = polyV.toArray();
                    QPolygonF poly;
                    for (int i = 0; i + 1 < polyA.size(); i += 2) {
                        poly << QPointF(polyA.at(i).toDouble(), polyA.at(i + 1).toDouble());
                    }
                    if (poly.size() >= 3) ann.polygons.append(poly);
                }
            } else {
                // single polygon flat list
                QPolygonF poly;
                for (int i = 0; i + 1 < segA.size(); i += 2) {
                    poly << QPointF(segA.at(i).toDouble(), segA.at(i + 1).toDouble());
                }
                if (poly.size() >= 3) ann.polygons.append(poly);
            }
        }
        // segmentation as RLE
        // COCO allows segmentation to be RLE: {"counts": [...], "size": [h,w]} or compressed string.
        if (segV.isObject()) {
            const QJsonObject segO = segV.toObject();
            const QJsonArray sizeA = segO.value("size").toArray();
            const int h = sizeA.size() >= 1 ? sizeA.at(0).toInt(0) : 0;
            const int w = sizeA.size() >= 2 ? sizeA.at(1).toInt(0) : 0;

            const QJsonValue countsV = segO.value("counts");
            if (countsV.isArray()) {
                // Uncompressed RLE counts array
                ann.rleMask = decodeCocoRleCountsArray(countsV.toArray(), h, w);
            } else {
                // Compressed RLE string needs the COCO RLE decode algorithm (pycocotools).
                // We fall back to bbox-only rendering in this case.
            }
        }

        cache.imageIdToAnns[imageId].append(ann);
    }

    cache.loaded = true;
    return true;
}

QImage MainWindow::renderCocoOn(const QImage &img, const QString &imagePath, const CocoCache &cache) const
{
    if (!cache.loaded) return img;

    const QString key = imageFileNameKey(imagePath);
    const int imageId = cache.fileNameToImageId.value(key, -1);
    if (imageId < 0) return img;

    const QVector<CocoAnnotation> anns = cache.imageIdToAnns.value(imageId);
    if (anns.isEmpty()) return img;

    QImage out = img.convertToFormat(QImage::Format_RGB32);
    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing, true);

    for (const CocoAnnotation &ann : anns) {
        const QColor c = cocoColorForId(ann.categoryId);

        // segmentation polygons (fill)
        if (!ann.polygons.isEmpty()) {
            QColor fill = c;
            fill.setAlpha(60);
            p.setBrush(QBrush(fill));
            p.setPen(QPen(c, 2));
            for (const QPolygonF &poly : ann.polygons) {
                p.drawPolygon(poly);
            }
            p.setBrush(Qt::NoBrush);
        }

        // RLE mask (alpha) fill
        if (!ann.rleMask.isNull()) {
            QImage m = ann.rleMask;
            if (m.width() != out.width() || m.height() != out.height()) {
                m = m.scaled(out.size(), Qt::IgnoreAspectRatio, Qt::FastTransformation);
            }

            QImage overlay(out.size(), QImage::Format_ARGB32_Premultiplied);
            overlay.fill(Qt::transparent);
            const QColor cFill(c.red(), c.green(), c.blue(), 80);

            for (int y = 0; y < overlay.height(); ++y) {
                const uchar *a = m.constScanLine(y);
                QRgb *dst = reinterpret_cast<QRgb*>(overlay.scanLine(y));
                for (int x = 0; x < overlay.width(); ++x) {
                    if (a[x] > 0) dst[x] = qRgba(cFill.red(), cFill.green(), cFill.blue(), cFill.alpha());
                }
            }
            p.drawImage(0, 0, overlay);
        }

        // bbox
        if (ann.bbox.isValid()) {
            p.setPen(QPen(c, 3));
            p.drawRect(ann.bbox);

            // label
            const QString label = ann.categoryName;
            QFont font = p.font();
            font.setBold(true);
            font.setPointSize(10);
            p.setFont(font);

            const int pad = 4;
            QRect tr = p.fontMetrics().boundingRect(label);
            QRect bg(int(ann.bbox.x()), int(ann.bbox.y()) - tr.height() - 2*pad,
                     tr.width() + 2*pad, tr.height() + 2*pad);
            if (bg.y() < 0) bg.moveTop(int(ann.bbox.y()) + 2);

            QColor bgc = c;
            bgc.setAlpha(190);
            p.fillRect(bg, bgc);
            p.setPen(Qt::white);
            p.drawText(bg.adjusted(pad, pad, -pad, -pad), Qt::AlignLeft | Qt::AlignVCenter, label);
        }
    }

    p.end();
    return out;
}


// Load a per-image COCO JSON file (one JSON per image). Expected structure:
// { "annotations": [ { "bbox": [...], "category_id": n, "segmentation": ... }, ... ],
//   "categories": [ { "id": n, "name": "..." }, ... ]  (optional)
// }
bool MainWindow::loadCocoPerImageJson(const QString &jsonPath,
                                      QVector<CocoAnnotation> &outAnns,
                                      QHash<int, QString> &outCatMap) const
{
    outAnns.clear();
    outCatMap.clear();

    QFile f(jsonPath);
    if (!f.open(QIODevice::ReadOnly)) return false;

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    f.close();

    if (doc.isNull() || !doc.isObject()) return false;

    const QJsonObject root = doc.object();

    // categories (optional)
    const QJsonArray categories = root.value("categories").toArray();
    for (const QJsonValue &v : categories) {
        const QJsonObject o = v.toObject();
        const int id = o.value("id").toInt(-1);
        const QString name = o.value("name").toString();
        if (id >= 0 && !name.isEmpty()) outCatMap.insert(id, name);
    }

    const QJsonArray anns = root.value("annotations").toArray();
    if (anns.isEmpty()) return false;

    for (const QJsonValue &v : anns) {
        const QJsonObject o = v.toObject();
        const int catId = o.value("category_id").toInt(-1);
        if (catId < 0) continue;

        CocoAnnotation ann;
        ann.categoryId = catId;
        ann.categoryName = outCatMap.value(catId, QString("cat_%1").arg(catId));

        const QJsonArray bbox = o.value("bbox").toArray();
        if (bbox.size() == 4) {
            const double x = bbox.at(0).toDouble();
            const double y = bbox.at(1).toDouble();
            const double w = bbox.at(2).toDouble();
            const double h = bbox.at(3).toDouble();
            ann.bbox = QRectF(x, y, w, h);
        }

        const QJsonValue segV = o.value("segmentation");

        // polygons
        if (segV.isArray()) {
            const QJsonArray segA = segV.toArray();
            if (!segA.isEmpty() && segA.at(0).isArray()) {
                for (const QJsonValue &polyV : segA) {
                    const QJsonArray polyA = polyV.toArray();
                    QPolygonF poly;
                    for (int i = 0; i + 1 < polyA.size(); i += 2) {
                        poly << QPointF(polyA.at(i).toDouble(), polyA.at(i + 1).toDouble());
                    }
                    if (poly.size() >= 3) ann.polygons.append(poly);
                }
            } else {
                QPolygonF poly;
                for (int i = 0; i + 1 < segA.size(); i += 2) {
                    poly << QPointF(segA.at(i).toDouble(), segA.at(i + 1).toDouble());
                }
                if (poly.size() >= 3) ann.polygons.append(poly);
            }
        }

        // RLE object
        if (segV.isObject()) {
            const QJsonObject segO = segV.toObject();
            const QJsonArray sizeA = segO.value("size").toArray();
            const int h = sizeA.size() >= 1 ? sizeA.at(0).toInt(0) : 0;
            const int w = sizeA.size() >= 2 ? sizeA.at(1).toInt(0) : 0;
            const QJsonValue countsV = segO.value("counts");
            if (countsV.isArray()) {
                ann.rleMask = decodeCocoRleCountsArray(countsV.toArray(), h, w);
            }
        }

        outAnns.append(ann);
    }

    return !outAnns.isEmpty();
}

QImage MainWindow::renderCocoForPath(const QImage &img,
                                    const QString &imagePath,
                                    const QString &cocoPathOrDir,
                                    CocoCache &cache) const
{
    if (cocoPathOrDir.isEmpty()) return img;

    const QFileInfo fi(cocoPathOrDir);
    if (fi.exists() && fi.isFile()) {
        // Standard single COCO instances JSON
        if (!cache.loaded || cache.jsonPath != cocoPathOrDir) {
            MainWindow *self = const_cast<MainWindow*>(this);
            self->loadCocoJson(cocoPathOrDir, cache);
        }
        return cache.loaded ? renderCocoOn(img, imagePath, cache) : img;
    }

    if (!(fi.exists() && fi.isDir())) return img;

    // Per-image JSON folder: try <completeBaseName>.json and <fileName>.json
    const QFileInfo imgFi(imagePath);
    const QString candidate1 = QDir(cocoPathOrDir).filePath(imgFi.completeBaseName() + ".json");
    const QString candidate2 = QDir(cocoPathOrDir).filePath(imgFi.fileName() + ".json");

    QString jsonFile;
    if (QFileInfo::exists(candidate1)) jsonFile = candidate1;
    else if (QFileInfo::exists(candidate2)) jsonFile = candidate2;
    else return img;

    QVector<CocoAnnotation> anns;
    QHash<int, QString> cats;
    if (!loadCocoPerImageJson(jsonFile, anns, cats)) return img;

    QImage out = img.convertToFormat(QImage::Format_RGB32);
    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing, true);

    for (const CocoAnnotation &ann : anns) {
        const QColor c = cocoColorForId(ann.categoryId);

        if (!ann.polygons.isEmpty()) {
            QColor fill = c; fill.setAlpha(60);
            p.setBrush(QBrush(fill));
            p.setPen(QPen(c, 2));
            for (const QPolygonF &poly : ann.polygons) p.drawPolygon(poly);
            p.setBrush(Qt::NoBrush);
        }

        if (!ann.rleMask.isNull()) {
            QImage m = ann.rleMask;
            if (m.size() != out.size()) m = m.scaled(out.size(), Qt::IgnoreAspectRatio, Qt::FastTransformation);

            QImage overlay(out.size(), QImage::Format_ARGB32_Premultiplied);
            overlay.fill(Qt::transparent);
            const QColor cFill(c.red(), c.green(), c.blue(), 80);

            for (int y = 0; y < overlay.height(); ++y) {
                const uchar *a = m.constScanLine(y);
                QRgb *dst = reinterpret_cast<QRgb*>(overlay.scanLine(y));
                for (int x = 0; x < overlay.width(); ++x) {
                    if (a[x] > 0) dst[x] = qRgba(cFill.red(), cFill.green(), cFill.blue(), cFill.alpha());
                }
            }
            p.drawImage(0, 0, overlay);
        }

        if (ann.bbox.isValid()) {
            p.setPen(QPen(c, 3));
            p.drawRect(ann.bbox);

            const QString label = ann.categoryName.isEmpty() ? QString("cat_%1").arg(ann.categoryId) : ann.categoryName;
            QFont font = p.font(); font.setBold(true); font.setPointSize(10);
            p.setFont(font);

            const int pad = 4;
            QRect tr = p.fontMetrics().boundingRect(label);
            QRect bg(int(ann.bbox.x()), int(ann.bbox.y()) - tr.height() - 2*pad,
                     tr.width() + 2*pad, tr.height() + 2*pad);
            if (bg.y() < 0) bg.moveTop(int(ann.bbox.y()) + 2);

            QColor bgc = c; bgc.setAlpha(190);
            p.fillRect(bg, bgc);
            p.setPen(Qt::white);
            p.drawText(bg.adjusted(pad, pad, -pad, -pad), Qt::AlignLeft | Qt::AlignVCenter, label);
        }
    }

    p.end();
    return out;
}

void MainWindow::toggleCocoAnnotations()
{
    showCocoAnnotations = !showCocoAnnotations;

    if (toggleCocoButton) {
        if (showCocoAnnotations) {
            toggleCocoButton->setStyleSheet(
                "QPushButton { background-color: #1E90FF; color: white; border-radius: 6px; font-size: 14px; }"
                "QPushButton:hover { background-color: #187bcd; }");
        } else {
            styleButton(toggleCocoButton);
            toggleCocoButton->setFixedHeight(40);
        }
    }

    if (compareMode) updateCompareView();
    else updateImage();

    logActivity(QString("COCO annotations %1").arg(showCocoAnnotations ? "ON" : "OFF"));
}


void MainWindow::toggleLabelMeAnnotations()
{
    showLabelMeAnnotations = !showLabelMeAnnotations;

    if (toggleLabelMeButton) {
        if (showLabelMeAnnotations) {
            toggleLabelMeButton->setStyleSheet(
                "QPushButton { background-color: #2E8B57; color: white; font-weight: bold; border-radius: 6px; }"
                "QPushButton:hover { background-color: #3CB371; }"
            );
        } else {
            styleButton(toggleLabelMeButton);
        }
    }

    updateImage();
}

void MainWindow::on_loadLabelMeDirButton_clicked()
{
    const QString dir = QFileDialog::getExistingDirectory(this, "Select LabelMe JSON folder");
    if (dir.isEmpty()) return;

    labelMeJsonDirPath = dir;

    logStream << "[LabelMe] Labels folder set: " << dir << "\n";
    logStream.flush();

    updateImage();
}

QVector<LabelMeShape> MainWindow::loadLabelMeForImage(const QString &imagePath, const QString &labelMeDir) const
{
    QVector<LabelMeShape> out;
    if (labelMeDir.isEmpty()) return out;

    const QString base = QFileInfo(imagePath).completeBaseName();
    QString jsonPath = QDir(labelMeDir).filePath(base + ".json");
    if (!QFileInfo::exists(jsonPath)) {
        // also try <filename>.json
        jsonPath = QDir(labelMeDir).filePath(QFileInfo(imagePath).fileName() + ".json");
        if (!QFileInfo::exists(jsonPath)) return out;
    }

    QFile f(jsonPath);
    if (!f.open(QIODevice::ReadOnly)) return out;
    const QByteArray data = f.readAll();
    f.close();

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return out;

    const QJsonObject root = doc.object();
    const QJsonArray shapes = root.value("shapes").toArray();

    for (const QJsonValue &sv : shapes) {
        if (!sv.isObject()) continue;
        const QJsonObject s = sv.toObject();
        const QString label = s.value("label").toString();
        const QString shapeType = s.value("shape_type").toString("polygon");
        const QJsonArray pts = s.value("points").toArray();
        if (pts.size() < 2) continue;

        QPolygonF poly;

        if (shapeType == "rectangle" && pts.size() >= 2) {
            const QJsonArray p0 = pts.at(0).toArray();
            const QJsonArray p1 = pts.at(1).toArray();
            if (p0.size() < 2 || p1.size() < 2) continue;

            const double x0 = p0.at(0).toDouble();
            const double y0 = p0.at(1).toDouble();
            const double x1 = p1.at(0).toDouble();
            const double y1 = p1.at(1).toDouble();

            const double xmin = std::min(x0, x1);
            const double xmax = std::max(x0, x1);
            const double ymin = std::min(y0, y1);
            const double ymax = std::max(y0, y1);

            poly << QPointF(xmin, ymin)
                 << QPointF(xmax, ymin)
                 << QPointF(xmax, ymax)
                 << QPointF(xmin, ymax);
        } else {
            for (const QJsonValue &pv : pts) {
                const QJsonArray p = pv.toArray();
                if (p.size() < 2) continue;
                poly << QPointF(p.at(0).toDouble(), p.at(1).toDouble());
            }
            if (poly.size() < 3) continue;
        }

        LabelMeShape sh;
        sh.label = label;
        sh.polygon = poly;
        out.push_back(sh);
    }

    return out;
}

QImage MainWindow::renderLabelMeOn(const QImage &img, const QString &imagePath, const QString &labelMeDir) const
{
    if (!showLabelMeAnnotations) return img;

    const QVector<LabelMeShape> shapes = loadLabelMeForImage(imagePath, labelMeDir);
    if (shapes.isEmpty()) return img;

    QImage out = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&out);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::yellow);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(QColor(255, 255, 0, 60));

    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(std::max(9, font.pointSize()));
    painter.setFont(font);

    for (const LabelMeShape &sh : shapes) {
        painter.drawPolygon(sh.polygon);

        if (!sh.label.isEmpty() && !sh.polygon.isEmpty()) {
            const QPointF anchor = sh.polygon.first();
            const QRectF bg(anchor.x(), anchor.y() - 18, 220, 18);

            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 140));
            painter.drawRect(bg);

            painter.setPen(Qt::white);
            painter.drawText(bg.adjusted(4, 0, -4, 0), Qt::AlignVCenter | Qt::AlignLeft, sh.label);

            painter.setPen(pen);
            painter.setBrush(QColor(255, 255, 0, 60));
        }
    }

    painter.end();
    return out;
}


void MainWindow::on_loadCocoJsonButton_clicked()
{
    const QString file = QFileDialog::getOpenFileName(this, "Select COCO JSON folder (one JSON per image)", "", "JSON (*.json);;All (*)");
    if (file.isEmpty()) return;

    cocoJsonPath = file;
    if (loadCocoJson(file, cocoCacheSingle)) {
        logActivity("Loaded COCO JSON: " + file);
        if (compareMode) updateCompareView();
        else updateImage();
    }
}

// ------------------------------------------------------------
// Compare mode
// ------------------------------------------------------------
static bool isImageExtAllowed(const QString &ext)
{
    static const QSet<QString> exts = {".jpg",".jpeg",".png",".JPG",".JPEG",".PNG"};
    return exts.contains(ext);
}

static QStringList listImagesInDir(const QString &dirPath)
{
    QDir d(dirPath);
    const QFileInfoList infos = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    QStringList out;
    out.reserve(infos.size());
    for (const QFileInfo &fi : infos) {
        if (isImageExtAllowed(fi.suffix().isEmpty() ? QString() : ("." + fi.suffix()))) {
            out << fi.fileName();
        } else {
            // suffix() returns without dot; handle case-insensitive by checking completeSuffix
            const QString extDot = "." + fi.completeSuffix();
            if (isImageExtAllowed(extDot)) out << fi.fileName();
        }
    }
    return out;
}

void MainWindow::startCompareMode()
{
    const QString leftImg = QFileDialog::getExistingDirectory(this, "Select LEFT image directory");
    if (leftImg.isEmpty()) return;
    const QString leftLbl = QFileDialog::getExistingDirectory(this, "Select LEFT labels directory (optional - Cancel to use image dir)");
    const QString leftCoco = QFileDialog::getOpenFileName(this, "Select LEFT COCO annotations JSON (optional - Cancel to skip)", leftImg, "JSON (*.json);;All (*)");
    const QString leftLabelMe = QFileDialog::getExistingDirectory(this, "Select LEFT LabelMe JSON folder (optional - Cancel to skip)", leftImg);

    const QString rightImg = QFileDialog::getExistingDirectory(this, "Select RIGHT image directory");
    if (rightImg.isEmpty()) return;
    const QString rightLbl = QFileDialog::getExistingDirectory(this, "Select RIGHT labels directory (optional - Cancel to use image dir)");
    const QString rightCoco = QFileDialog::getOpenFileName(this, "Select RIGHT COCO annotations JSON (optional - Cancel to skip)", rightImg, "JSON (*.json);;All (*)");
    const QString rightLabelMe = QFileDialog::getExistingDirectory(this, "Select RIGHT LabelMe JSON folder (optional - Cancel to skip)", rightImg);

    compareLeftImageDir = leftImg;
    compareRightImageDir = rightImg;
    compareLeftLabelsDir = leftLbl.isEmpty() ? leftImg : leftLbl;
    compareRightLabelsDir = rightLbl.isEmpty() ? rightImg : rightLbl;

    compareLeftCocoJsonPath = leftCoco;
    compareRightCocoJsonPath = rightCoco;
    compareLeftLabelMeJsonDir = leftLabelMe;
    compareRightLabelMeJsonDir = rightLabelMe;
    cocoCacheLeft.loaded = false;
    cocoCacheRight.loaded = false;

    const QStringList leftFiles = listImagesInDir(compareLeftImageDir);
    const QStringList rightFiles = listImagesInDir(compareRightImageDir);
    QSet<QString> leftSet;
    leftSet.reserve(leftFiles.size());
    for (const QString &f : leftFiles) leftSet.insert(f);

    compareImageList.clear();
    for (const QString &f : rightFiles) {
        if (leftSet.contains(f)) compareImageList << f;
    }
    std::sort(compareImageList.begin(), compareImageList.end(), [](const QString &a, const QString &b){
        return a.toLower() < b.toLower();
    });

    if (compareImageList.isEmpty()) {
        QMessageBox::information(this, "Compare", "No matching image filenames found between the two directories.");
        return;
    }

    compareMode = true;
    compareIndex = 0;

    // Disable tagging/category selection in compare mode
    categoryTabs->setEnabled(false);
    configureTaggingButton->setEnabled(false);
    addButton->setEnabled(false);
    removeButton->setEnabled(false);
    saveButton->setEnabled(false);
    clearButton->setEnabled(false);
    deleteButton->setEnabled(false);
    moveButton->setEnabled(false);
    copyButton->setEnabled(false);
    toggleYoloButton->setEnabled(true);
    loadNamesButton->setEnabled(true);
    loadLabelsButton->setEnabled(false);
    loadCocoButton->setEnabled(false);

    imageLabel->setVisible(false);
    infoLabel->setVisible(false);
    dateTimeLabel->setVisible(false);
    compareWidget->setVisible(true);

    imageSlider->blockSignals(true);
    imageSlider->setRange(0, compareImageList.size() - 1);
    imageSlider->setValue(0);
    imageSlider->blockSignals(false);

    updateCompareView();
    logActivity("Entered compare mode. LEFT=" + compareLeftImageDir + " RIGHT=" + compareRightImageDir);
}

void MainWindow::exitCompareMode()
{
    if (!compareMode) return;
    compareMode = false;
    compareImageList.clear();

    categoryTabs->setEnabled(true);
    configureTaggingButton->setEnabled(true);
    addButton->setEnabled(true);
    removeButton->setEnabled(true);
    saveButton->setEnabled(true);
    clearButton->setEnabled(true);
    deleteButton->setEnabled(true);
    moveButton->setEnabled(true);
    copyButton->setEnabled(true);

    loadLabelsButton->setEnabled(true);
    loadCocoButton->setEnabled(true);

    imageLabel->setVisible(true);
    infoLabel->setVisible(true);
    dateTimeLabel->setVisible(true);
    compareWidget->setVisible(false);

    imageSlider->blockSignals(true);
    imageSlider->setRange(0, std::max(0, imageList.size() - 1));
    imageSlider->setValue(currentImageIndex);
    imageSlider->blockSignals(false);

    updateImage();
    logActivity("Exited compare mode.");
}

void MainWindow::updateCompareView()
{
    if (!compareMode || compareImageList.isEmpty()) return;
    compareIndex = std::clamp(compareIndex, 0, compareImageList.size() - 1);
    const QString fileName = compareImageList.at(compareIndex);

    const QString leftPath = QDir(compareLeftImageDir).filePath(fileName);
    const QString rightPath = QDir(compareRightImageDir).filePath(fileName);

    QImage leftImg(leftPath);
    QImage rightImg(rightPath);
    if (leftImg.isNull() || rightImg.isNull()) return;

    auto labelStatus = [](const QString &imgPath, const QString &labelsDir) -> QString {
        const QString base = QFileInfo(imgPath).completeBaseName();
        const QString txtPath = QDir(labelsDir).filePath(base + ".txt");
        return QFileInfo::exists(txtPath) ? "labels: FOUND" : "labels: missing";
    };

    if (showYoloBoundingBoxes) {
        leftImg = renderBoundingBoxesOn(leftImg, leftPath, compareLeftLabelsDir);
        rightImg = renderBoundingBoxesOn(rightImg, rightPath, compareRightLabelsDir);
    }

    if (showCocoAnnotations) {
        if (!compareLeftCocoJsonPath.isEmpty()) leftImg = renderCocoForPath(leftImg, leftPath, compareLeftCocoJsonPath, cocoCacheLeft);
        if (!compareRightCocoJsonPath.isEmpty()) rightImg = renderCocoForPath(rightImg, rightPath, compareRightCocoJsonPath, cocoCacheRight);
    }

    if (showLabelMeAnnotations) {
        const QString leftLmDir = compareLeftLabelMeJsonDir.isEmpty() ? compareLeftImageDir : compareLeftLabelMeJsonDir;
        const QString rightLmDir = compareRightLabelMeJsonDir.isEmpty() ? compareRightImageDir : compareRightLabelMeJsonDir;
        leftImg = renderLabelMeOn(leftImg, leftPath, leftLmDir);
        rightImg = renderLabelMeOn(rightImg, rightPath, rightLmDir);
    }

    compareLeftImageLabel->setPixmap(QPixmap::fromImage(leftImg).scaled(compareLeftImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    compareRightImageLabel->setPixmap(QPixmap::fromImage(rightImg).scaled(compareRightImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    compareLeftInfoLabel->setText(QString("LEFT: %1<br>%2 x %3<br>%4<br>%5")
        .arg(QFileInfo(compareLeftImageDir).fileName())
        .arg(leftImg.width())
        .arg(leftImg.height())
        .arg(fileName)
        .arg(labelStatus(leftPath, compareLeftLabelsDir) + QString("<br>COCO: ") + (cocoCacheLeft.loaded ? "LOADED" : "none")));
    compareRightInfoLabel->setText(QString("RIGHT: %1<br>%2 x %3<br>%4<br>%5")
        .arg(QFileInfo(compareRightImageDir).fileName())
        .arg(rightImg.width())
        .arg(rightImg.height())
        .arg(fileName)
        .arg(labelStatus(rightPath, compareRightLabelsDir) + QString("<br>COCO: ") + (cocoCacheRight.loaded ? "LOADED" : "none")));

    indexLabel->setText(QString::number(compareIndex + 1) + " / " + QString::number(compareImageList.size()));
}

// ------------------------------------------------------------
// Tagging helpers
// ------------------------------------------------------------
void MainWindow::ensureDefaultCategory()
{
    const QString def = "Uncategorized";
    if (!categoryPaths.contains(def)) categoryPaths[def] = QStringList();
}

QString MainWindow::currentCategory() const
{
    if (!categoryTabs || categoryTabs->count() == 0) return "Uncategorized";
    return categoryTabs->tabText(categoryTabs->currentIndex());
}

void MainWindow::rebuildCategoryTabs()
{
    categoryTabs->clear();
    categoryWidgets.clear();

    QStringList cats = categoryPaths.keys();
    cats.removeAll("Uncategorized");
    std::sort(cats.begin(), cats.end(), [](const QString &a, const QString &b){
        return a.toLower() < b.toLower();
    });
    cats.prepend("Uncategorized");

    for (const QString &cat : cats) {
        QListWidget *w = new QListWidget(this);
        w->setSelectionMode(QAbstractItemView::ExtendedSelection);
        w->setStyleSheet("QListWidget { color: black; background-color: white; }");

        for (const QString &p : categoryPaths.value(cat)) w->addItem(p);

        categoryWidgets[cat] = w;
        categoryTabs->addTab(w, cat);
    }
}

void MainWindow::updateTaggingHintLabel()
{
    QStringList parts;
    for (auto it = keyToCategory.constBegin(); it != keyToCategory.constEnd(); ++it) {
        QString keyText = QKeySequence(it.key()).toString(QKeySequence::NativeText);
        if (keyText.isEmpty()) continue;
        parts << QString("%1=%2").arg(keyText, it.value());
    }
    parts.sort();
    QString hint = "Tag keys: " + parts.join("   ");
    hint += "   |   A = add to current tab";
    taggingHintLabel->setText(hint);
}

void MainWindow::addCurrentImageToCategory(const QString &category, bool advanceAfter)
{
    if (imageList.isEmpty()) return;

    const QString cat = category.isEmpty() ? "Uncategorized" : category;
    if (!categoryPaths.contains(cat)) categoryPaths[cat] = QStringList();

    const QString imagePath = directory.filePath(imageList.at(currentImageIndex));
    if (!categoryPaths[cat].contains(imagePath)) {
        categoryPaths[cat].append(imagePath);

        if (categoryWidgets.contains(cat)) {
            categoryWidgets[cat]->addItem(imagePath);
        } else {
            rebuildCategoryTabs();
        }

        logActivity(QString("Tagged: %1 -> %2").arg(imagePath, cat));
    }

    if (advanceAfter) showNextImage();
}

void MainWindow::addImageToList()
{
    addCurrentImageToCategory(currentCategory(), false);
    setFocus();
}

void MainWindow::removeImageFromList()
{
    const QString cat = currentCategory();
    if (!categoryWidgets.contains(cat)) return;

    QListWidget *w = categoryWidgets.value(cat);
    const QList<QListWidgetItem*> sel = w->selectedItems();
    if (sel.isEmpty()) return;

    // Collect selected row indices and remove from bottom to top to avoid pointer invalidation.
    QVector<int> rows;
    rows.reserve(sel.size());
    for (QListWidgetItem *it : sel) {
        const int row = w->row(it);
        if (row >= 0) rows.push_back(row);
    }
    std::sort(rows.begin(), rows.end());
    rows.erase(std::unique(rows.begin(), rows.end()), rows.end());

    for (int i = rows.size() - 1; i >= 0; --i) {
        const int row = rows[i];
        QListWidgetItem *item = w->item(row);
        if (!item) continue;

        const QString path = item->text(); // capture before deletion
        categoryPaths[cat].removeAll(path);

        delete w->takeItem(row);

        logActivity(QString("Removed from '%1': %2").arg(cat, path));
    }

    setFocus();
}

void MainWindow::clearImageList()
{
    const QString cat = currentCategory();
    const auto reply = QMessageBox::question(
        this, "Clear Category",
        QString("Clear all items in category '%1'?").arg(cat),
        QMessageBox::Yes | QMessageBox::No
    );
    if (reply != QMessageBox::Yes) return;

    categoryPaths[cat].clear();
    if (categoryWidgets.contains(cat)) categoryWidgets[cat]->clear();
    logActivity("Cleared category: " + cat);
    setFocus();
}

// ------------------------------------------------------------
// Saving per-category lists
// ------------------------------------------------------------
bool MainWindow::ensureSavedListsDir()
{
    if (!savedListsDir.isEmpty()) return true;

    const QString dir = QFileDialog::getExistingDirectory(this, "Choose folder to save category lists", "");
    if (dir.isEmpty()) return false;
    savedListsDir = dir;
    return true;
}

QString MainWindow::categoryListFilePath(const QString &category) const
{
    return QDir(savedListsDir).filePath(QString("%1_list.txt").arg(category));
}

void MainWindow::saveAllCategoryLists(bool silent)
{
    if (!ensureSavedListsDir()) return;

    for (auto it = categoryPaths.constBegin(); it != categoryPaths.constEnd(); ++it) {
        QFile f(categoryListFilePath(it.key()));
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) continue;
        QTextStream out(&f);
        for (const QString &p : it.value()) out << p << "\n";
        f.close();
    }

    const QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    lastSavedLabel->setText("Last saved lists at: " + ts);
    logActivity("Saved category lists to: " + savedListsDir);

    if (!silent) QMessageBox::information(this, "Saved", "All category lists saved.");
}

void MainWindow::saveImageList()
{
    saveAllCategoryLists(false);
    setFocus();
}

// ------------------------------------------------------------
// Tagging config dialog
// ------------------------------------------------------------
static bool isSingleKeyValid(const QString &s)
{
    const QString t = s.trimmed();
    if (t.size() != 1) return false;
    const QChar c = t.at(0);
    return c.isLetterOrNumber();
}

static int keyCodeFromSingleChar(const QString &s)
{
    const QString t = s.trimmed();
    if (t.size() != 1) return 0;
    const QChar c = t.at(0);

    if (c.isDigit()) {
        return int(Qt::Key_0) + (c.unicode() - QChar('0').unicode());
    }
    if (c.isLetter()) {
        const QChar u = c.toUpper();
        return int(Qt::Key_A) + (u.unicode() - QChar('A').unicode());
    }
    return 0;
}

void MainWindow::openTaggingConfig()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Configure Image Tagging (Key -> Category)");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QLabel *desc = new QLabel(
        "Add up to 30 mappings. Key must be a single letter or digit (no multi-key like \"ab\").\n"
        "Pressing the key will tag the current image into that category and advance.",
        &dlg
    );
    layout->addWidget(desc);

    QTableWidget *table = new QTableWidget(&dlg);
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Key", "Category"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    layout->addWidget(table);

    QVector<QPair<int, QString>> existing;
    existing.reserve(keyToCategory.size());
    for (auto it = keyToCategory.constBegin(); it != keyToCategory.constEnd(); ++it)
        existing.push_back({it.key(), it.value()});

    std::sort(existing.begin(), existing.end(), [](const auto &a, const auto &b){ return a.first < b.first; });

    table->setRowCount(std::min(30, int(existing.size())));
    for (int r = 0; r < table->rowCount(); ++r) {
        const QString keyText = QKeySequence(existing[r].first).toString(QKeySequence::NativeText);
        table->setItem(r, 0, new QTableWidgetItem(keyText));
        table->setItem(r, 1, new QTableWidgetItem(existing[r].second));
    }

    QHBoxLayout *rowBtns = new QHBoxLayout();
    QPushButton *addRow = new QPushButton("Add Row", &dlg);
    QPushButton *rmRow = new QPushButton("Remove Row", &dlg);
    rowBtns->addWidget(addRow);
    rowBtns->addWidget(rmRow);
    rowBtns->addStretch();
    layout->addLayout(rowBtns);

    connect(addRow, &QPushButton::clicked, &dlg, [table](){
        if (table->rowCount() >= 30) return;
        table->insertRow(table->rowCount());
    });
    connect(rmRow, &QPushButton::clicked, &dlg, [table](){
        const auto ranges = table->selectedRanges();
        if (ranges.isEmpty()) return;
        for (int i = ranges.size()-1; i >= 0; --i) {
            for (int r = ranges[i].bottomRow(); r >= ranges[i].topRow(); --r)
                table->removeRow(r);
        }
    });

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(bb);
    connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() != QDialog::Accepted) return;

    QMap<int, QString> newMap;
    QStringList newCats;

    for (int r = 0; r < table->rowCount(); ++r) {
        const QString keyText = table->item(r, 0) ? table->item(r, 0)->text().trimmed() : "";
        const QString catText = table->item(r, 1) ? table->item(r, 1)->text().trimmed() : "";

        if (keyText.isEmpty() && catText.isEmpty()) continue;

        if (!isSingleKeyValid(keyText)) {
            QMessageBox::warning(this, "Invalid Key",
                                 QString("Row %1: Key must be a single letter or digit.").arg(r+1));
            return;
        }
        if (catText.isEmpty()) {
            QMessageBox::warning(this, "Invalid Category",
                                 QString("Row %1: Category cannot be empty.").arg(r+1));
            return;
        }

        const int keyCode = keyCodeFromSingleChar(keyText);
        if (keyCode == 0) {
            QMessageBox::warning(this, "Invalid Key",
                                 QString("Row %1: Unable to interpret key.").arg(r+1));
            return;
        }

        if (newMap.contains(keyCode)) {
            QMessageBox::warning(this, "Duplicate Key",
                                 QString("Key '%1' is used more than once.").arg(keyText));
            return;
        }

        newMap[keyCode] = catText;
        if (!newCats.contains(catText)) newCats << catText;
    }

    keyToCategory = newMap;

    for (const QString &c : newCats) {
        if (!categoryPaths.contains(c)) categoryPaths[c] = QStringList();
    }

    rebuildCategoryTabs();
    updateTaggingHintLabel();
    logActivity("Updated tagging configuration.");
    setFocus();
}

// ------------------------------------------------------------
// Bulk actions
// ------------------------------------------------------------
bool MainWindow::chooseDestinationsForCategories(const QStringList &categories,
                                                QMap<QString, QString> &outCategoryToDir,
                                                const QString &title,
                                                const QString &actionVerb) const
{
    outCategoryToDir.clear();

    for (const QString &cat : categories) {
        const QString prompt = QString("%1 destination folder for category '%2'").arg(actionVerb, cat);
        const QString dir = QFileDialog::getExistingDirectory(const_cast<MainWindow*>(this), title + " - " + prompt, "");
        if (dir.isEmpty()) return false;
        outCategoryToDir[cat] = dir;
    }
    return true;
}

bool MainWindow::applyActionToCategory(const QString &category,
                                      const QStringList &paths,
                                      BulkAction action,
                                      const QString &destDir)
{
    if (paths.isEmpty()) return true;

    QDir d(destDir);
    if (!d.exists()) {
        if (!d.mkpath(".")) {
            QMessageBox::warning(this, "Folder", "Failed to create destination folder:\n" + destDir);
            return false;
        }
    }

    for (const QString &src : paths) {
        QFileInfo fi(src);
        if (!fi.exists()) continue;

        const QString dst = d.filePath(fi.fileName());
        const QString srcTxt = fi.dir().filePath(fi.completeBaseName() + ".txt");
        const QString dstTxt = d.filePath(fi.completeBaseName() + ".txt");

        auto doCopy = [&](const QString &a, const QString &b){
            if (QFile::exists(b)) QFile::remove(b);
            return QFile::copy(a, b);
        };
        auto doMove = [&](const QString &a, const QString &b){
            if (QFile::exists(b)) QFile::remove(b);
            return QFile::rename(a, b);
        };

        bool okImg = true;
        bool okAnn = true;

        if (action == BulkAction::Copy) {
            okImg = doCopy(src, dst);
            if (QFile::exists(srcTxt)) okAnn = doCopy(srcTxt, dstTxt);
        } else {
            okImg = doMove(src, dst);
            if (QFile::exists(srcTxt)) okAnn = doMove(srcTxt, dstTxt);
        }

        if (!okImg) {
            QMessageBox::warning(this, "File Operation", "Failed on:\n" + src);
            return false;
        }
        if (!okAnn) logActivity("Warning: failed annotation op for " + src);

        logActivity(QString("%1: %2 -> %3 (cat=%4)")
                        .arg(action == BulkAction::Copy ? "COPY" : "MOVE",
                             src, destDir, category));
    }

    return true;
}

bool MainWindow::runBulkAction(BulkAction action)
{
    saveAllCategoryLists(true);

    QMap<QString, QStringList> selectedByCat;
    for (auto it = categoryWidgets.constBegin(); it != categoryWidgets.constEnd(); ++it) {
        const QString cat = it.key();
        QListWidget *w = it.value();
        QStringList sel;
        for (QListWidgetItem *item : w->selectedItems()) sel << item->text();
        if (!sel.isEmpty()) selectedByCat[cat] = sel;
    }

    const int totalSel = [&](){
        int n=0;
        for (auto it=selectedByCat.begin(); it!=selectedByCat.end(); ++it) n += it.value().size();
        return n;
    }();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        (action == BulkAction::Copy) ? "Copy" : (action == BulkAction::Move) ? "Move" : "Delete",
        QString("Proceed to %1 selected images across categories?\n(Selected: %2)\n\nYou will be asked for a destination folder per category.")
            .arg(action == BulkAction::Copy ? "COPY" : (action == BulkAction::Move ? "MOVE" : "MOVE (Delete)"))
            .arg(totalSel),
        QMessageBox::Yes | QMessageBox::No
    );
    if (reply != QMessageBox::Yes) return false;

    if (selectedByCat.isEmpty()) {
        logActivity("Bulk action requested with no selections.");
        return true;
    }

    const QStringList cats = selectedByCat.keys();
    QMap<QString, QString> catToDir;
    const QString actionVerb = (action == BulkAction::Copy) ? "Choose COPY"
                                : (action == BulkAction::Move) ? "Choose MOVE"
                                : "Choose DELETE (Move)";

    if (!chooseDestinationsForCategories(cats, catToDir, "Select Destination Folders", actionVerb)) {
        logActivity("Bulk action cancelled by user during destination selection.");
        return false;
    }

    for (const QString &cat : cats) {
        if (!applyActionToCategory(cat, selectedByCat.value(cat), action, catToDir.value(cat)))
            return false;
    }

    if (action == BulkAction::Move || action == BulkAction::Delete) {
        for (const QString &cat : cats) {
            const QStringList moved = selectedByCat.value(cat);
            // Qt5/older Qt6 compatibility: QStringList has no removeIf().
            {
                const QStringList current = categoryPaths.value(cat);
                QStringList kept;
                kept.reserve(current.size());
                for (const QString &p : current) {
                    if (!moved.contains(p)) kept << p;
                }
                categoryPaths[cat] = kept;
            }

            if (categoryWidgets.contains(cat)) {
                QListWidget *w = categoryWidgets[cat];
                for (int i = w->count()-1; i >= 0; --i) {
                    if (moved.contains(w->item(i)->text())) delete w->takeItem(i);
                }
            }
        }
        pruneMissingFiles();
        updateImage();
    }

    logActivity("Bulk action completed.");
    QMessageBox::information(this, "Done", "Action completed.");
    return true;
}

void MainWindow::copySelectedImages()  { runBulkAction(BulkAction::Copy); setFocus(); }
void MainWindow::moveSelectedImage()   { runBulkAction(BulkAction::Move); setFocus(); }
void MainWindow::deleteSelectedImage() { runBulkAction(BulkAction::Delete); setFocus(); }

// ------------------------------------------------------------
// Prune missing files
// ------------------------------------------------------------
void MainWindow::pruneMissingFiles()
{
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG" << "*.PNG";
    imageList = directory.entryList(filters, QDir::Files, QDir::Name);
    if (currentImageIndex >= imageList.size()) currentImageIndex = std::max(0, imageList.size()-1);

    imageSlider->setRange(0, std::max(0, imageList.size()-1));
    imageSlider->blockSignals(true);
    imageSlider->setValue(currentImageIndex);
    imageSlider->blockSignals(false);

    for (auto it = categoryPaths.begin(); it != categoryPaths.end(); ++it) {
        QStringList kept;
        kept.reserve(it.value().size());
        for (const QString &p : it.value()) {
            if (QFileInfo::exists(p)) kept << p;
        }
        it.value() = kept;
    }

    updateFolderDateTimeLabel();
}

// ------------------------------------------------------------
// Logging
// ------------------------------------------------------------
void MainWindow::logActivity(const QString &message)
{
    if (!logStream.device()) return;
    logStream << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
              << " - " << message << "\n";
    logStream.flush();
}


// ------------------------------------------------------------
// Open current image folder in file explorer
// ------------------------------------------------------------
void MainWindow::openCurrentImageFolderInExplorer()
{
    const QString folder = directory.absolutePath();
    if (folder.isEmpty()) return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}


// ------------------------------------------------------------
// Status bar: show current directory name
// ------------------------------------------------------------
void MainWindow::updateDirectoryNameLabel()
{
    // Folder name is displayed in the centered info label under the image.
}


// ------------------------------------------------------------
// Folder timestamp label: uses modified time of FIRST image in folder
// ------------------------------------------------------------
void MainWindow::updateFolderDateTimeLabel()
{
    if (!dateTimeLabel) return;

    if (imageList.isEmpty()) {
        dateTimeLabel->setText("");
        return;
    }

    const int idx = std::clamp(currentImageIndex, 0, imageList.size() - 1);
    const QString imgPath = directory.filePath(imageList.at(idx));

    QFileInfo fi(imgPath);
    if (!fi.exists()) {
        dateTimeLabel->setText("");
        return;
    }

    const QDateTime dt = fi.lastModified();
    dateTimeLabel->setText(QString("Modified: %1").arg(dt.toString("yyyy-MM-dd HH:mm:ss")));
}
