# IRIS_ImageSelector
A QT application that allows for data annotators to manage data for filtering.
General flow of application:
1. User chooses an image directory on startup
2. Use left and right arrows or the arrow buttons on GUI to navigate back and forth between images
3. Click on "Add" or Press Key "A" to select an image. The image path is immediately added to the right-side panel.
4. If an image path is added by accident or user changes mind, user can remove the path by selecting the path in the list and clicking remove and confirming.
5. Save the list of filepaths that are selected and create a filename based on its purpose. For example, "move_to_train.txt," or "move_to_val.txt"
6. List must be saved before user performs an action.
7. Actions available: Delete, Move or Copy.
8. Delete Action uses the selected image paths and moves all the images from current directory to a folder called "deleted_images" in the current image directory's parent directory
9. Move Action uses the same principle as Delete but user must choose the folder. For example, if user is selecting images to move to train from a raw image folder, user can choose the folder "Train." Once user is done moving the images, user should click on "Clear List" to clear the list. User will be prompted to create a txt file to save image paths again. User can choose the pre-existing file (then the future content replaces the previous content) or create a new file if the purpose has changed.
10. Copy Action uses the same principle as above but to copy imaegs to a folder of user's choice.
11. Toggle YOLO Bounding Boxes and Choose Names File buttons: Find a names file in .names extension and click on Toggle YOLO Bounding Boxes button once to enable viewing the annotations. Click the button again to stop viewing the annotations.

Instructions:
```
git clone https://github.com/nbandaru1h/IRIS_ImageSelector.git

mkdir build && cd build

cmake ..

make   # Linux

cmake --build .   # Windows

./IRIS_ImageSelector   # Linux

git checkout master # Linux

IRIS_ImageSelector.exe  # Windows
```
------------------------------------------------------------------------------------------------------------------------------------------------------
Upcoming Updates:
1. A toggle button like Yolo_mark to slide through the images. User will be able to find the number of total images in currently loaded image directory
2. Ability to view (only) yolo bounding boxes in the image. User can choose to not view bounding boxes as well.
------------------------------------------------------------------------------------------------------------------------------------------------------

AUTHOR: NIRALI BANDARU
