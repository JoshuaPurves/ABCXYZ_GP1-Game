/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();


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
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Store the textures
	textureName = { "x square", "y square", "z square", "ball","thePencil" ,"theBackground" };
	texturesToUse = { "Images\\x square.png", "Images\\y square.png", "Images\\z square.png", "Images\\ball.png", "Images\\shooter game.png", "Images\\background.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "bbe" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/Behind Blue Eyes.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "ABCXYZ - The Game", "The Score: 0" };

	theTextureMgr->addTexture("Title", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("score", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "fire", "destroy" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/8bit2.wav", "Audio/fire.wav", "Audio/destroy.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	thePencil.setSpritePos({ 400, 650 });
	thePencil.setTexture(theTextureMgr->getTexture("thePencil"));
	thePencil.setSpriteDimensions(theTextureMgr->getTexture("thePencil")->getTWidth(), theTextureMgr->getTexture("thePencil")->getTHeight());
	FPoint scale = { 1, 1 };
	thePencil.setPencilVelocity({ 0, 0 });

	// Create vector array of textures
	
	//int someArray[4][10];
	//for(int y=0;y< 10; y++)
	//{
		//for (int x = 0; x < 4, x++)
		//{
			//someArray[x][y] = randletter;
		//}
	for (int ltr = 0; ltr < 10; ltr++)
	{

		theLetters.push_back(new cLetter);
		theLetters[ltr]->setSpritePos({ ltr * 100 , 100 });
		theLetters[ltr]->setSpriteTranslation({ 10, 59 });
		int randLetter = rand() % 3;
		theLetters[ltr]->setTexture(theTextureMgr->getTexture(textureName[randLetter]));
		theLetters[ltr]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randLetter])->getTWidth(), theTextureMgr->getTexture(textureName[randLetter])->getTHeight());
		theLetters[ltr]->setLetterVelocity({ 1, 1 });
		theLetters[ltr]->setActive(true);



	}


}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	bool loop = true;

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
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	// Render each letter in the vector array
	for (int draw = 0; draw < theLetters.size(); draw++)
	{
		theLetters[draw]->render(theRenderer, &theLetters[draw]->getSpriteDimensions(), &theLetters[draw]->getSpritePos(), theLetters[draw]->getSpriteRotAngle(), &theLetters[draw]->getSpriteCentre(), theLetters[draw]->getSpriteScale());
	}
	// Render each ball in the vector array
	for (int draw = 0; draw < theball.size(); draw++)
	{
		theball[draw]->render(theRenderer, &theball[draw]->getSpriteDimensions(), &theball[draw]->getSpritePos(), theball[draw]->getSpriteRotAngle(), &theball[draw]->getSpriteCentre(), theball[draw]->getSpriteScale());
	}
	// Render the Title
	cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
	SDL_Rect pos = { 350, 0, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	FPoint scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	//render score

	if (scoreChanged)

	{
		gameTextList[1] = ScoreAsString.c_str();
		theTextureMgr->addTexture("score", theFontMgr->getFont("bbe")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }));
		scoreChanged = false;
	}
	tempTextTexture = theTextureMgr->getTexture("score");
	pos = { 400, 25, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

	// render the rocket
	thePencil.render(theRenderer, &thePencil.getSpriteDimensions(), &thePencil.getSpritePos(), thePencil.getSpriteRotAngle(), &thePencil.getSpriteCentre(), thePencil.getSpriteScale());
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
	// Update the visibility and position of each asteriod
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
				// if a collision set the bullet and asteroid to false

				score += 5;
				theTextureMgr->deleteTexture("score");

				string thescore = to_string(score);
				ScoreAsString = "score: " + thescore;

				scoreChanged = true;

				(*letterIterator)->setActive(false);
				(*ballIterartor)->setActive(false);
				theSoundMgr->getSnd("destroy")->play(0);
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
				}
				break;
				case SDL_BUTTON_RIGHT:

					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_LEFT:
				{
					thePencil.setPencilVelocity({ -90, 0 });
				}
				break;
				case SDLK_RIGHT:
				{
					thePencil.setPencilVelocity({ +120, 0 });
				}
				break;
				case SDLK_d:
				{
					thePencil.setSpriteRotAngle(thePencil.getSpriteRotAngle() + 5);
				}
				break;

				case SDLK_a:
				{
					thePencil.setSpriteRotAngle(thePencil.getSpriteRotAngle() - 5);
				}
				break;
				case SDLK_SPACE:
				{
					theball.push_back(new cBall);
					int numBullets = theball.size() - 1;
					theball[numBullets]->setSpritePos({ thePencil.getBoundingRect().x + thePencil.getSpriteCentre().x, thePencil.getBoundingRect().y + thePencil.getSpriteCentre().y });
					theball[numBullets]->setSpriteTranslation({ 2, 2 });
					theball[numBullets]->setTexture(theTextureMgr->getTexture("ball"));
					theball[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("ball")->getTWidth(), theTextureMgr->getTexture("ball")->getTHeight());
					theball[numBullets]->setBallVelocity({ 2, 2 });
					theball[numBullets]->setSpriteRotAngle(thePencil.getSpriteRotAngle());
					theball[numBullets]->setActive(true);
					cout << "Ball added to Vector at position - x: " << thePencil.getBoundingRect().x << " y: " << thePencil.getBoundingRect().y << endl;
				}
				theSoundMgr->getSnd("fire")->play(0);
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

