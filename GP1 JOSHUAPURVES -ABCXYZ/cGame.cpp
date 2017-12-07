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

	// Set filename
	//theFile.setFileName("Data/usermap.dat");
	// Check file is available
	//	if (!theFile.openFile(ios::in)) //open file for input output
	//{
	//	cout << "Could not open specified file '" << theFile.getFileName() << "'. Error " << SDL_GetError() << endl;
	//	fileAvailable = false;
	//	}
	//	else
	//	{
	//		cout << "File '" << theFile.getFileName() << "' opened for input!" << endl;
	//		fileAvailable = true;
	//	}
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
	gameTextList = { "ABCXYZ - The Game", "The Score: 0" , "GAME OVER", "Time Left: 0" };
	//creating array of text to call
	theTextureMgr->addTexture("Title", theFontMgr->getFont("point")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("score", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("gameover", theFontMgr->getFont("point")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("time", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[3], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));

	//IF PLAY BOOL IS TRUE, CREATE THE LETTERS AND MOVE THEIR POSITION DOWN. RANDOMISE WHICH SPRITE USED
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

	// Load game sounds
	soundList = { "theme" , "boop", "beep" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/8bit2.wav" , "Audio/boop.wav", "Audio/beepbeep.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	//gets the sound saved as theme and loops it continuously.
	theSoundMgr->getSnd("theme")->play(-1);

	//sets sprite background to centre at back.
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
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
		//RENDER CHANGES DEPENDING ON WGAR STATE THE GAME IS IN.
	case MENU:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render Buttons
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 700, 475 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case PLAYING:
	{
		{
			spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

			//	if (fileAvailable && theGameState == PLAYING)
			//	FileSave.loadlastgame(&theFile);
			//	theGameState = PLAYING;
			//		theAreaClicked = { 0, 0 };
			//	}
			//theGameState = theButtonMgr->getBtn("save_btn")->update(theGameState, SAVEMAP, theAreaClicked);
			//if (theGameState == SAVEMAP)
			//{
			// Check file is available
			//	if (!theFile.openFile(ios::out)) //open file for output
			//	{
			//		cout << "Could not open specified file '" << theFile.getFileName() << "'. Error " << SDL_GetError() << endl;
			//	}
			//	else
			//	{
			//		cout << "File '" << theFile.getFileName() << "' opened for output!" << endl;
			//		theTileMap.writeMapDataToFile(&theFile);
			//	}

			//theTileMap.writeMapDataToFile(&theFile);
			//	theGameState = PLAYING;
			//	theAreaClicked = { 0, 0 };
			//}

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
			pos = { 150, 55, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 1, 1 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			//RENDER TIME
			gameTextList[3] = TimeAsString.c_str();
			theTextureMgr->addTexture("time", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[3], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));

			//creates time texture
			tempTextTexture = theTextureMgr->getTexture("time");
			pos = { 700, 55, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			scale = { 1, 1 };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

			// render the rocket
			thePencil.render(theRenderer, &thePencil.getSpriteDimensions(), &thePencil.getSpritePos(), thePencil.getSpriteRotAngle(), &thePencil.getSpriteCentre(), thePencil.getSpriteScale());
			spriteBkgd.setTexture(theTextureMgr->getTexture("thePlay"));
			spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("thePlay")->getTWidth(), theTextureMgr->getTexture("thePlay")->getTHeight());
			theButtonMgr->getBtn("exit_btn")->setSpritePos({ 800, 700 });
			theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		}
	}

	break;
	case END:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		//RENDER SCORE
		if (scoreChanged)
		{
			gameTextList[1] = ScoreAsString.c_str();
			theTextureMgr->addTexture("score", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 255, 0 }, { 0, 0, 0, 0 }));
			scoreChanged = false;
		}
		tempTextTexture = theTextureMgr->getTexture("score");
		pos = { 425, 350, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("gameover");
		pos = { 378, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 60, 60 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		//RENDER BUTTONS
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("theEnd"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theEnd")->getTWidth(), theTextureMgr->getTexture("theEnd")->getTHeight());
		theButtonMgr->getBtn("replay_btn")->setSpritePos({ 180, 600 });
		theButtonMgr->getBtn("replay_btn")->render(theRenderer, &theButtonMgr->getBtn("replay_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("replay_btn")->getSpritePos(), theButtonMgr->getBtn("replay_btn")->getSpriteScale());
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
	// Check Button clicked and change state
	if (theGameState == MENU || theGameState == END)
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	else
	{
		//if its not END OR MENU the game begins
		play = true;
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
	}
	theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
	theGameState = theButtonMgr->getBtn("replay_btn")->update(theGameState, PLAYING, theAreaClicked);


	if (theGameState == END)
	{
		play = false;
	}
	{
		if (play == true)
		{

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
		}

		if (play == true)
		{
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
		}

		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/
		for (vector<cBall*>::iterator ballIterartor = theball.begin(); ballIterartor != theball.end(); ++ballIterartor)
		{
			//(*bulletIterartor)->update(deltaTime);
			for (vector<cLetter*>::iterator letterIterator = theLetters.begin(); letterIterator != theLetters.end(); ++letterIterator)
			{
				if ((*letterIterator)->collidedWith(&(*letterIterator)->getBoundingRect(), &(*ballIterartor)->getBoundingRect()))
				{

					// if a collision set the letter and ball to false
					score += 5;
					if (theTextureMgr->getTexture("score") != NULL)
					{
						theTextureMgr->deleteTexture("score");
					}

					string thescore = to_string(score);
					ScoreAsString = "Score: " + thescore;

					scoreChanged = true;

					string theTime = to_string(time);
					TimeAsString = " Time Left" + time;

					//timecount = true;
					
					(*letterIterator)->setActive(false);
					(*ballIterartor)->setActive(false);

				}
			}
		}
	}


	// Update the Rockets position
	thePencil.update(deltaTime);
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
				//if left arrow clicked pencil moves left
				thePencil.setPencilVelocity({ -300, 0 });
			}
			break;
			case SDLK_RIGHT:
			{
				//if right arrow clicked pencil moves right
				thePencil.setPencilVelocity({ 300, 0 });
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
				theSoundMgr->getSnd("boop")->play(0);
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

