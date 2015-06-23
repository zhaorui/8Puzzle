#include"itemcontaniner.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
ItemContainer::ItemContainer(QPixmap &graph,State s,QWidget *parent)
    :width(300),height(300),state(s),level(state.N),
    pieceWidth(width/level),pieceHeight(height/level),
    picture(graph),paintoption(paintAll),QWidget(parent)
{
    resize(width,height);
    trimPicture();//trim the picture and save it to myPicture
    createPieces();//setup pieces
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateArea()));
    connect(this,SIGNAL(moveDone()),this,SLOT(processNextMove()));//actually It could be done with invoking.
    setFocusPolicy(Qt::StrongFocus);
}

void ItemContainer::changeBackground(QPixmap &newPict)
{
    picture = newPict;
    trimPicture();
    createPieces();
    update();
}

//This fuction process the message from list
void ItemContainer::moveToward(State::Way to)
{
    UINT8 opt = state.wayToGo();//the able way to move from the current state
    blankPos = state.id%10-1;//get the blank piece pos;
    int blankX = blankPos%level*pieceWidth;//get blank piece's x position
    int blankY = blankPos/level*pieceHeight;
    objPos = 0;
    switch(to){
    case State::L:
		if((opt&State::L)==0){
			moveMessageList.pop_front();
			return;
		}
        paintoption = pieceSlide;
        state.moveLeft();
        objPos = blankPos+1;
        currentPiece = &arrange[objPos];
        towards = State::L;
        newX = blankX+pieceWidth;
        newY = blankY;
        updateRect = QRect(blankX,blankY,pieceWidth*2,pieceHeight);
        timer->start(10);
        arrange.swap(blankPos,objPos);
        //paintoption = paintAll;
        break;
    case State::R:
		if((opt&State::R)==0){
			moveMessageList.pop_front();
			return;
		}
        paintoption = pieceSlide;
        state.moveRight();
        objPos = blankPos-1;
        currentPiece = &arrange[objPos];
        towards = State::R;
        newX = blankX-pieceWidth;
        newY = blankY;
        updateRect = QRect(newX,newY,pieceWidth*2,pieceHeight);
        timer->start(10);
        arrange.swap(blankPos,objPos);
        //paintoption = paintAll;
        break;
    case State::U:
		if((opt&State::U)==0){
			moveMessageList.pop_front();
			return;
		}
        paintoption = pieceSlide;
        state.moveUp();
        objPos = blankPos+level;
        currentPiece = &arrange[objPos];
        towards = State::U;
        newX = blankX;
        newY = blankY+pieceHeight;
        updateRect = QRect(blankX,blankY,pieceWidth,pieceHeight*2);
        timer->start(10);
        arrange.swap(blankPos,objPos);
        //paintoption = paintAll;
        break;
    case State::D:
		if((opt&State::D)==0){
			moveMessageList.pop_front();
			return;
		}
        paintoption = pieceSlide;
        state.moveDown();
        objPos = blankPos-level;
        currentPiece = &arrange[objPos];
        towards = State::D;
        newX = blankX;
        newY = blankY-pieceHeight;
        updateRect = QRect(newX,newY,pieceWidth,pieceHeight*2);
        timer->start(10);
        arrange.swap(blankPos,objPos);
        //paintoption = paintAll;
        break;
	case State::Z:
		//when we receive this state, it means the puzzle has accomplish
		emit moveDone();
		break;
    }

}

void ItemContainer::move(State::Way w)
{
	if(moveMessageList.empty()){
		moveMessageList.push_back(w);
		moveToward(w);
	}else
		moveMessageList.push_back(w);
}

void ItemContainer::keyPressEvent(QKeyEvent *event)
{ 
    switch(event->key()){
    case Qt::Key_Left:
        move(State::L);
        break;
    case Qt::Key_Right:
        move(State::R);
        break;
    case Qt::Key_Up:
        move(State::U);
        break;
    case Qt::Key_Down:
        move(State::D);
        break;
    case Qt::Key_Space:
        paintoption = clearAll;
        update(0,0,width/level,height/level);
		break;
    }

}

void ItemContainer::mousePressEvent(QMouseEvent *event)
{
    b_ReleaseEvent = true;
    b_MoveEvent = true;
    if(event->button() == Qt::LeftButton){
        cursorX = event->x();
        cursorY = event->y();
        newX = cursorX/pieceWidth*pieceWidth;
        newY = cursorY/pieceHeight*pieceHeight;
        objPos = cursorY/pieceHeight*level+cursorX/pieceWidth;//get the piece;
        currentPiece = &arrange[objPos];

        blankPos = state.id%10-1;//get the blank piece pos;
        int blankX = blankPos%level*pieceWidth;//get blankpiece's x position
        int blankY = blankPos/level*pieceHeight;

        //calculate manhattan distance
        int dist=abs(newX-blankX)/pieceWidth+abs(newY-blankY)/pieceHeight;
        if(dist>1||dist==0){
            b_ReleaseEvent = false;
            b_MoveEvent = false;
            return;
        }

        if(newX>blankX){
            towards = State::L;
            updateRect = QRect(blankX,blankY,2*pieceWidth,pieceHeight);
        }
        else if(newX<blankX){
            towards = State::R;
            updateRect = QRect(newX,newY,2*pieceWidth,pieceHeight);
        }
        else if(newY<blankY){
            towards = State::D;
            updateRect = QRect(newX,newY,pieceWidth,2*pieceHeight);
        }
        else{
            towards = State::U;
            updateRect = QRect(blankX,blankY,pieceWidth,2*pieceHeight);
        }
    }
}

void ItemContainer::mouseMoveEvent(QMouseEvent *event)
{
    if(!b_MoveEvent)
        return;
    int currentX = event->x();
    int currentY = event->y();
    int dx = currentX - cursorX;
    int dy = currentY - cursorY;

    if(towards==State::L||towards==State::R){
        newX+=dx;
        if(newX<=updateRect.x())
            newX=updateRect.x();
        if(newX>=updateRect.x()+pieceWidth)
            newX = updateRect.x()+pieceWidth;
    }

    if(towards==State::U||towards==State::D){
        newY+=dy;
        if(newY<=updateRect.y())
            newY = updateRect.y();
        if(newY>=updateRect.y()+pieceHeight)
            newY = updateRect.y()+pieceHeight;
    }
    cursorX = currentX;
    cursorY = currentY;
    paintoption = pieceSlide;
    update(updateRect);
}

void ItemContainer::mouseReleaseEvent(QMouseEvent *event)
{
    if(!b_ReleaseEvent)
        return;
    paintoption = paintAll;

    if(towards==State::L&&newX<updateRect.x()+pieceWidth/4*3){
        state.moveLeft();
        arrange.swap(blankPos,objPos);
    }else if(towards==State::R&&newX>updateRect.x()+pieceWidth/4){
        state.moveRight();
        arrange.swap(blankPos,objPos);
    }else if(towards==State::U&&newY<updateRect.y()+pieceWidth/4*3){
        state.moveUp();
        arrange.swap(blankPos,objPos);
    }else if(towards==State::D&&newY>updateRect.y()+pieceWidth/4){
        state.moveDown();
        arrange.swap(blankPos,objPos);
    }
    update();
}

void ItemContainer::paintEvent(QPaintEvent *evnet)
{
    QPainter painter(this);
    int count = 0;
    switch(paintoption)
    {
    case paintAll:
        for(int y=0;y<level;y++){
            for(int x=0;x<level;x++){
                painter.drawPixmap(x*pieceWidth,y*pieceHeight,
                                   pieceWidth,pieceHeight,arrange[count++]);
            }
        }break;
    case pieceSlide:
        painter.drawPixmap(newX,newY,pieceWidth,pieceHeight,*currentPiece);
        break;
    case clearAll:
        break;
    }
}

void ItemContainer::resizeEvent(QResizeEvent *event)
{
    width = event->size().width();
    height = event->size().height();
    pieceWidth = width/level;
    pieceHeight = height/level;
    trimPicture();
    createPieces();
    paintoption = paintAll;
}

inline void ItemContainer::trimPicture()
{
    //[1] trim the picture and save it to "myPicture"
    int size = qMin(picture.width(), picture.height());
    myPictrue = picture.copy((picture.width() - size)/2,
             (picture.height() - size)/2, size, size).scaled(width,
                 height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //[1]!
}

/*
Detail: Every time invoke this function would clear all previous graph record.
        include pieces,arrange list.but the "state" still
*/
inline void ItemContainer::createPieces()
{
    if(!pieces.isEmpty())
        pieces.clear();
	if(!arrange.isEmpty())
		arrange.clear();

    //[1]setup pieces
    for(int y=0;y<=height-pieceHeight-(height%level);y+=pieceHeight){
        for(int x=0;x<=width-pieceWidth-(width%level);x+=pieceWidth){
            QPixmap pixmap = myPictrue.copy(x,y,pieceWidth,pieceHeight);
            pieces.push_back(pixmap);
        }
    }
    pieces[level*level-1] = QPixmap();//the last piece is blank piece
    //![1]

    //[2]arrage pieces
    int last = state.vect.size()-1;//indicate the pos of blank piece
    for(int i=0;i<last;i++)
        arrange.push_back(pieces[state.vect[i]-1]);
    arrange.insert(state.vect[last]-1,pieces[last]);//insert blank piece to arrange list
    //![2]

}

void ItemContainer::updateArea()
{
	const int totalnum = 20;
    static int count = totalnum;
	
    int dx = pieceWidth/totalnum;
    int dy = pieceHeight/totalnum;
    if(count == 0)
        return;
    switch(towards){
	case State::L:
        newX-=dx;
        repaint(updateRect);
        break;
    case State::R:
        newX+=dx;
        repaint(updateRect);
        break;
    case State::U:
        newY-=dy;
        repaint(updateRect);
        break;
    case State::D:
        newY+=dy;
        repaint(updateRect);
        break;
    }
    //repaint(updateRect);
    count--;
    if(count==0){
        count = totalnum;
        timer->stop();
		paintoption = paintAll;
		repaint();
        emit moveDone();
    }
}

QSize ItemContainer::sizeHint() const
{
    return QSize(300,300);
}

int ItemContainer::getStateId()const
{
    return state.id;
}

void ItemContainer::processNextMove()
{
	moveMessageList.pop_front();
    if(moveMessageList.empty())
        return;
    else{
        moveToward(moveMessageList.first());
    }
}

bool ItemContainer::isSolvable()const
{
    return state.isReachable();
}
