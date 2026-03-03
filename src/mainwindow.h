#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QPushButton>
#include <QSlider>
#include <QTabWidget>
#include <QTextStream>
#include <QVector>
#include <QHash>
#include <QPolygonF>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class QKeyEvent;
class QResizeEvent;
class QAction;

struct BoundingBox {
    QRect rect;
    QString label;
    float confidence = 0.0f;
};

// YOLO parsed annotation (pixel-space)
struct Annotation {
    QRect boundingBox;
    int classId = -1;
    QString className;
    float confidence = 0.0f; // optional if present in label file
};

// COCO parsed annotation (pixel-space)
struct CocoAnnotation {
    QRectF bbox;                 // [x,y,w,h] in pixels
    int categoryId = -1;
    QString categoryName;
    QVector<QPolygonF> polygons; // segmentation polygons (if available)
    QImage rleMask;              // alpha mask for RLE segmentation (optional)
};

// Parsed COCO dataset cache (per JSON)
struct CocoCache {
    bool loaded = false;
    QString jsonPath;
    QHash<QString, int> fileNameToImageId;             // file_name -> image_id
    QHash<int, QVector<CocoAnnotation>> imageIdToAnns; // image_id -> annotations
    QHash<int, QString> categoryIdToName;              // category_id -> name
};

// LabelMe parsed shape (pixel-space)
struct LabelMeShape {
    QString label;
    QPolygonF polygon;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Navigation
    void showPreviousImage();
    void showPreviousDirectory();
    void showNextImage();
    void showNextDirectory();

    // Tagging / lists
    void openTaggingConfig();
    void addImageToList();          // add current image to current category tab
    void removeImageFromList();     // remove selected from current category tab
    void saveImageList();           // save all category lists (first time chooses base dir)
    void clearImageList();          // clear current category tab

    // Actions (operate on selected items across categories)
    void deleteSelectedImage();     // moves to user-selected per-category folders
    void moveSelectedImage();
    void copySelectedImages();

    // YOLO
    void toggleYoloBoundingBoxes();
    void on_loadNamesFileButton_clicked();
    void on_loadLabelsDirButton_clicked();

    // COCO
    void toggleCocoAnnotations();
    void on_loadCocoJsonButton_clicked();

    // Compare
    void startCompareMode();
    void exitCompareMode();

    // File menu
    void openImageDirectory();
    void openCurrentImageFolderInExplorer();

private:
    // UI helpers
    void styleButton(QPushButton *button);
    void setMainWindowStyle();
    void updateToggleYoloButtonStyle();

    // Image handling
    void loadImagesFromDirectory();
    bool loadImagesFromDirectoryPath(const QString &dirPath, bool logIt = true);
    void updateImage();
    void updateCompareView();
    void logActivity(const QString &message);

    // YOLO helpers
    QString getClassName(int classId) const;
    void loadClassNames(const QString &namesFilePath);
    QImage renderBoundingBoxesOn(const QImage &img,
                                 const QString &imagePath,
                                 const QString &labelsDir) const;

    QString effectiveLabelsDirForSingleView() const;


    // COCO helpers
    QString imageFileNameKey(const QString &imagePath) const;
    bool loadCocoJson(const QString &jsonPath, CocoCache &cache);
    QImage renderCocoOn(const QImage &img,
                        const QString &imagePath,
                        const CocoCache &cache) const;

    // LabelMe (per-image JSON) annotations (polygon/rectangle)
    void toggleLabelMeAnnotations();
    void on_loadLabelMeDirButton_clicked();
    QVector<LabelMeShape> loadLabelMeForImage(const QString &imagePath, const QString &labelMeDir) const;
    QImage renderLabelMeOn(const QImage &img, const QString &imagePath, const QString &labelMeDir) const;

    // Per-image COCO JSON mode: one JSON per image
    bool loadCocoPerImageJson(const QString &jsonPath,
                              QVector<CocoAnnotation> &outAnns,
                              QHash<int, QString> &outCatMap) const;

    // Render COCO annotations from either a per-image JSON folder OR a single COCO instances JSON file.
    QImage renderCocoForPath(const QImage &img,
                             const QString &imagePath,
                             const QString &cocoPathOrDir,
                             CocoCache &cache) const;

    // Tagging helpers
    void ensureDefaultCategory();
    void rebuildCategoryTabs();
    QString currentCategory() const;
    void updateTaggingHintLabel();
    void updateDirectoryNameLabel();
    void updateFolderDateTimeLabel();
    void addCurrentImageToCategory(const QString &category, bool advanceAfter);

    // Saving lists
    bool ensureSavedListsDir();
    void saveAllCategoryLists(bool silent);
    QString categoryListFilePath(const QString &category) const;

    // Bulk actions
    enum class BulkAction { Copy, Move, Delete };
    bool runBulkAction(BulkAction action);
    bool chooseDestinationsForCategories(const QStringList &categories,
                                        QMap<QString, QString> &outCategoryToDir,
                                        const QString &title,
                                        const QString &actionVerb) const;

    bool applyActionToCategory(const QString &category,
                               const QStringList &paths,
                               BulkAction action,
                               const QString &destDir);

    void pruneMissingFiles();
    QStringList siblingDirectories(QString *outCurrentName = nullptr) const;

private:
    // Data
    QDir directory;
    QStringList imageList;
    int currentImageIndex = 0;

    // YOLO
    bool showYoloBoundingBoxes = false;
    QImage currentImage;
    QString yoloLabelsDirPath; // optional separate labels dir for single-view
    QStringList classNames;
    QMap<int, QColor> classColors;

    // COCO
    bool showCocoAnnotations = false;
    QString cocoJsonPath; // single-view COCO path: folder of per-image JSONs OR a single COCO instances JSON
    CocoCache cocoCacheSingle;

    // LabelMe
    bool showLabelMeAnnotations = false;
    QString labelMeJsonDirPath; // single-view: folder containing per-image LabelMe JSONs

    // Compare mode
    bool compareMode = false;
    QString compareLeftImageDir;
    QString compareRightImageDir;
    QString compareLeftLabelsDir;
    QString compareRightLabelsDir;
    QString compareLeftCocoJsonPath; // folder or file
    QString compareRightCocoJsonPath; // folder or file
    QString compareLeftLabelMeJsonDir;  // folder of per-image LabelMe JSONs (optional)
    QString compareRightLabelMeJsonDir; // folder of per-image LabelMe JSONs (optional)
    CocoCache cocoCacheLeft;
    CocoCache cocoCacheRight;
    QStringList compareImageList; // filenames that exist in both dirs
    int compareIndex = 0;

    // Tagging
    QMap<int, QString> keyToCategory;            // Qt::Key_* -> category
    QMap<QString, QStringList> categoryPaths;    // category -> full file paths
    QMap<QString, QListWidget*> categoryWidgets; // category -> list widget
    QString savedListsDir;                       // base dir for list txts

    // UI elements
    QLabel *titleLabel = nullptr;
    QLabel *imageLabel = nullptr;
    QLabel *infoLabel = nullptr;
    QLabel *taggingHintLabel = nullptr;
    QLabel *lastSavedLabel = nullptr;
    QLabel *indexLabel = nullptr;

    // Compare UI
    QWidget *compareWidget = nullptr;
    QLabel *compareLeftImageLabel = nullptr;
    QLabel *compareRightImageLabel = nullptr;
    QLabel *compareLeftInfoLabel = nullptr;
    QLabel *compareRightInfoLabel = nullptr;
    QAction *compareAction = nullptr;
    QAction *exitCompareAction = nullptr;
    QAction *loadLabelsDirAction = nullptr;
    QLabel *dirNameLabel = nullptr;
    QLabel *dateTimeLabel = nullptr;

    QPushButton *leftButton = nullptr;
    QPushButton *rightButton = nullptr;

    QSlider *imageSlider = nullptr;

    QPushButton *configureTaggingButton = nullptr;

    QTabWidget *categoryTabs = nullptr;

    // Group buttons
    QPushButton *addButton = nullptr;
    QPushButton *removeButton = nullptr;
    QPushButton *saveButton = nullptr;
    QPushButton *clearButton = nullptr;

    QPushButton *deleteButton = nullptr;
    QPushButton *moveButton = nullptr;
    QPushButton *copyButton = nullptr;

    QPushButton *toggleYoloButton = nullptr;
    QPushButton *loadNamesButton = nullptr;
    QPushButton *loadLabelsButton = nullptr;
    QPushButton *toggleCocoButton = nullptr;
    QPushButton *loadCocoButton = nullptr;
    QPushButton *toggleLabelMeButton = nullptr;
    QPushButton *loadLabelMeButton = nullptr;

    // Logging
    QFile logFile;
    QTextStream logStream;
};

#endif // MAINWINDOW_H