//THE WIZZ BALL :
//included libraries:
#include <raylib.h>
#include <stdlib.h>
//required constant
#define MAX_PLATFORMS 10

//STRUCTS:
//the struct ball stores the ball x and y positions, it size and its condition whether it is ascending or not
typedef struct{
    int x;
    int y;
    float size;
    int velocity;
    bool isAscending;
}Ball;
//the struct platform stores rectangle array as platforms, their colour and whether they are on screen or not
typedef struct{
    Rectangle rect;
    Color color;
    bool active;
}Platform;


//FUNCTIONS:
//this function ball on platform intakes the struct ball , window dimensions and the platform array and senses whether the ball 
//touches any platform, if yes it lands the ball on that specific platform
int ballOnPlatform(Ball *ball, int windowHeight, Platform platforms[]){
    if (ball->y>=windowHeight-ball->size){
        ball->velocity=0;
        ball->y=windowHeight-ball->size;
        ball->isAscending=false;
        return 1;
    }
    for (int i = 0; i < MAX_PLATFORMS; i++){
        if (platforms[i].active) {
            if (CheckCollisionCircleRec((Vector2){ball->x, ball->y + ball->size}, ball->size, platforms[i].rect)){
                if (ball->velocity>=0 && ball->y+ball->size<=platforms[i].rect.y){
                    ball->velocity=0;
                    ball->y=platforms[i].rect.y-ball->size;
                    ball->isAscending=false;
                        if (IsKeyPressed(KEY_UP)){
                            ball->velocity=-30;
                            ball->isAscending=true;
                        }
                    return 1;
                }
            }
        }
    }
    return 0;
}

//this functiom senses whether the ball is at the bottom of the window or not, if the ball is at the bottom it returns 
//the position of that ball
int ballAtBottom(Ball *ball, int windowHeight){
    return ball->y+ball->size>=windowHeight;
}

//this function senses the x position os the ball, if the ball touches the right or left corner of the window, it makes
// the ball to stopo at the corner so that the ball cannot go out of the window 
int ballAtEdge(Ball *ball, int windowWidth){
    if (ball->x-ball->size<=0){
        return 1;
    } 
    else if(ball->x + ball->size >= windowWidth){
        return 2;
    }
    return 0;
}

//this function checks the collisiion between two active platform so that they both cannot over lap each other, if so
//it returns 1
int checkCollisionBetweenPlatforms(const Platform platforms[], int index, const Rectangle *newPlatform){
    for(int i=0; i<MAX_PLATFORMS; i++){
        if(i!=index && platforms[i].active){
            if(CheckCollisionRecs(platforms[i].rect, *newPlatform)){
                return 1;
            }
        }
    }
    return 0;
}

// this function actually uses the rectangle struct to generate the platforms after a regular interval and also after 
// any platform becomes unactive as it goes down the window
void updatePlatforms(Platform platforms[],int screenWidth,int screenHeight,float speed,Ball *ball,bool *ballPlaced,bool gameOver){
    static int platformCooldown=0;
    if (platformCooldown>0){
        platformCooldown--;
    }
    // to make the platforms active use a for loop
    for(int i=0;i<MAX_PLATFORMS;i++){
        if(platforms[i].active && !gameOver){
            platforms[i].rect.y+=speed;
            if(platforms[i].rect.y>screenHeight){
                platforms[i].active=false;
                platformCooldown=0;
            }
        } 
        else if(platformCooldown==0 && !gameOver){
            Rectangle newPlatformRect={
                GetRandomValue(0, screenWidth - 150),
                0,
                GetRandomValue(100, 150),
                10
            };
            // do not generate a platform when it tries to collide with an existing platform
            if(!checkCollisionBetweenPlatforms(platforms, i, &newPlatformRect)){
                platforms[i].rect=newPlatformRect;
                platforms[i].color=WHITE;
                platforms[i].active=true;
                platformCooldown=75;
                if (!(*ballPlaced)){
                    ball->x=(int)(newPlatformRect.x+newPlatformRect.width/2);
                    ball->y=(int)(newPlatformRect.y-ball->size);
                    ball->size=20.0;
                    ball->velocity=1;
                    ball->isAscending=false;
                    (*ballPlaced)=true;
                }
            }
        }
    }
}


//THE MAIN FUNCTION:
int main(){
    //window FPS maintains the speed of running of raylib loop 
    SetTargetFPS(60);
    //all the variables to be used
    int windowWidth=800;
    int windowHeight=600;
    int gravity=2;
    float platformSpeed=2.0;
    Ball ball;
    ball.x=windowWidth/2;
    ball.y=windowHeight/2;
    ball.size=20.0;
    ball.velocity=1;
    ball.isAscending=false;
    int timer=0;
    int score=0;
    bool gameover=false;
    bool ballPlaced=false;
    bool paused=false;
    int playsound;
    //sound is a data type which holds audio variable
    Sound gamesound;
    //rectangle struct to initialize all the buttons used in the game
    Rectangle playButton={windowWidth/2-70,180,140,30};
    Rectangle selectLevelButton={windowWidth/2- 70,220,140,30};  
    Rectangle easyLevelButton={windowWidth/2-70,260,140,30};  
    Rectangle mediumLevelButton={windowWidth/2-70,300,140,30};  
    Rectangle hardLevelButton = {windowWidth/2-70,340,140,30};  
    Rectangle pauseButton = {windowWidth-50,10,30,30};
    Rectangle quitbutton={windowWidth/2-70,300,140,30};
    Rectangle soundbutton={windowWidth/2-70,260,140,30};
    Rectangle soundonbutton={windowWidth/2-70,300,140,30};
    Rectangle soundoffbutton={windowWidth/2-70,340,140,30};
    
    
    
    
    //here the initwindow function initializes the window, a GUI for game
    InitWindow(windowWidth,windowHeight, "                          WIZZ BALL");
    
    //this function allows the access of speaker to the program to run the music
    InitAudioDevice();
    
    //this function takes the path of a picture to be set as the background of the game
    Texture2D background=LoadTexture("D:\\acb.png");
    
    //this function takes the path of a music file to be played
    gamesound=LoadSound("D:\\123.wav");
    
    //primarily uninitializing all the platforms by a for loop
    Platform platforms[MAX_PLATFORMS];
    for(int i=0;i<MAX_PLATFORMS;i++) {
        platforms[i].active=false;
    }
    
    //an enum which defines the state of game at a particular situation
    enum GameState{START,PLAYING,LEVEL_SELECTION,SOUND};
    
    //initial condition to be start which is at the main menu page
    int gameState=START;
    
    
    
    
    
    while(WindowShouldClose()==false){
        Vector2 mousePos=GetMousePosition();
        
        //the buttons present at the start condition are checked conditionally as being pressed or not
        if(gameState==START){
            // Check if the Play button is pressed
            if(CheckCollisionPointRec(mousePos,playButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                gameState=PLAYING;
            }

            // Check if the Select Level button is pressed
            if(CheckCollisionPointRec(mousePos,selectLevelButton) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                gameState=LEVEL_SELECTION;
            }

            //check if the quit button is pressed
            if(CheckCollisionPointRec(mousePos,quitbutton)&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                break;
            }

            //check if the sound button is pressed
            if(CheckCollisionPointRec(mousePos,soundbutton)&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                gameState=SOUND;
            }
        }
        //condition to be implemented in the playing state of the game
        if(gameState==PLAYING){
            updatePlatforms(platforms,windowWidth,windowHeight,platformSpeed,&ball,&ballPlaced,gameover);

            if(ballOnPlatform(&ball,windowHeight,platforms) || ballAtBottom(&ball,windowHeight)){
                if(!ball.isAscending && IsKeyPressed(KEY_UP)){
                    ball.velocity=-30;
                    ball.isAscending=true;
                }
            }
        //ballatedge function used to define and control the ball x poditions
            if(ballAtEdge(&ball, windowWidth)!=1){
                if(IsKeyDown(KEY_LEFT)){
                    ball.x-=20;
                }
            }
            if(ballAtEdge(&ball, windowWidth)!=2){
                if(IsKeyDown(KEY_RIGHT)){
                    ball.x+=20;
                }
            }
            ball.y+=ball.velocity;
            ball.velocity+=gravity;
            timer++;
            if((timer)%4==0){
                score++;
            }

            if(ball.y+ball.size>=windowHeight){
                gameover=true;

                // Stop the platforms from moving when game is over
            for(int i=0;i<MAX_PLATFORMS;i++){
                    platforms[i].active=false;
                }
            }
        }
        //the seletion staate allows the button to choose the level by providing respective buttons
        if (gameState==LEVEL_SELECTION){
            // Check if the Easy button is pressed
            if(CheckCollisionPointRec(mousePos, easyLevelButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                gameState=START;
            }
            // Check if the Medium button is pressed
            else if(CheckCollisionPointRec(mousePos, mediumLevelButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                gameState=START;
                platformSpeed+=1;
                
            }
            // Check if the Hard button is pressed
            else if(CheckCollisionPointRec(mousePos, hardLevelButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                gameState = START;
                platformSpeed+=2;
            
            }
        }
        //the sound game state allows the user to turn off and on the game sound
        if(gameState==SOUND){
            if(CheckCollisionPointRec(mousePos,soundonbutton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            playsound=1;
            gameState=START;
            }
            else if(CheckCollisionPointRec(mousePos,soundoffbutton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                playsound=0;
                gameState=START;
                
            }
        }
        if(playsound==1 && !IsSoundPlaying(gamesound)){
            PlaySound(gamesound);
        }
        if(playsound==0){
            StopSound(gamesound);
        }
        if(!paused){
            
            //this function starts drawing game onthe window
            BeginDrawing();
            DrawTexture(background,0,0,WHITE);
            
            
            //checks if the game state is playing then starts the game
            if (gameState==PLAYING){
                DrawCircle(ball.x,ball.y,ball.size,RED);
            
            //make the platforms active
                for (int i=0;i<MAX_PLATFORMS;i++){
                    if(platforms[i].active){
                        DrawRectangleRec(platforms[i].rect,platforms[i].color);
                    }
                }
            
            //score and time texts on the top corners of the screen 
            DrawText(TextFormat("SCORE: %d", score / 60), 10, 10, 20, BLACK);
            DrawText(TextFormat("Time: %ds", timer / 60), windowWidth - 140, 10, 20, BLACK);
            
            }

            
            
            if(gameState == START){
                DrawText("WIZZ BALL",windowWidth/2-185,100,70,GREEN);
                
                //drawing the main menu buttons
                DrawRectangleRec(playButton,GRAY);
                DrawText("Play",playButton.x+10,playButton.y+5,20,WHITE);
                DrawRectangleRec(selectLevelButton,GRAY);
                DrawText("Select Level",selectLevelButton.x+10,selectLevelButton.y+5,20,WHITE);
                DrawRectangleRec(quitbutton,GRAY);
                DrawText("Quit",quitbutton.x+10,quitbutton.y+5,20,WHITE);
                DrawRectangleRec(soundbutton,GRAY);
                DrawText("sound",soundbutton.x+10,soundbutton.y+5,20,WHITE);
            } 
            
            
            else if(gameState==LEVEL_SELECTION){
                
                // Draw level buttons only if the game state is LEVEL_SELECTION
                DrawRectangleRec(easyLevelButton,GRAY);
                DrawText("Easy",easyLevelButton.x+10,easyLevelButton.y+5,20,WHITE);

                DrawRectangleRec(mediumLevelButton,GRAY);
                DrawText("Medium",mediumLevelButton.x+10,mediumLevelButton.y+5,20,WHITE);

                DrawRectangleRec(hardLevelButton,GRAY);
                DrawText("Hard",hardLevelButton.x+10,hardLevelButton.y+5,20,WHITE);
            }
            
            
            else if(gameState==SOUND){

                //draw the sound state buttons
                DrawRectangleRec(soundoffbutton,GRAY);
                DrawText("OFF",soundoffbutton.x+10,soundoffbutton.y+5,20,WHITE);
                DrawRectangleRec(soundonbutton,GRAY);
                DrawText("ON",soundonbutton.x+10,soundonbutton.y+5,20,WHITE);
            }
            

            
            
            //the game over condition
            if(gameover){

                //showing the game over and the score of the player
                DrawText("Game Over",windowWidth/2-210,windowHeight/2-120,90,GREEN);
                DrawText(TextFormat("Your SCORE:%d",score/60),windowWidth/2-210,windowHeight/2-50,60,GREEN);

                bool replayButtonHovered=CheckCollisionPointRec(mousePos,(Rectangle){windowWidth/2-100,windowHeight/2+20,200,30});
                bool quitButtonHovered=CheckCollisionPointRec(mousePos, (Rectangle){windowWidth/2-100,windowHeight/2+90,200,30});
                
                //draw the game over buttons
                DrawRectangle(windowWidth/2-100,windowHeight/2+20,200,30,replayButtonHovered?RED:GRAY);
                DrawText("Replay",windowWidth/2-50,windowHeight/2+25,20,WHITE);

                DrawRectangle(windowWidth/2-100,windowHeight/2+90,200,30,quitButtonHovered?RED:GRAY);
                DrawText("Quit",windowWidth/2-35,windowHeight/2+95,20,WHITE);
                
                //reply button restarts the game
                if(replayButtonHovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                    ballPlaced=false;
                    ball.size=20.0;
                    ball.velocity=1;
                    ball.isAscending=false;
                    timer=0;
                    gameover=false;
                    score=0;
                    paused=false;

                    for (int i= 0;i<MAX_PLATFORMS;i++){
                        platforms[i].active=false;
                    }
                }
            
            //quit button ends the game to quit
                if (quitButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                    break;
                }
            }

            EndDrawing();
        }
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
