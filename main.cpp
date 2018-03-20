#include <SFML/Graphics.hpp>
#include <cstring>
using namespace sf;

int ts = 54;
Vector2i offset(48,24);


struct piece {
    int x,y,col,row,kind,match,alpha;
    piece() {
        match=0; alpha=255;
    }
} grid[10][10];

void swap(piece p1,piece p2) {
    std::swap(p1.col,p2.col);
    std::swap(p1.row,p2.row);
    grid[p1.row][p1.col]=p1;
    grid[p2.row][p2.col]=p2;
}

void findMatch(int userMove, int &hor, int &ver) {
    for(int i=1;i<=8;i++) {
        for(int j=1;j<=8;j++) {
            if (grid[i][j].kind==grid[i+1][j].kind)
                if (grid[i][j].kind==grid[i-1][j].kind) {
                    if(userMove)
                        hor=1;
                    for(int n=-1;n<=1;n++)
                        grid[i+n][j].match++;
                }
            
            if (grid[i][j].kind==grid[i][j+1].kind)
                if (grid[i][j].kind==grid[i][j-1].kind) {
                    if(userMove)
                        ver=1;
                    for(int n=-1;n<=1;n++)
                        grid[i][j+n].match++;
                }
        }
    }
}

int main() {
    srand(time(0));

    RenderWindow app(VideoMode(740,480), "Match-3 Game!");
    app.setFramerateLimit(60);


    Text scoreText, scoreVal, moveText, moveVal;
    
    int points = 0, moves = 30;
    Font font;
    if(!font.loadFromFile("/Users/gsehwal/Desktop/Tasks/EA/bejewelled/bejewelled/sansation.ttf")) {
        // error...
    }
    
    //Setting values for texts
    scoreText.setFont(font);
    scoreText.setString("Score : ");
    scoreText.setCharacterSize(30);
    scoreText.setStyle(Text::Bold);
    scoreText.move(550, 20);
    scoreVal.setFont(font);
    scoreVal.setString(std::to_string(points));
    scoreVal.setCharacterSize(30);
    scoreVal.setStyle(Text::Bold);
    scoreVal.setFillColor(Color::Green);
    scoreVal.move(650,20);
    moveText.setFont(font);
    moveText.setString("Moves : ");
    moveText.setCharacterSize(30);
    moveText.setStyle(Text::Bold);
    moveText.move(550, 70);
    moveVal.setFont(font);
    moveVal.setString(std::to_string(moves));
    moveVal.setCharacterSize(30);
    moveVal.setStyle(Text::Bold);
    moveVal.setFillColor(Color::Green);
    moveVal.move(665,70);
    

    Texture t1,t2;
    t1.loadFromFile("/Users/gsehwal/Desktop/Tasks/EA/bejewelled/bejewelled/images/background.png");
    t2.loadFromFile("/Users/gsehwal/Desktop/Tasks/EA/bejewelled/bejewelled/images/gems.png");

    Sprite background(t1), gems(t2);

    //Initialization of grid
    for (int i=1;i<=8;i++)
        for (int j=1;j<=8;j++) {
            grid[i][j].kind=rand()%5;
            grid[i][j].col=j;
            grid[i][j].row=i;
            grid[i][j].x = j*ts;
            grid[i][j].y = i*ts;
        }

    int x0,y0,x,y; int click=0; Vector2i pos;
    bool isSwap=false, isMoving=false;

    while (app.isOpen()) {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();

            if (e.type == Event::MouseButtonPressed)
                if (e.key.code == Mouse::Left) {
                    if (!isSwap && !isMoving)
                        click++;
                    pos = Mouse::getPosition(app)-offset;
                }
        }
        
        int userMove=0; //To check if the move is made by user or not
        if (click==1) {
            x0=pos.x/ts+1;
            y0=pos.y/ts+1;
        }
        if (click==2) {
            x=pos.x/ts+1;
            y=pos.y/ts+1;
            if (abs(x-x0)+abs(y-y0)==1) {
                userMove=1; //Update it if user makes the move
                swap(grid[y0][x0],grid[y][x]);
                isSwap=1;
                click=0;
            }
            else
                click=1;
        }
        
        int hor=0, ver=0;
        if(userMove) {
            findMatch(1,hor,ver);
        }
        else {
            findMatch(0,hor,ver);
        }
        
        if(ver==1 && hor==1) { //Horizontal AND Vertical match gives 20 points
            points+=20;
            moves--;
            scoreVal.setString(std::to_string(points));
            moveVal.setString(std::to_string(moves));
            app.draw(scoreVal);
            app.draw(moveVal);
            
        }
        else if(ver==1 || hor==1) { //Horizontal OR Vertical match gives 10 points
            points+=10;
            moves--;
            scoreVal.setString(std::to_string(points));
            moveVal.setString(std::to_string(moves));
            app.draw(scoreVal);
            app.draw(moveVal);
        }

        //Animating the pieces
        isMoving=false;
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++) {
                piece &p = grid[i][j];
                int dx,dy;
                for(int n=0;n<4;n++) {
                    dx = p.x-p.col*ts;
                    dy = p.y-p.row*ts;
                    if (dx) p.x-=dx/abs(dx);
                    if (dy) p.y-=dy/abs(dy);
                }
                if (dx||dy)
                    isMoving=1;
            }

        //Deleting animation
        if (!isMoving)
            for (int i=1;i<=8;i++)
                for (int j=1;j<=8;j++)
                    if (grid[i][j].match) if (grid[i][j].alpha>10) {
                        grid[i][j].alpha-=10;
                        isMoving=true;
                    }

        int score=0; //To check if there is any match or not
        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++)
                score+=grid[i][j].match;

        if(isSwap && !isMoving) {
            if(!score) { //If no match swap again
                moves--;
                moveVal.setString(std::to_string(moves));
                app.draw(moveVal);
                swap(grid[y0][x0],grid[y][x]);
            }
                isSwap=0;
        }

        //Update the grid
        if(!isMoving) {
            for(int i=8;i>0;i--)
                for(int j=1;j<=8;j++)
                    if (grid[i][j].match)
                        for(int n=i;n>0;n--)
                            if (!grid[n][j].match) {
                                swap(grid[n][j],grid[i][j]);
                                break;
                            }

            for(int j=1;j<=8;j++)
                for(int i=8,n=0;i>0;i--)
                    if (grid[i][j].match) {
                        grid[i][j].kind = rand()%5;
                        grid[i][j].y = -ts*n++;
                        grid[i][j].match=0;
                        grid[i][j].alpha = 255;
                    }
        }

        
        //Drawings
        app.draw(background);
        app.draw(scoreText);
        app.draw(moveText);
        app.draw(scoreVal);
        app.draw(moveVal);

        for (int i=1;i<=8;i++)
            for (int j=1;j<=8;j++) {
                piece p = grid[i][j];
                gems.setTextureRect( IntRect(p.kind*49,0,49,49));
                gems.setColor(Color(255,255,255,p.alpha));
                gems.setPosition(p.x,p.y);
                gems.move(offset.x-ts,offset.y-ts);
                app.draw(gems);
            }
        app.display();
    }
    return 0;
}
