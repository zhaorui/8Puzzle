#ifndef ITEMCONTANINER_H
#define ITEMCONTANINER_H

#include <QWidget>
#include "state.h"

class ItemContainer:public QWidget
{
    Q_OBJECT
public:
    ItemContainer(QPixmap& graph,State s,QWidget *parent=0);
    void changeBackground(QPixmap&);
    void move(State::Way);
    QSize sizeHint() const;
    int getStateId() const;
    bool isSolvable()const;
signals:
    void moveDone();
protected:
    void keyPressEvent(QKeyEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void moveToward(State::Way);//pop-up the queue and go for it
private:
    inline void trimPicture();//trim the picture and save it to myPicture
    inline void createPieces();//use myPicture to create Pieces
private slots:
    void updateArea();//for the animation created by key event
    void processNextMove();//when piece moveDone,it will be invoked
private:
    int width;
    int height;
	State state;
    int level;//the game level , could be 2,3,4,5
	int pieceWidth;
    int pieceHeight;
    QPixmap& picture;//reference to the original picture at first time,not always valid.
    QPixmap myPictrue;//picture which has been cut
    QList<QPixmap> pieces;
    QList<QPixmap> arrange;
    enum PaintOption{paintAll,pieceSlide,clearAll};
    PaintOption paintoption;
    QPixmap* currentPiece;
    State::Way towards;
    int newX;
    int newY;
    QRect updateRect;//the area should be update
    QTimer *timer;//used in "updateArea" "KeyPressEvent" "ItemContainer" for animate
    int cursorX;
    int cursorY;
    int blankPos;
    int objPos;
    bool b_ReleaseEvent;
    bool b_MoveEvent;
    QList<State::Way> moveMessageList;
};

#endif // ITEMCONTANINER_H
