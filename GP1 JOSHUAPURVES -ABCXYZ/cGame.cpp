/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"
#include "cButtonMgr.h"
#include "cButton.h"
#include <sstream>


cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	//sets score to 0
	score = 0;
	//play bool begins as off to stop game playing from start
	play = false;
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	thePencil.setSpritePos({ 400, 650 });
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	theAreaClicked = { 0, 0 };
	// Store the textures
	textureName = { "x square", "y square", "z square", "ball","thePencil" , "theBackground" , "theEnd" , "thePlay" };
	texturesToUse = { "Images\\x square.png", "Images\\y square.png", "Images\\z square.png", "Images\\ball.png", "Images\\shooter game.png", "Images/intro.jpg" , "Images/game over.jpg" , "Images/background.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Store the textures
	btnNameList = { "exit_btn", "instructions_btn", "replay_btn", "menu_btn", "play_btn", "save_btn" , "mainmenu_btn" };
	btnTexturesToUse = { "Images/Buttons/exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button1.png", "Images/Buttons/button_menu.png", "Images/Buttons/play.png", "Images/Buttons/button_save.png", "Images/Buttons/button2.png" };
	btnPos = { { 700, 650 },{ 400, 300 },{ 400, 300 },{ 500, 500 },{ 700, 400 },{ 740, 500 },{ 400, 300 } };
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	//button creation
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	//game starts at the menu screen
	theGameState = MENU;
	theBtnType = EXIT;
	// Create textures for Game Dialogue (text)
	fontList = { "point", "bbe" , };
	fontsToUse = { "Fonts/score.ttf", "Fonts/Behind Blue Eyes.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 38);
	}
	gameTextList = { "ABCXYZ - The Game", "The Score: 0" , "GAME OVER", "Time Left: 0", "Latest scores:"  };
	//creating array of text to call
	theTextureMgr->addTexture("Title", theFontMgr->getFont("point")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("score", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("gameover", theFontMgr->getFont("point")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("time", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[3], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("latestscore", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme" , "boop", "laser", "click", "pencil" };
	soundTypes = { MUSIC, SFX, SFX, SFX, SFX };
	soundsToUse = { "Audio/8bit2.wav" , "Audio/boop.wav", "Audio/laser.wav", "Audio/click.wav", "Audio/pencil.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	//gets the sound saved as theme and loops it continuously.
	theSoundMgr->getSnd("theme")->play(-1);
	//outputs the latest score saved to file
	ifstream scoreSave;
	string latestscore;
	scoreSave.open("latestscore.Txt");
	scoreSave.clear();
	scoreSave >> latestscore;
	scoreSave.close();
	gameTextList[4] = latestscore.c_str();
	theTextureMgr->addTexture("latestscore", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 228, 213, 238, 255 }, { 0, 0, 0, 0 }));

	//sets sprite background to centre at back.
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	//game is running
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}



void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	switch (theGameState)
	{
		//RENDER CHANGES DEPENDING ON WHAT STATE THE GAME IS IN.
	case MENU:
	{
		//renders background
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render Buttons
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 700, 475 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	//All code effects PLAYING state
	case PLAYING:
	{
		{
			// renders background
			spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

			//RENDER EACH LETTER BLOCK IN VECTOR ARRAY
			for (int draw = 0; draw < theLetters.size(); draw++)
			{
				theLetters[draw]->render(theRenderer, &theLetters[draw]->getSpriteDimensions(), &theLetters[draw]->getSpritePos(), theLetters[draw]->getSpriteRotAngle(), &theLetters[draw]->getSpriteCentre(), theLetters[draw]->getSpriteScale());
			}
			// Render each ball in the vector array
			for (int draw = 0; draw < theball.size(); draw++)
			{
				theball[draw]->render(theRenderer, &theball[draw]->getSpriteDimensions(), &theball[draw]->getSpritePos(), theball[draw]->getSpriteRotAngle(), &theball[draw]->getSpriteCentre(), theball[draw]->getSpriteScale());
			}
			//renders the pencil and renders what position it currently is.
			thePencil.setTexture(theTextureMgr->getTexture("thePencil"));
			thePencil.setSpriteDimensions(theTextureMgr->getTexture("thePencil")->getTWidth(), theTextureMgr->getTexture("thePencil")->getTHeight());
			FPoint scale = { 0, 0 };
			thePencil.setPencilVelocity({ 1, 1 });
			spriteBkgd.setSpritePos({ 0, 0 });
			//renders the Title
			cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
			SDL_Rect pos = { 285, 0, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 1, 1 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			//render score
			if (scoreChanged)
			{
				gameTextList[1] = ScoreAsString.c_str();
				theTextureMgr->addTexture("score", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
				scoreChanged = false;
			}
			//creates score texture
			tempTextTexture = theTextureMgr->getTexture("score");
			pos = { 200, 55, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 1, 1 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			// render the rocket
			thePencil.render(theRenderer, &thePencil.getSpriteDimensions(), &thePencil.getSpritePos(), thePencil.getSpriteRotAngle(), &thePencil.getSpriteCentre(), thePencil.getSpriteScale());
			spriteBkgd.setTexture(theTextureMgr->getTexture("thePlay"));
			spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("thePlay")->getTWidth(), theTextureMgr->getTexture("thePlay")->getTHeight());
			theButtonMgr->getBtn("exit_btn")->setSpritePos({ 800, 700 });
			theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		}
		//Checks if there is a latest score clears the old data
		//and renders it on screen
		if (latestscores)
		{
			//checks if txt latest score is erased if it isn't it erases it
			if (theTextureMgr->getTexture("latestscore") != NULL)
			{
				theTextureMgr->deleteTexture("latestscore");
			}
			ifstream scoreSave;
			string latestscore;
			scoreSave.open("latestscores.txt");
			scoreSave.clear();
			scoreSave >> latestscore;
			scoreSave.close();
			gameTextList[4] = latestscore.c_str();
			theTextureMgr->addTexture("latestscore", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
			latestscores = false;
		}
		//renders latest score
		tempTextTexture = theTextureMgr->getTexture("latestscore");
		pos = { 600 ,55, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	}
	break;
	//all code effects END state
	case END:
	{
		//checks if latestscores is false, saves the latest score to the txt file
		if (!latestscores)
		{
			//changes score int to string
			string thescore = to_string(score);
			ScoreAsString = "Latest-Score:" + thescore;
			ofstream scoreSave;
			//opens txt file and and inputs string
			scoreSave.open("latestscores.txt");
			scoreSave.clear();
			scoreSave << NewScoreAsString;
			scoreSave.close();
			latestscores = true;
		}
		//renders background
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		//checks if txt latest score is erased if it isn't it erases it
		if (theTextureMgr->getTexture("latestscore") != NULL)
		{
			theTextureMgr->deleteTexture("latestscore");
		}
		//takes the information from the file and adds the texture
		ifstream scoreSave;
		string latestscore;
		scoreSave.open("latestscores.txt");
		scoreSave.clear();
		scoreSave >> latestscore;
		scoreSave.close();
		gameTextList[4] = latestscore.c_str();
		theTextureMgr->addTexture("latestscore", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[4], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
		latestscores = false;

		//RENDER NEWSCORE
		if (newscoreChanged)
		{
			gameTextList[4] = NewScoreAsString.c_str();
			theTextureMgr->addTexture("latestscore", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
			newscoreChanged = false;
		}
		//creates temptexture for gametext of latestscore
		tempTextTexture = theTextureMgr->getTexture("latestscore");
		pos = { 425, 350, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		//creates temptexture for gametext of gameover
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("gameover");
		pos = { 378, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 60, 60 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		//renders background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("theEnd"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theEnd")->getTWidth(), theTextureMgr->getTexture("theEnd")->getTHeight());
		//spawns in replay button
		theButtonMgr->getBtn("replay_btn")->setSpritePos({ 180, 600 });
		theButtonMgr->getBtn("replay_btn")->render(theRenderer, &theButtonMgr->getBtn("replay_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("replay_btn")->getSpritePos(), theButtonMgr->getBtn("replay_btn")->getSpriteScale());
		//spawns in exit button
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 680, 600 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());

	}

	break;
	case QUIT:
	{
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{
	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	//switches between states
	switch (theGameState)
	{
	//effects everything that happens in the MENU gamestate
	case MENU:
	{
		//sets play bool to false
		play = false;
		//creates play button and changes it to PLAYING gamestate
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
		//creates exit button and changes it to quit
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	break;
	//effects everything that happens in the PLAYING gamestate
	case PLAYING:
	{
		//creates exit button and changes it to end gamestate
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
		//resets the last thing clicked allowing replay or exit to be clicked again
		theAreaClicked = { 0, 0 };
		//if its not play
		if (!play)
		{
			//sets int to 0
			latestscore = 0;
			//sets int to 0
			score = 0;
			//clears the letters
			theLetters.clear();
			// an array to create the letters across the screen and move them in their direction
			for (int ltr = 0; ltr < 10; ltr++)
			{
				theLetters.push_back(new cLetter);
				theLetters[ltr]->setSpritePos({ ltr * 100 , 100 });
				theLetters[ltr]->setSpriteTranslation({ 10, 60 });
				int randLetter = rand() % 3;
				theLetters[ltr]->setTexture(theTextureMgr->getTexture(textureName[randLetter]));
				theLetters[ltr]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randLetter])->getTWidth(), theTextureMgr->getTexture(textureName[randLetter])->getTHeight());
				theLetters[ltr]->setLetterVelocity({ 1, 1 });
				theLetters[ltr]->setActive(true);
			}
			//play is equal to true
			play = true;
		}

		// Update the visibility and position of each letter
		vector<cLetter*>::iterator letterIterator = theLetters.begin();
		while (letterIterator != theLetters.end())
		{
			if ((*letterIterator)->isActive() == false)
			{
				letterIterator = theLetters.erase(letterIterator);
			}
			else
			{
				(*letterIterator)->update(deltaTime);
				++letterIterator;
			}
		}

		// Update the visibility and position of each bullet
		vector<cBall*>::iterator ballIterartor = theball.begin();
		while (ballIterartor != theball.end())
		{
			if ((*ballIterartor)->isActive() == false)
			{
				ballIterartor = theball.erase(ballIterartor);
			}
			else
			{
				(*ballIterartor)->update(deltaTime);
				++ballIterartor;
			}
		}

		for (vector<cBall*>::iterator ballIterartor = theball.begin(); ballIterartor != theball.end(); ++ballIterartor)
		{
			//(*bulletIterartor)->update(deltaTime);
			for (vector<cLetter*>::iterator letterIterator = theLetters.begin(); letterIterator != theLetters.end(); ++letterIterator)
			{
				if ((*letterIterator)->collidedWith(&(*letterIterator)->getBoundingRect(), &(*ballIterartor)->getBoundingRect()))
				{
					// adds 5 to score int
					score += 5;
					//adds 5 to latestscore int 
					latestscore += 5;
					//checks if score texture is erased if not it is then deleted
					if (theTextureMgr->getTexture("score") != NULL)
					{
						theTextureMgr->deleteTexture("score");
					}
					//plays the boop sound
					theSoundMgr->getSnd("boop")->play(0);
					//changes score int to string
					string thescore = to_string(score);
					ScoreAsString = "The Score: " + thescore;
					//changes latestscore int to string
					string newscore = to_string(latestscore);
					NewScoreAsString = "Latest-Score:" + newscore;
					newscoreChanged = true;
					scoreChanged = true;
					// if a collision set the letter and ball to false
					(*letterIterator)->setActive(false);
					(*ballIterartor)->setActive(false);

				}
			}
		}
		// if score is 40 or greater...
		if (score >= 40)
		{
			//change gamestate END
			theGameState = END;
		}
		// Update the Rockets position
		thePencil.update(deltaTime);
	}
	break;
	//effects everything that happens in the END gamestate
	case END:
	{
		//sets score to 0
		score = 0;
		//checks if score texture is erased if not it is then deleted
		if (theTextureMgr->getTexture("score") != NULL)
		{
			theTextureMgr->deleteTexture("score");
		}
		//sets score int to string
		string thescore = to_string(score);
		ScoreAsString = "The Score: " + thescore;
		scoreChanged = true;
		//play becomes false
		play = false;
		//replay button takes the game to PLAYING state
		theGameState = theButtonMgr->getBtn("replay_btn")->update(theGameState, PLAYING, theAreaClicked);
		//exit button takes the game to quit
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	break;
	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
				//left click plays click sound along with picking up where click occured.
			case SDL_BUTTON_LEFT:
			{
				theSoundMgr->getSnd("click")->play(0);
				theAreaClicked = { event.motion.x, event.motion.y };

			}
			break;
			case SDL_BUTTON_RIGHT:
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
				theAreaClicked = { event.motion.x, event.motion.y };
			}
			break;
			case SDL_BUTTON_RIGHT:
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
		{
		}
		break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				theLoop = false;
				break;
			case SDLK_DOWN:
			{
			}
			break;

			case SDLK_UP:
			{
			}
			break;
			case SDLK_LEFT:
			{
				//if left arrow clicked pencil moves left & sound played
				thePencil.setPencilVelocity({ -300, 0 });
				theSoundMgr->getSnd("pencil")->play(0);
			}
			break;
			case SDLK_RIGHT:
			{
				//if right arrow clicked pencil moves right & sound played 
				thePencil.setPencilVelocity({ 300, 0 });
				theSoundMgr->getSnd("pencil")->play(0);
			}
			break;
			case SDLK_d:
			{
				//ROTATES PENCIL
				thePencil.setSpriteRotAngle(thePencil.getSpriteRotAngle() + 5);
			}
			break;

			case SDLK_a:
			{
				//ROTATES PENCIL
				thePencil.setSpriteRotAngle(thePencil.getSpriteRotAngle() - 5);
			}
			break;
			case SDLK_SPACE:
			{
				theSoundMgr->getSnd("laser")->play(0);
				//FIRES BALL AT ANGLE RELATIVE TO PENCIL
				theball.push_back(new cBall);
				int numBullets = theball.size() - 1;
				theball[numBullets]->setSpritePos({ thePencil.getBoundingRect().x + thePencil.getSpriteCentre().x, thePencil.getBoundingRect().y + thePencil.getSpriteCentre().y });
				theball[numBullets]->setSpriteTranslation({ 10, 10 });
				theball[numBullets]->setTexture(theTextureMgr->getTexture("ball"));
				theball[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("ball")->getTWidth(), theTextureMgr->getTexture("ball")->getTHeight());
				theball[numBullets]->setBallVelocity({ 10, 10 });
				theball[numBullets]->setSpriteRotAngle(thePencil.getSpriteRotAngle());
				theball[numBullets]->setActive(true);
				cout << "Ball added to Vector at position - x: " << thePencil.getBoundingRect().x << " y: " << thePencil.getBoundingRect().y << endl;


			}
			break;
			default:
				break;
			}

		default:
			break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

